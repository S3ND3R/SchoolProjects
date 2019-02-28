//program for converting a doubles array into bin and pushing it to a file

#include <stdio.h>
#include <stdlib.h>

void to_bin(int dub, int *i_arr, int *num_read) {
  int max_size = 1024;
  for(int i = 0; (dub / 2) && i < max_size; i++) {
    i_arr[i]= dub % 2;
    dub = dub / 2;
    *num_read = i + 1;
  }
}

int main(int argc, char *argv[]) {
  int max_elm = 1024;
  int bin_arr[max_elm];
  int d_array[] = {1, 2, 3, 4, 5, 6, 7};
  int num_elem = sizeof(d_array) / sizeof(double);
  FILE *out = fopen(argv[1], "w");
  if(out == NULL) {
    perror("Couldn't open file");
    exit(1);
  }
  for(int i = 0; i < num_elem; i++) {
    int numread;
    to_bin(d_array[i], bin_arr, &numread);
    fprintf(out, "double val from printf: %d\n", d_array[i]);
    for(int j = 0; j < numread; j++) {
      fprintf(out, "bin val from printf: %d\n", bin_arr[j]);
    }
  }
  return 0;
}
