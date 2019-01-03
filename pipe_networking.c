#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client, int up) {
  remove("wkp");
  char input[BUFFER_SIZE];
  char* msg = malloc(HANDSHAKE_BUFFER_SIZE);
  read(up, msg, HANDSHAKE_BUFFER_SIZE);
  printf("message recieved by server\n");

  printf("server opening private downstream pipe...\n");
  int down = open(msg, O_WRONLY);
  write(down, msg, HANDSHAKE_BUFFER_SIZE);
  printf("message sent by server\n");

  read(up, msg, HANDSHAKE_BUFFER_SIZE);
  if (strncmp(ACK, msg, HANDSHAKE_BUFFER_SIZE) == 0) {
    printf("server connected\n");
  }

  free(msg);

  *to_client = down;
  return up;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  char* private = malloc(HANDSHAKE_BUFFER_SIZE);
  sprintf(private, "/priv/%d", getpid());
  printf("client creating private pipe...\n");
  mkfifo(private, 0644);

  printf("client opening wkp...\n");
  int up = open("wkp", O_WRONLY);
  if (up == -1) {
    printf("failed to connect to server\n");
    return 1;
  }
  write(up, private, HANDSHAKE_BUFFER_SIZE);
  printf("message sent by client\n");

  printf("client opening private pipe...\n");
  int down = open(private, O_RDONLY);
  char* msg = malloc(HANDSHAKE_BUFFER_SIZE);
  read(down, msg, HANDSHAKE_BUFFER_SIZE);
  if (strncmp(private, msg, HANDSHAKE_BUFFER_SIZE) == 0) {
    printf("client recieved message\nclient connected\n");
  }

  remove(private);

  printf("client sending connection confirmation...\n");
  write(up, ACK, HANDSHAKE_BUFFER_SIZE);

  free(msg);

  *to_server = up;
  return down;
}
