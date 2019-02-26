// cmdcol.c tracks active jobs by altering the cmdcol_t data type

#include "commando.h"

// prints out a job list with relevant info
void cmdcol_print(cmdcol_t *col) {
  printf("%-4s %-9s %4s %10s %4s %-s\n", "JOB" , "#PID", "STAT", "STR_STAT",
         "OUTB", "COMMAND");
  for(int i = 0; i < col->size; i++) {
    printf("%-4d ", i);
    printf("#%-8d ",col->cmd[i]->pid);
    printf("%4d ", col->cmd[i]->status);
    printf("%10s ", col->cmd[i]->str_status);
    printf("%4d ", col->cmd[i]->output_size);
    for(int j = 0; col->cmd[i]->argv[j] != NULL; j++) {
      printf("%-s ", col->cmd[i]->argv[j]);
    }
    printf("\n");
  }

}

// adds a new cmd_t to the passed cmdcol_t
void cmdcol_add(cmdcol_t *col, cmd_t *cmd) {
  if(col->size < MAX_CMDS) {
    col->cmd[col->size] = cmd;
    col->size = col->size + 1;
  } else {
    printf("Reached maximum number of commands\n ");
  }
}

// updates all commands in cmdcol->argv
void cmdcol_update_state(cmdcol_t *col, int nohang) {
  for(int i = 0; i < col->size; i++) {
    cmd_update_state(col->cmd[i], nohang);
  }
}

// frees all memory allocated to the commands within cmdcol->argv
void cmdcol_freeall(cmdcol_t *col) {
  for(int i = 0; i < col->size; i++) {
    cmd_free(col->cmd[i]);
  }
}
