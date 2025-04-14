#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

int calcular(const char *expresion) {
  int num1, num2, resultado;
  char operador;

  // Usamos sscanf para extraer los dos números y el operador de la expresión
  if (sscanf(expresion, "%d%c%d", &num1, &operador, &num2) != 3) {
    printf("Formato incorrecto\n");
    return 0; // En caso de error, retornamos 0.
  }

  // Realizamos la operación según el operador
  switch (operador) {
  case '+':
    resultado = num1 + num2;
    break;
  case '-':
    resultado = num1 - num2;
    break;
  case '*':
    resultado = num1 * num2;
    break;
  case '/':
    if (num2 != 0) {
      resultado = num1 / num2;
    } else {
      printf("Error: División por cero\n");
      return 0; // Si hay división por cero, retornamos 0.
    }
    break;
  default:
    printf("Operador no reconocido\n");
    return 0; // Si el operador no es válido, retornamos 0.
  }

  return resultado;
}

void servidor(int client_socket) {
  char operacion[20];
  while (recv(client_socket, &operacion, sizeof(operacion), 0)) {
    printf("Servidor: recibí la operacion %s del cliente!\n", operacion);
    int num = calcular(operacion);
    send(client_socket, &num, sizeof(num), 0);
  }
  exit(0);
}

int main() {
  int server_socket;
  int client_socket;
  struct sockaddr_un server_addr;
  struct sockaddr_un client_addr;
  uint slen = sizeof(server_addr);
  uint clen = sizeof(client_addr);

  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, "unix_socket");
  unlink(server_addr.sun_path);

  server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  bind(server_socket, (struct sockaddr *)&server_addr, slen);
  listen(server_socket, 1);

  printf("Servidor: esperando conexión del cliente...\n");
  while (1) {
    client_socket =
        accept(server_socket, (struct sockaddr *)&client_addr, &clen);
    int pid = fork();
    if (pid == 0) {
      servidor(client_socket);
      break;
    }
  }
  close(client_socket);
  close(server_socket);
  exit(0);
}