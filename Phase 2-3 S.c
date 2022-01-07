#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
int conn = 0;

int tokenize(char *in, char * res[],char*  a);

void run(char* a,char * reply) {
    char * b[100];
    int i;
    i=tokenize(a,b,"|");
    if(i==1) { 
	int fd2[2];
	pipe(fd2);
        int j=tokenize(a,b," ");
        b[j]=NULL;
        pid_t pid=fork();
        if(pid==0) {
            printf("\n");
	    close(1);
	    dup(fd2[1]);
	    close(fd2[1]);
	    close(fd2[0]);
            execvp(b[0],b);
        }
	else{
	    close(fd2[1]);
	    read(fd2[0],reply,10000);
	    
	    }
    }
    if(i==2) {
	int fd2[2];
	pipe(fd2);
        char* p[100];
        char* l[100];
        int j=tokenize(b[0],p," ");
        int y=tokenize(b[1],l," ");
        p[j]=NULL;
        l[y]=NULL;
        int fd[2];
        pipe(fd);
        pid_t pid1=fork();
        if(pid1==0) {
            close(1);
	    close(fd2[0]);
	    close(fd2[1]);
            dup(fd[1]);
            close(fd[0]);
            close(fd[1]);
            execvp(p[0],p);
        }
        if(pid1>0) {
            pid_t pid2=fork();
            if(pid2==0) {
		
                dup2(fd[0],0);
		dup2(fd2[1],1);
                close(fd[1]);
                close(fd[0]);
	        close(fd2[1]);
	        close(fd2[0]);
                execvp(l[0],l);
            }
            if(pid2>0) {
		
		
                close(fd[0]);
                close(fd[1]);
            }
		close(fd2[1]);
		read(fd2[0],reply,10000);
		
	    }
    }
    if(i==3) {
	int fd2[2];
	pipe(fd2);
        char* q[100];
        char* w[100];
        char* e[100];
        int r=tokenize(b[0],q," ");
        int t=tokenize(b[1],w," ");
        int d=tokenize(b[2],e," ");
        q[r]=NULL;
        w[t]=NULL;
        e[d]=NULL;
        int pipe1[2];
        int pipe2[2];
        pipe(pipe1);
        pipe(pipe2);
        pid_t pid1=fork();
        if(pid1==0) {
	    close(fd2[1]);
	    close(fd2[0]);
            dup2(pipe1[1],1);
            close(pipe1[1]);
            close(pipe1[0]);

            execvp(q[0],q);
        }
        if(pid1>0) {
            pid_t pid2=fork();
            if(pid2==0) {
                close(fd2[1]);
	 	close(fd2[0]);
		dup2(pipe1[0],0);
                close(pipe1[0]);
                close(pipe1[1]);
                dup2(pipe2[1],1);
                close(pipe2[0]);
                close(pipe2[1]);
                execvp(w[0],w);
            }
            if(pid2>0) {
                pid_t pid3=fork();
                if(pid3==0) {
		    
                    close(pipe1[0]);
                    close(pipe1[1]);
                    dup2(pipe2[0],0);
                    close(pipe2[0]);
                    close(pipe2[1]);
		    dup2(fd2[1],1);
		    close(fd2[1]);
	            close(fd2[0]);
		    execvp(e[0],e);
                }
		if(pid3>0){
		 
          	  close(pipe1[0]);
          	  close(pipe1[1]);
           	  close(pipe2[0]);
          	  close(pipe2[1]);

			} 
		  close(fd2[1]);
		  read(fd2[0],reply,10000);
            }
	
	   
        }
    }
	
}
typedef struct thread_data {
    int id;
    int con;
} THREAD_DATA;

void * terminal(void * arg){
	THREAD_DATA* data = (THREAD_DATA*) arg;
	int sock;
	sock=data->con;
	char a[256];
	char reply[10000];
	conn++;
	printf("Connection between client %d and server is established.\n", conn);
	if(strcmp(a,"serverexit")==0){
			printf("Exiting server..\n");
			exit(0);
		}
	while(strcmp(a,"exit")!=0){
    bzero(a,256);
    bzero(reply,10000);
    read(sock,a,256);
    run(a,reply);
    write(sock,reply,10000);
	}
	printf("Connection %d has been lost\n\n", conn);
}
int main(int argc, char *argv[]){
    int sockfd, newsockfd, portno, clilen;
    char a[256];
	char reply[10000];
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
	while(newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)){
	THREAD_DATA data;
	data.con=newsockfd;
	pthread_t tid;
 	pthread_attr_t attr;
  	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  	pthread_create(&tid, &attr, terminal,(void *) &data);
	}
return 0;
}


int tokenize(char *in, char * res[],char*  a){
         int i = 0;
         char *p=strtok(in, a);
        while (p != NULL){
                res[i++] = p;
                p = strtok (NULL, a);
        }
        return i;

}


