#include <stdio.h>


void MVS_logToTextFile(const char *strToLog){
  FILE *test = fopen("log.txt","a");
  fprintf(test,"%s\r\n",strToLog);
  fclose(test);
}