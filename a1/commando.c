// main function for commando that consists of the primary loop of the
// commando shell

#include "commando.h"

// Helper function that returns 0 if echoing is not set or 1 otherwise
int get_echo(int arg_count, char *argv[]) {
  int bool_echo = 0;
  if(arg_count >= 2) {
    if(strncmp(argv[1], "--echo", strlen(argv[1])) == 0) {
      bool_echo = 1;
    }
  } else if(getenv("COMMANDO_ECHO")) {
    bool_echo = 1;
  }
  return bool_echo;
}

int main(int argc, char *argv[], char *envp[]){

  // output directly to the screen while testing
  setvbuf(stdout, NULL, _IONBF, 0); // Turn off output buffering

  // create command collection
  cmdcol_t *cmd_list = malloc(sizeof(cmdcol_t));

  if(cmd_list == NULL) {
    printf("Failed to allocate memory\n");
    exit(1);
  }
  cmd_list->size = 0;

  // infinite loop that runs the main loop
  // that serves as the visible shell for commando
  while(1) {
    printf("@> ");
    char lineBuffer[MAX_LINE];
    char *line_in = fgets(lineBuffer, MAX_LINE, stdin);
    if(line_in == NULL) {
      printf("\nEnd of input\n");
      free(cmd_list);
      exit(1);
    }
    line_in[strlen(line_in) - 1] = '\0';  //replacing \n with null character
    char *cmd_tokens[ARG_MAX];
    int num_tokens;
    parse_into_tokens(line_in, cmd_tokens, &num_tokens);

    //print if echo is set
    if(get_echo(argc, argv)) {
      for(int i = 0; i < num_tokens; i++) {
        printf("%s", cmd_tokens[i]);
        printf(" ");
      }
      printf("\n");
    }

    if(num_tokens > 0 && strncmp(cmd_tokens[0], "help",
                                 strlen(cmd_tokens[0])) == 0) {
      char *help = "COMMANDO COMMANDS\n"
      "help               : show this message\n"
      "exit               : exit the program\n"
      "list               : list all jobs that have been started giving "
      "information on each\n"
      "pause nanos secs   : pause for the given number of nanseconds "
      "and seconds\n"
      "output-for int     : print the output for given job number\n"
      "output-all         : print output for all jobs\n"
      "wait-for int       : wait until the given job number finishes\n"
      "wait-all           : wait for all jobs to finish\n"
      "command arg1 ...   : non-built-in is run as a job\n";
      printf("%s",help);
    } else if(num_tokens > 0 && strncmp(cmd_tokens[0], "list",
                                        strlen(cmd_tokens[0])) == 0) {
      cmdcol_print(cmd_list);
    } else if(num_tokens > 0 && strncmp(cmd_tokens[0], "output-for",
                                        strlen(cmd_tokens[0])) == 0) {
      if(cmd_tokens[1] == NULL) {
        printf("the number of arguments is incorrect\n");
      } else {
        int job_num = atoi(cmd_tokens[1]);
        if(job_num < cmd_list->size && job_num >= 0) {
          printf("@<<< Output for %s[#%d] (%d bytes):\n",
                 cmd_list->cmd[job_num]->name,
                 cmd_list->cmd[job_num]->pid,
                 cmd_list->cmd[job_num]->output_size);
          printf("----------------------------------------\n");
          cmd_print_output(cmd_list->cmd[job_num]);
          printf("----------------------------------------\n");
        } else {
          printf("Command selected is not in the command list\n");
        }
      }
    } else if(num_tokens > 0 && strncmp(cmd_tokens[0], "output-all",
                                        strlen(cmd_tokens[0])) == 0) {
      for(int i = 0; i < cmd_list->size; i++) {
        printf("@<<< Output for %s[#%d] (%d bytes):\n",
               cmd_list->cmd[i]->name,
               cmd_list->cmd[i]->pid,
               cmd_list->cmd[i]->output_size);
        printf("----------------------------------------\n");
        cmd_print_output(cmd_list->cmd[i]);
        printf("----------------------------------------\n");
      }
    } else if(num_tokens > 0 && strncmp(cmd_tokens[0], "wait-for",
                                        strlen(cmd_tokens[0])) == 0) {
      if(cmd_tokens[1] == NULL) {
        printf("the number of arguments is incorrect\n");
      } else {
        int job_num = atoi(cmd_tokens[1]);
        if(job_num < cmd_list->size && job_num >= 0) {
          cmd_update_state(cmd_list->cmd[job_num], DOBLOCK);
        } else {
          printf("Command selected is not in the command list\n");
        }
      }
    } else if(num_tokens > 0 && strncmp(cmd_tokens[0], "wait-all",
                                        strlen(cmd_tokens[0])) == 0) {
      for(int i = 0; i < cmd_list->size; i++) {
        cmd_update_state(cmd_list->cmd[i], DOBLOCK);
      }
    } else if(num_tokens > 0 && strncmp(cmd_tokens[0], "pause",
                                        strlen(cmd_tokens[0])) == 0) {
      if(cmd_tokens[1] == NULL || cmd_tokens[2] == NULL) {
        printf("the number of arguments is incorrect\n");
      } else {
        int nanos = atoi(cmd_tokens[1]);
        int secs = atoi(cmd_tokens[2]);
        pause_for(nanos, secs);
      }
    } else if(num_tokens > 0 && strncmp(cmd_tokens[0], "exit",
                                        strlen(cmd_tokens[0])) == 0) {
      break;
    } else if(num_tokens > 0) {
      cmd_t *cmd = cmd_new(cmd_tokens);
      cmd_start(cmd);
      cmdcol_add(cmd_list, cmd);
    }
    cmdcol_update_state(cmd_list, NOBLOCK);
  }
  cmdcol_freeall(cmd_list);
  free(cmd_list);
  return 0;
}
