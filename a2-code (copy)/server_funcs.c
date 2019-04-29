// server.c
#include "blather.h"

//************************************************************************
// server_get_client
// Gets a pointer to the client_t struct at the given index. If the
// index is beyond n_clients, the behavior of the function is
// unspecified and may cause a program crash.
//
// NOTE: check bounds on index
//************************************************************************
client_t *server_get_client(server_t *server, int idx) {
  check_fail(indx > server->n_clients, 1, "index greater than client range");
  client_t *selected_client = &server->client[indx];
  return selected_client;
} // End server_get_client

//************************************************************************
// server_start
// Initializes and starts the server with the given name. A join fifo
// called "server_name.fifo" should be created. Removes any existing
// file of that name prior to creation. Opens the FIFO and stores its
// file descriptor in join_fd._
//************************************************************************
void server_start(server_t *server, char *server_name, int perms) {
  remove(server_name);

  int fifo_chk = mkfifo(server_name, perms) //perms is iffy
  check_fail(fifo_chk != 0, 1, "failed to create fifo");

  int join_server_fd = open(server_name, O_RDWR);
  check_fail(join_server_fd == -1, 1, "failed to open fifo");

  //initialize server_t
  //NOTE: need to possibly initialize client array
  //      may need to check bounds for server_name
  strncpy(server->server_name, server_name, sizeof(server_name));
  server->join_fd = join_server_fd;
  server->join_ready = 0;
  server->n_clients = 0;
} // End server_start

//************************************************************************
// server_shutdown
// Shut down the server. Close the join FIFO and unlink (remove) it so
// that no further clients can join. Send a BL_SHUTDOWN message to all
// clients and proceed to remove all clients in any order.
//************************************************************************
void server_shutdown(server_t *server) {

} // End server_shutdown

//************************************************************************
// server_add_client
// Adds a client to the server according to the parameter join which
// should have fields such as name filled in.  The client data is
// copied into the client[] array and file descriptors are opened for
// its to-server and to-client FIFOs. Initializes the data_ready field
// for the client to 0. Returns 0 on success and non-zero if the
// server has no space for clients (n_clients == MAXCLIENTS).
//************************************************************************
int server_add_client(server_t *server, join_t *join) {
  if(server->n_clients < MAXCLIENTS) {
    //opening the client FIFOs
    int to_client_fd = open(join->to_client_fname, O_WRONLY);
    check_fail(to_client_fd == -1, 1, "failed to open to_client fifo");

    int to_server_fd = open(join->to_server_fname, O_RDONLY);
    check_fail(to_server_fd == -1, 1, "failed to open to_server fifo");

    // client initialization
    client_t *new_client = server->client[server->n_clients];
    // NOTE: May need to bounds check names
    strncpy(new_client->name, join->name, sizeof(join->name));
    new_client->to_client_fd = to_client_fd;
    new_client->to_server_fd = to_server_fd;
    strncpy(new_client->to_client_fname, join->to_client_fname,
            sizeof(join->to_client_fname));
    strncpy(new_client->to_server_fname, join->to_server_fname,
            sizeof(join->to_server_fname));

    new_client->data_ready = 0
    server->n_clients += 1;
    return 0;
  }
// NOTE: should add message that server is full
return -1;
} //End server_add_client

//************************************************************************
// server_remove_client
// Remove the given client likely due to its having departed or
// disconnected. Close fifos associated with the client and remove
// them.  Shift the remaining clients to lower indices of the client[]
// preserving their order in the array; decreases n_clients.
//************************************************************************
int server_remove_client(server_t *server, int idx) {

} // End server_remove_client

//************************************************************************
// server_broadcast
// Send the given message to all clients connected to the server by
// writing it to the file descriptors associated with them.
//************************************************************************
int server_broadcast(server_t *server, mesg_t *mesg) {

} // End server_broadcast

//************************************************************************
// server_check_sources
// Checks all sources of data for the server to determine if any are
// ready for reading. Sets the servers join_ready flag and the
// data_ready flags of each of client if data is ready for them.
// Makes use of the select() system call to efficiently determine
// which sources are ready.
//************************************************************************
void server_check_sources(server_t *server) {
  //NOTE: May need to add error cheecking on system calls
  //setup set of file_descriptors
  fd_set read_set;
  FD_ZERO(&read_set);
  FD_SET(server->join_fd, &read_set);
  for(int i = 0; i < server->n_clients; i++) {
    FD_SET(server->client[i].to_server_fd, &read_set);
  }
  int maxfd = server->join_fd;
  int curr_client_fd = -1;
  for(int j = 0; j < server->n_clients; j++) {
    curr_client_fd = server->client[j].to_server_fd;
    maxfd = (maxfd < curr_client_fd) ? curr_client_fd : maxfd;
  }
  // setting all ready fd
  select(maxfd + 1, &read_set, NULL, NULL, NULL);

  // checking that fd are ready then setting ready if true
  if(FD_ISSET(server->join_fd, &read_set)) {
    server->join_ready = 1;
  }

  for(int k = 0; k < server->n_clients; k++) {
    if(FD_ISSET(server->client[k].to_server_fd, &read_set)) {
      server->client[k].to_server_fd = 1;
    }
  }
} // End server_check_sources

//************************************************************************
// server_join_ready
// Return the join_ready flag from the server which indicates whether
// a call to server_handle_join() is safe.
//************************************************************************
int server_join_ready(server_t *server) {
  return server->join_ready;
} // End server_join_ready

//************************************************************************
// server_handle_join
// Call this function only if server_join_ready() returns true. Read a
// join request and add the new client to the server. After finishing,
// set the servers join_ready flag to 0.
//************************************************************************
int server_handle_join(server_t *server) {
  join_t join_req;
  mesg_t join_message;
  int nread = read(server->join_fd, &join_req, sizeof(join_t));
  check_fail(nread != sizeof(join_t), 1, "Join request failed");

  join_message.kind = BL_JOINED;
  strncpy(join_message.name, join_req.name, sizeof(join_req.name));
  server_broadcast(server, &join_message);
  server_add_client(server, join_req);
  server->join_ready = 0;
  return 0;
} // End server_handle_join

//************************************************************************
// server_client_ready
// Return the data_ready field of the given client which indicates
// whether the client has data ready to be read from it.
//************************************************************************
int server_client_ready(server_t *server, int idx) {
  check_fail(indx > server->n_clients, 1, "index greater than client range");
  return server->client[idx].data_ready;
} // End server_client_ready

//************************************************************************
// server_handle_client
// Process a message from the specified client. This function should
// only be called if server_client_ready() returns true. Read a
// message from to_server_fd and analyze the message kind. Departure
// and Message types should be broadcast to all other clients.  Ping
// responses should only change the last_contact_time below. Behavior
// for other message types is not specified. Clear the client's
// data_ready flag so it has value 0.
//************************************************************************
int server_handle_client(server_t *server, int idx) {

} // End server_handle_client

/* ADVANCED FEATURES
//************************************************************************
// server_tick
//************************************************************************
void server_tick(server_t *server) {

} // End server_tick

//************************************************************************
// server_ping_clients
//************************************************************************
void server_ping_clients(server_t *server) {

} // End server_ping_clients


//************************************************************************
// server_remove_disconnected
//************************************************************************
void server_remove_disconnected(server_t *server, int disconnect_secs) {

} // End server_remove_disconnected

//************************************************************************
// server_write_who
//************************************************************************
void server_write_who(server_t *server) {

} // End server_write_who

//************************************************************************
// server_log_message
//************************************************************************
void server_log_message(server_t *server, mesg_t *mesg) {

} // End server_log_message

*/
