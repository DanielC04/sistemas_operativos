#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number(){
	return (rand() % 50);
}

int hijo(int id, int n, bool es_primer_hijo, int pipe_ids[][2]){
	printf("soy hijo %d\n", id);
	int received_num;
	int numero_secreto;
	if (es_primer_hijo) {
		numero_secreto = generate_random_number();
		printf("el primer proceso generó el numero secreto %d\n", numero_secreto);
	}
	bool es_primer_iteration = true;
	while (true){
		int pipe_id = (id - 1 + n) % n;
		if (es_primer_hijo && es_primer_iteration){
			pipe_id = n;
			es_primer_iteration = false;
		}
		read(pipe_ids[pipe_id][PIPE_READ], &received_num, sizeof(received_num));
		printf("Proceso %d received numero %d\n", id, received_num);
		// proceso inical tiene que fijarse si el numero recibido ya es mas grande que el numero secreto
		if (es_primer_hijo && (received_num > numero_secreto)) {
			printf("terminando programa pq soy el primer hijo y el numeró %d es mas grande que mi numero secreto %d\n", received_num, numero_secreto);
			// mandar ultimo numero al padre
			write(pipe_ids[n + 1][PIPE_WRITE], &received_num, sizeof(received_num));
			exit(1);
		}
		received_num++;
		write(pipe_ids[(id + n) % n][PIPE_WRITE], &received_num, sizeof(received_num));
	}
}

int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	int status, n, start, buffer;
	n = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);

	// crear a n pipes
	
	// Declare pipe_ids as a 2D array
	int pipe_ids[n + 2][2];
	// n + 1-th pipe se usa para mandar del padre al primer hijo
	// n + 2-th pipe se usa para mandar del primer hijo al padre
	for(int i = 0; i < n + 2; i ++){
		pipe(pipe_ids[i]);
	}

	// crear a n hijos
	int pid;
	for (int process_id = 0; process_id < n; process_id ++){
		pid = fork();
		if (pid == 0){
			hijo(process_id, n, (start == process_id) ,pipe_ids);
			break;
		}
	}
	if (pid != 0){
		// proceso padre
		// mandar primer mensaje al primer hijo
		write(pipe_ids[n][PIPE_WRITE], &buffer, sizeof(buffer));
		// esperar el numero final
		int numero_final;
		read(pipe_ids[n + 1][PIPE_READ], &numero_final, sizeof(numero_final));
		printf("proceso padre termina y recibió el numero final de %d\n", numero_final);
	}
}
