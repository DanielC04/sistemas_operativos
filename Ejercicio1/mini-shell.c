#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

#define READ 0
#define WRITE 1

int hijo(int id, int** pipes, char ***progs, size_t count)
{
	// proceso hijo numero i
	close(pipes[0][READ]);
	close(pipes[count - 1][WRITE]);
	if (id == 0) dup2(pipes[0][WRITE], STD_OUTPUT); else close(pipes[0][WRITE]); // Redirigir la salida estándar a la escritura del pipe
	if (id == (count - 1)) dup2(pipes[id][READ], STD_INPUT); else close(pipes[count - 1][READ]); // Redirigir la entrada estándar a la lectura del pipe
	
	if(id != 0 && id != (count - 1)){
		// proceso i lee del proces i - 1
		dup2(pipes[id][READ], STD_INPUT);
		dup2(pipes[id][WRITE], STD_OUTPUT);
	}

	for (int pipe_id = 1; pipe_id < count - 1; pipe_id++)
	{
		if (pipe_id != (id - 1)) close(pipes[pipe_id][READ]);
		if (pipe_id != id) close(pipes[pipe_id][WRITE]);
	}
	
	execvp(progs[id][0], progs[id]);

}

static int run(char ***progs, size_t count)
{
	int r, status;

	// Reservo memoria para el arreglo de pids
	int pipes[count - 1][2];
	// crear los n - 1 pipes
	for (int i = 0; i < count - 1; i++) pipe(pipes[i]);

	// TODO: Guardar el PID de cada proceso hijo creado en children[i]
	pid_t *children = malloc(sizeof(*children) * count);
	pid_t pid;

	int i;
	for (i = 0; i < count; i++) {
		pid = fork();
		if (pid != 0) {
			// proceso padre
			children[i] = pid;
		} else {
			hijo(i, pipes, progs, count);
			// proceso hijo
			break;
		}
	}
	if (pid != 0) {
		for (int l = 0; l < count - 1; l++) {
			close(pipes[l][READ]);
			close(pipes[l][WRITE]);
		}
	}

	// Espero a los hijos y verifico el estado que terminaron
	for (int i = 0; i < count; i++)
	{
		waitpid(children[i], &status, 0);

		if (!WIFEXITED(status))
		{
			fprintf(stderr, "proceso %d no terminó correctamente [%d]: ",
					(int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}
	r = 0;
	free(children);

	return r;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("El programa recibe como parametro de entrada un string con la linea de comandos a ejecutar. \n");
		printf("Por ejemplo ./mini-shell 'ls -a | grep anillo'\n");
		return 0;
	}
	int programs_count;
	char ***programs_with_parameters = parse_input(argv, &programs_count);

	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}
