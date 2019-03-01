//program for converting a doubles array into bin and pushing it to a file

#include <stdio.h>
#include <stdlib.h>

int int_to_bin(int num, int *b_arr) {
  int max_size = 1024;
  int b_length = 1;
  printf("loop on num: %d\n", num);
  for(int i = 0; i < max_size; i++) {
    b_arr[i]= num % 2;
    num = num / 2;
    printf("num: %d\n", num);
    if(num == 0) {
      return b_length;
    } else {
      b_length ++;
    }
  }
  return b_length;
}

int main(int argc, char *argv[]) {
  int max_elm = 1024;
  int bin_arr[max_elm];
  int i_array[] = {1, 2, 3, 4, 5, 6, 7};
  int num_elem = sizeof(i_array) / sizeof(int);
  FILE *out = fopen(argv[1], "w");
  if(out == NULL) {
    perror("Couldn't open file");
    exit(1);
  }
  for(int i = 0; i < num_elem; i++) {
    int num_read;
    num_read = int_to_bin(i_array[i], bin_arr);
    fprintf(out, "double val from printf: %d\n", i_array[i]);
    fprintf(out, "bin val from printf:");
    for(int j = 0; j < num_read; j++) {
      fprintf(out, "%d ", bin_arr[j]);
    }
    fprintf(out, "\n");
  }
  return 0;
}
