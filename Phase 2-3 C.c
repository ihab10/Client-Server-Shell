#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int sockfd, port, n;
  char* address = "127.0.0.1";
  struct sockaddr_in serv_addr;

  char buffer[256];

	/* Check if the IP address is the only provided argument */
  /* Create a socket with the provided settings */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0) {
    perror("Error in socket");
    exit(1);
  }

  port =atoi(argv[1]);
  serv_addr.sin_family = AF_INET; // set the domain 
  serv_addr.sin_port = htons(port); // set the port number

  /* Convert (into a network address) and set the IP */
  if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
    perror("inet_pton error for %s");
  exit(0);
  }

  char bf[10000];

  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
perror("ERROR connecting");
  exit(0);
}
  bzero(buffer,256);

  while(1){
   printf("Please enter the command: \n");
   scanf("%[^\n]",buffer);
   getchar();
   n = write(sockfd,buffer,strlen(buffer));
   n = read(sockfd,bf,10000);
   printf("%s\n",bf);
   if(strcmp(buffer,"exit")==0){
     exit(0);
   }
   if(strcmp(buffer,"serverexit")==0){
    exit(0);
  }
  if (n < 0) {
   perror("ERROR writing to socket");
exit(0);
}
}
return 0;
}
