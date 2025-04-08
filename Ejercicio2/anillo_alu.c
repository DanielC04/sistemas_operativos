#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number(){
	return (rand() % 50);
}

int hijo(int id, int n, int numero_inicial, int proceso_inicial, int** pipe_ids){
	int received_num;
	if (proceso_inicial == id) {
		received_num = numero_inicial;
	} else {
		received_num = read(pipe_ids[(id - 1 + n) % n], received_num, sizeof(received_num));
	}

	write(pipe_ids[id % n][PIPE_WRITE], (received_num + 1), sizeof(numero_inicial));
}

int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	int status, pid, n, start, buffer;
	n = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);

	// crear a n pipes
	int** pipe_ids[n][2];
	for(int i = 0; i < n; i ++){
		pipe(pipe_ids[i]);
	}

	// crear a n hijos
	pid_t* pids[n];
	pid_t pid;
	int process_id;
	for (process_id = 0; process_id < n; process_id ++){
		pid = fork();
		if (pid == 0) hijo(process_id, n, buffer, start, pipe_ids);
		else pids[process_id] = pid;
	}
}
