#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
  FILE* output = fopen("test1", "w+");
  for(int i = 0; i < 10; i++){
    fprintf(output, "%d\n", i);
  }
  //fflush(output);
  char buf[128];
  int fname = open("test1", O_RDONLY);
  int nread = read(fname, buf, 128);
  buf[nread] = '\0';
  printf("%s\n", buf);
  return 0;
}
