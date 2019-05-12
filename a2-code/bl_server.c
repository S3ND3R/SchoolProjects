// bl_server.c
// authors: Warren Weber
//          Alin Butoi

#include "blather.h"

int signalled = 0;

server_t server;
server_t *server_p  = &server;

//defined below main
void handle_shutdown(int signum);

int main(int argc, char *argv[]) {
  if(argc < 2) {
    printf("useage: %s <servername>\n", argv[0]);
    return 0;
  }

  char *server_name = argv[1];

  // server_t server;
  // server_t *server_p  = &server;

  server_start(server_p, server_name, DEFAULT_PERMS);

  //NOTE: testing print
  printf("NAME:%s\nfd:%d\nREADY:%d\nNUMCLIENT:%d\n", server.server_name,
         server.join_fd, server.join_ready, server.n_clients);

  // Setting up the signal handler
  struct sigaction my_sa = {};
  my_sa.sa_handler = handle_shutdown;
  sigemptyset(&my_sa.sa_mask);
  my_sa.sa_flags = SA_RESTART;
  sigaction(SIGTERM, &my_sa, NULL);
  sigaction(SIGINT, &my_sa, NULL);

  while(!signalled) {
    server_check_sources(server_p);
    if(server_join_ready(server_p)) {
      server_handle_join(server_p);
    }
    for(int i = 0; i < server_p->n_clients; i++) {
      if(server_client_ready(server_p,i)) {
        server_handle_client(server_p,i);
      }
    }
  } // End main server loop
  server_shutdown(server_p);
  return 0;
} // End main

void handle_shutdown(int signum) {
  signalled = 1;
} // End handle_shutdown
