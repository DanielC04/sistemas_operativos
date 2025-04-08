#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

#define READ 0
#define WRITE 1

int hijo(int id, int pipes[][2], char ***progs, size_t count)
{
	// Si soy el primer proceso redirijo el extremo de escritura del pipe[0] a STDOUT
	if(id == 0) dup2(pipes[id][WRITE],STDOUT_FILENO);
	
	// Si soy el ultimo proceso redirijo el extremo de lectura del pipe[count - 2] a STDIN
	if(id == count - 1) dup2(pipes[id - 1][READ],STDIN_FILENO);
	
    // Si no soy el primer proceso o el ultimo redirijo el extremo de escritura del pipe actual a STDOUT
	// y el extremo de lectura del pipe anterior a STDIN.
    if (id != 0 && id != count - 1) {
        dup2(pipes[id - 1][READ], STDIN_FILENO);
		dup2(pipes[id][WRITE], STDOUT_FILENO);
    }


	// Creo que no hace falta separar en casos,tipo podriamos excluirlos y ya esta:
	// if (id != 0 ) dup2(pipes[id - 1][READ], STDIN_FILENO);
	// if (id != count - 1 ) dup2(pipes[id][WRITE], STDOUT_FILENO);


    // Cierro todas las refencias(Solo mantengolas referencias de STDIN y STDOUT de cada proceso)
    for (int i = 0; i < count - 1; i++) {
        close(pipes[i][READ]);
        close(pipes[i][WRITE]);
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


}



