#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORTNO 12346
#define BUFFER_SIZE 512

int h=0,b=0,p=0;

int checksec(FILE* fptr){
  FILE* key;
  char buf[1024];

  if(h&b&p){
    key=fopen("easy_key", "r");
    fread(buf, 1024, 1, key);
    fprintf(fptr, "%s", buf);
    fclose(key);
    return 0;
  }
  return 1;
}

void hekers(FILE* fptr) {
  volatile int zeroWeekend;
  char buf[32];

  fprintf(fptr, "So you want to be an 31337 Hax0r?\n");
  fgets(buf, 40, fptr);
  switch(strcmp("y3$\n", buf)) {
  case 0:
    fprintf(fptr, "First you must get power\n");
    break;
  default:
    fprintf(fptr, "Well then go away\n");
    break;
  }

  if(zeroWeekend==0xcafebabe) {
    printf("set h\n");
    h=1;
  } else printf("heckers: %4x\n", zeroWeekend);

  return;
}

void batmenss(FILE* fptr) {
  volatile int batsignet;
  char buf[32];

  fprintf(fptr, "So you want to be the batman?\n");
  fgets(buf, 40, fptr);
  switch(strcmp("YESSSSSSS\n", buf)) {
  case 0:
    fprintf(fptr, "First you must get rich\n");
    break;
  default:
    fprintf(fptr, "Well then go away\n");
    break;
  }

  if(batsignet==0x12345678) {
    printf("set b\n");
    b=1;
  } else printf("batmenss: %4x\n", batsignet);

  return;
}

void pokemans(FILE* fptr) {
  volatile int pikachy;
  char buf[32];

  fprintf(fptr, "So you want to be the best there ever was?\n");
  fgets(buf, 40, fptr);
  switch(strcmp("catchemall\n", buf)) {
  case 0:
    fprintf(fptr, "First you must get respect\n\n");
    break;
  default:
    fprintf(fptr, "Well then go away\n");
    break;
  }

  if(pikachy==0xfa75beef) {
    printf("set b\n");
    p=1;
  } else printf("pokemans: %4x\n", pikachy);

  return;
}

void readInput(int sock){
  int msg;
  char choice[4];
  char buffer[BUFFER_SIZE];
  FILE* fptr = fdopen(sock, "r+");
  char* prompt="Do you want to be a?\n"
               "1.) Pokemon Master\n"
               "2.) Elite Hacker\n"
               "3.) The Batman\n";

  while(checksec(fptr)) {
    fprintf(fptr, "%s", prompt);
    fgets(choice, 4, fptr);
    switch(choice[0]) {
    case '1':
      pokemans(fptr);
      break;
    case '2':11
      hekers(fptr);
      break;
    case '3':
      batmenss(fptr);
      break;
    default:
      fprintf(fptr, "\nThat is not one of the choices\n");
      fflush(fptr);
    }
  }

  fprintf(fptr, "%s", buffer);
  fflush(fptr);
  fclose(fptr);
  return;
}

int main(int argc, char *argv[]) {

  char buffer[BUFFER_SIZE];
  int sockfd, newsockfd, portno, pid;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
  serv_addr.sin_port = htons(PORTNO);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {

    perror("ERROR on binding");
    exit(1);
  }
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  while (1) {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
      perror("ERROR on accept");
    pid = fork();
    if (pid < 0)
      perror("ERROR on fork");
    if (pid == 0)  {
      close(sockfd);
      readInput(newsockfd);
      return;
    }
    close(newsockfd);
    waitpid(-1, NULL, WNOHANG);
  } /* end of while */
  close(sockfd);
  return 0; /* we never get here */
}
