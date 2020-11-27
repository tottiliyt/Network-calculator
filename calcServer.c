/*
 * calculator server implementation
 * Xi He, Yuntao Li
 * xhe35@jh.edu yli346@jh.edu
 */
#include <stdio.h>      /* for snprintf */
#include "csapp.h"
#include "calc.h"

#define LINEBUF_SIZE 1024

//print out error message
void errorHandler(char* message){
  fprintf(stderr, "%s\n", message);
  exit(1);
}

//communicate with each accepted client
int chat_with_client(struct Calc *calc, int infd, int outfd) {
  rio_t in;
  char linebuf[LINEBUF_SIZE];

  /* wrap standard input (which is file descriptor 0) */
  rio_readinitb(&in, infd);

  /*
  * Read lines of input, evaluate them as calculator expressions,
  * and (if evaluation was successful) print the result of each
  * expression.  Quit when "quit" command is received.
  */
  int done = 0;
  while (!done) {
    ssize_t n = rio_readlineb(&in, linebuf, LINEBUF_SIZE);
    if (n <= 0) {
      /* error or end of input */
      done = 1;
    }
    else if (strcmp(linebuf, "quit\n") == 0 || strcmp(linebuf, "quit\r\n") == 0) {
      /* quit command */
      done = 1;
    }
    else if (strcmp(linebuf, "shutdown\n") == 0 || strcmp(linebuf, "shutdown\r\n") == 0) {
      /*shutdown command */
      return 0;
    }
    else {
      /* process input line */
      int result;
      if (calc_eval(calc, linebuf, &result) == 0) {
	/* expression couldn't be evaluated */
	rio_writen(outfd, "Error\n", 6);
      } else {
	/* output result */
	int len = snprintf(linebuf, LINEBUF_SIZE, "%d\n", result);
	if (len < LINEBUF_SIZE) {
	  rio_writen(outfd, linebuf, len);
	}
      }
    }
  }
  return 1;
}


int main(int argc, char **argv) {
  if(argc != 2){
    errorHandler("Error: Invalid argument number");
  }

  int listenfd = Open_listenfd(argv[1]);
  if (listenfd < 0) {
    errorHandler("Error: false to open server");
  }

  struct Calc *networkCalc = calc_create();
  
  while (1) {
    int clientfd = Accept(listenfd, NULL, NULL);
    if (clientfd < 0){
      errorHandler("Error: accepting client connection");
    }
    int result = chat_with_client(networkCalc,clientfd, clientfd);
    //if shutdown recieved
    if(result == 0){
      break;
    }
    close(clientfd);
  }

  calc_destroy(networkCalc);
  close(listenfd);

  return 0;
}

