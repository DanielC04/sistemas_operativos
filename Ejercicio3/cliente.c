#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
  int server_socket;
  struct sockaddr_un server_addr;

  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, "unix_socket");

  server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (connect(server_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) == -1) {
    perror("Error");
    exit(1);
  }

  char operacion[20];
  while (1) {
    printf("ingresá una operacion para resolver: ");
    scanf("%s", operacion);

    if (!strcmp(operacion, "exit")) {
      printf("el client se muere porque recibio exit\n");
      close(server_socket);
      exit(1);
    }

    int resultado;
    write(server_socket, &operacion, sizeof(operacion));
    read(server_socket, &resultado, sizeof(resultado));
    printf("Cliente: recibí el resultado %d del servidor!\n", resultado);
  }

  close(server_socket);
  // Completar
  exit(0);
}
