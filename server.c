#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void handle_client(int client_socket) {
  char request[BUFFER_SIZE];
  int bytes_received = recv(client_socket, request, BUFFER_SIZE, 0);

  if (bytes_received < 0) {
    perror("recv failed");
    close(client_socket);
    return;
  }

  char *method = strtok(request, " ");
  char *path = strtok(NULL, " ");

  char response[BUFFER_SIZE];
  if (strcmp(method, "GET") == 0) {
    FILE *file = fopen(path, "rb");
    if (file) {
      FILE *html = fopen("./index.html", "r");
      char *my_html = (char *)malloc(BUFFER_SIZE);

      strcpy(response, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n");
      size_t buffer_read = 0;
      int c;
      while ((c = fgetc(html)) != EOF) {
        if (buffer_read >= BUFFER_SIZE) {
          my_html = (char *)realloc(my_html, buffer_read + 10);
          if (!my_html) {
            perror("Memory allocation failed");
            fclose(html);
            return;
          }
        }
        my_html[buffer_read++] = c;
      }

      my_html[buffer_read] = '\0'; // NULL

      int bytes_read;
      while ((bytes_read = fread(response + strlen(response), 1,
                                 BUFFER_SIZE - strlen(response), file)) > 0) {
        // nose
      }
      fclose(html);
      fclose(file);
      free(my_html);
    } else {
      sprintf(response,
              "HTTP/1.1 404 Not Found\nContent-Type: text/plain\n\nNot Found");
    }
  } else {
    sprintf(response, "HTTP/1.1 501 Not Implemented\nContent-Type: "
                      "text/plain\n\nMethod not supported");
  }

  send(client_socket, response, strlen(response), 0);

  close(client_socket);
}

int main() {
  int server_socket;
  struct sockaddr_in server_address;

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("socket failed");
    exit(1);
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT);

  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    perror("bind failed");
    close(server_socket);
    exit(1);
  }

  if (listen(server_socket, MAX_CLIENTS) == -1) {
    perror("listen failed");
    close(server_socket);
    exit(1);
  }

  printf("Server listening on port %d\n", PORT);

  while (1) {
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket == -1) {
      perror("accept failed");
      continue;
    }

    handle_client(client_socket);
  }

  close(server_socket);
  return 0;
}
