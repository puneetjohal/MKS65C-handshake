#include "pipe_networking.h"
#include <signal.h>


int main() {

  int to_client;
  int from_client;

  from_client = server_handshake( &to_client );

  static void sighandler(int signo) {
    if (signo == SIGINT) {
      printf("server removing wkp...\n");
      remove("wkp");
      exit(0);
    }
  }

  while(1) {
    printf("server creating wkp...\n");
    mkfifo("wkp", 0644);
    printf("server opening wkp...\n");
    from_client = open("wkp", O_RDONLY);
    int id = fork();

    if(!id) {
      from_client = server_handshake(from_client, &to_client);
      char input[BUFFER_SIZE];
      while(read(from_client, input, BUFFER_SIZE)) {
        printf("S%d recieved: %s\n", getpid(), input);
        for(int i = 0; i < strlen(input); i++) {
          input[i] += 1;
        }
        write(to_client, input, BUFFER_SIZE);
      }
      exit(0);
    }

    else {
      remove("wkp");
      close(from_client);
      close(to_client);
    }

  }
}
