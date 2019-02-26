// cmd.c file provides functions to manipulate the cmd_t struct data type

#include "commando.h"

// allocates a new cmd_t for the passed array argv[]
// 1. copies string values of arguments in argv[] into cmd->argv[]
// 2. ensures cmd->argv[] ends with a NULL char
// 3. cmd-> name[] set to argv[0]
// 4. cmd->finished to 0
// 5. cmd->str_status[] to "INIT"
// 6. initializes remaining fields to default val
cmd_t *cmd_new(char *argv[]) {
  cmd_t *new = malloc(sizeof(cmd_t));
  if(new == NULL) {
    printf("Failed to allocate memory\n");
    exit(1);
  }
  int pos = 0;
  for(; argv[pos] != NULL; pos++) {
    new->argv[pos] = strdup(argv[pos]);
  }
  new->argv[pos] = NULL;
  strncpy(new->name, new->argv[0], NAME_MAX);
  new->finished = 0;
  snprintf(new->str_status, STATUS_LEN,"INIT");
  new->pid = -1;
  new->status = -1;
  new->output = NULL;
  new->output_size = -1;
  new->out_pipe[0] = -1;
  new->out_pipe[1] = -1;
  return new;
}

// Deallocates the cmd_t structure
// to be deallocated: strings in argv[], output buffer, cmd itself
void cmd_free(cmd_t *cmd) {
  for(int i = 0; cmd->argv[i] != NULL; i++) {
    free(cmd->argv[i]);
  }
  if (cmd->output != NULL) {
    free(cmd->output);
  }
  free(cmd);
}

// not sure yet
void cmd_set_stdin(cmd_t *cmd, char *input_file) {}

// forks the process and creates pipe then lets child execute the cmds and
// the parent records the running child.
// output is stored in the pipe.
void cmd_start(cmd_t *cmd) {
  if(pipe(cmd->out_pipe) < 0) {
    printf("failed to open the pipe\n");
    exit(1);
  }
  snprintf(cmd->str_status, STATUS_LEN,"RUN");
  cmd->pid = fork();
  if(cmd->pid < 0) {
    printf("failed to find a suitable fork\n");
    exit(1);
  } else if(cmd->pid == 0) {  // you are inside the child
    if(dup2(cmd->out_pipe[PWRITE], STDOUT_FILENO) < 0) {
      printf("failed to duplicate\n");
      exit(1);
    }
    execvp(cmd->name, cmd->argv);
    close(cmd->out_pipe[PREAD]);
  } else {  // you are inside the parent
    close(cmd->out_pipe[PWRITE]);
  }
}

// retrieves output from cmd->out_pipe and fills cmd->output
// sets cmd->output_size to number of bytes in output
// closes pipe associated with output
// prints error if cmd->finished = 0
void cmd_fetch_output(cmd_t *cmd) {
  if(cmd->finished == 1) {
    int nread;
    cmd->output = read_all(cmd->out_pipe[PREAD], &nread);
    cmd->output_size = nread;
    close(cmd->out_pipe[PREAD]);
  } else {
    printf("%s[#%d] not finished yet\n", cmd->name, cmd->pid);
  }
}

// prints output of cmd->output if not NULL, otherwise prints error message
void cmd_print_output(cmd_t *cmd) {
  if(cmd->output == NULL) {
    printf("%s[#%d] :  output not ready\n", cmd->name, cmd->pid);
  } else {
    int num_write = write(STDOUT_FILENO, cmd->output, cmd->output_size);
    if(num_write == -1) {
      perror("write failed\n");
      exit(1);
    } else if(num_write < cmd->output_size) {
      printf("wrote less bytes than expected\n");
      exit(1);
    }
  }
}

// updates the state of cmd
void cmd_update_state(cmd_t *cmd, int nohang) {
  if(cmd->finished < 1) {
    int status;
    pid_t pid_status = waitpid(cmd->pid, &status, nohang);
    if(pid_status < 0) {
      perror("error in the child\n");
      exit(1);
    }
    if(pid_status == cmd->pid && WIFEXITED(status)) {
      cmd->finished = 1;
      cmd->status = WEXITSTATUS(status);
      snprintf(cmd->str_status, STATUS_LEN,"EXIT(%d)", cmd->status);
      cmd_fetch_output(cmd);
      printf("@!!! %s[#%d]: EXIT(%d)\n",cmd->name, cmd->pid, cmd->status);
    }
  }
}

// helper function for reading all input from the open file descriptor
char *read_all(int fd, int *nread) {
  int pos = 0;
  int max_size = BUFSIZE;
  char *input = malloc(max_size*sizeof(char));
  if(input == NULL) {
    printf("Failed to allocate memory\n");
    exit(1);
  }
  while(1) {
    if(pos >= max_size) {
      max_size = max_size * 2;
      input = realloc(input, max_size);
      if(input == NULL) {
        printf("Failed to reallocate memory\n");
        exit(1);
      }
    }
    int max_read = max_size - pos;
    *nread = read(fd, (input + pos), max_read);
    // check if everything has been read from input
    if(*nread == 0) {
      break;
    } else if(*nread == -1) {
      perror("read failed");
      exit(1);
    }
    pos += *nread;
  }
  if(pos >= max_size) {
    max_size = max_size + 1;
    input = realloc(input, max_size);
    *nread = pos + 1;
    if(input == NULL) {
      printf("Failed to reallocate memory\n");
      exit(1);
    }
  } else {
    *nread = pos;
  }
  input[*nread] = '\0';
return input;
}
