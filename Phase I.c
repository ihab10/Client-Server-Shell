#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define WORD 16
#define CHAR 1024

char* input = NULL; //input file
char* output = NULL; //output file
int pipe_nb = 0; // nb of pipes
int iflag = 0; // input flag
int oflag = 0; // output flag
int is_piping = 0; // flag to determine if there is a pipe
int position = 0; // position


void count_pipe(char*str[]) //count the number of piping symbols
{
	int i = 0;
	int output_nb = 0;
	int input_nb = 0;

	while(str[i] != NULL){ 
		if(strcmp(str[i],">") == 0)
			output_nb++;
		if(strcmp(str[i],"<") == 0)
			input_nb++;
		if(strcmp(str[i],"|") == 0)
			pipe_nb++;
		i++;
	}
	
	if(pipe_nb > 3){
		printf("Error: we do not support more than 3 pipes");
	}
	if(!((output_nb==1 && input_nb==0)||(output_nb==0 && input_nb==1)||(output_nb==0 && input_nb==0))){
		printf("Error: you can use either one input or one output\n");
		return;
	}

}

int  process_pipe(char * str[]){ //handle piping and I/O

	int i=0;
	while(str[i] != NULL)
	{

		if(strcmp(str[i],">") == 0){ //output
			oflag = 1; //set ouput flag on
			output = str[i+1];//set the output file to the following value in the array of strings
			return i; //return the index of the ouput
		}


		if(strcmp(str[i],"<") == 0){ //input
                        iflag = 1; //set input flag on
			input = str[i+1];//set the output file to the following value in the array of strings

			return i;//return the index of the input

		}

		if(strcmp(str[i],"|") == 0)///////check for pipe symbol in the line
		{
			is_piping = 1; //set the pipe flag on

			return i;//return the index of the pipe
		}

		i++;
	}

	return i;

}



int get_command(char* cmd[], char line[], char* pipe1_cmd[], char* pipe2_cmd[], char* pipe3_cmd[]){
	//get the command, parse it, split it on pipes, and store the arguments of each command in a different array

	int i = 0;
	char* str[WORD];

	// read input from user
	fgets(line, CHAR, stdin); 

        //remove last \n
	int j=0; 
	while(line[j] != '\n')
	{
		j++;
	}
	line[j]= '\0';

	//exit when the user types "exit"
	if(strcmp(line,"exit") == 0) 
		exit(0);

	//devide the string into array of strings based on the spaces
	int k=0;
	str[k]= strtok(line, " ");
	while(str[k] != NULL){
		k++;
		str[k] = strtok(NULL," ");
	}

	if(str[i] == NULL) //if the user did not enter any command print error
	{
		printf("Error: no command in the input\n");
		return 1;
	}

	count_pipe(str);
	position = process_pipe(str);//position of pipe

	while(i < position){ //store the arguments of the first command before the first pipe into array
		cmd[i] = str[i];
		i++;
	}

	cmd[i] = NULL;//mark the end of array
	i++;

	if(is_piping == 1) //go into this block only if there is pipe(s)
	{
		int j = 0;
		while(str[i] != NULL && strcmp(str[i],"|") != 0)//store the arguments of the second command into array until reaching a second pipe (if available)
		{
			pipe1_cmd[j] = str[i];
			i++;
			j++;
		}
		pipe1_cmd[j] = NULL;
		i++;


	if(pipe_nb >1) //if it contains a second pipe

	{
		int k = 0;
		while(str[i] != NULL  && strcmp(str[i],"|") != 0)
		{
			pipe2_cmd[k] = str[i];
			i++;
			k++;
		}
		pipe2_cmd[k] = NULL;
		i++;

	if (pipe_nb>2) //if it contains a third pipe
	{

		int z=0;
		while(str[i]!=NULL)
		{
			pipe3_cmd[z] = str[i];
			i++;
			z++;
		}
		pipe3_cmd[z] = NULL;
		i++;


	}
}
}
return 1;
}




void exec_pipes(char* cmd[], char * pipe1_cmd[], char* pipe2_cmd[], char* pipe3_cmd[], int pipefd1[],int pipefd2[], int pipefd3[]){
	//execute the pipes based on their number

	int pid;
	int i;
	
	pipe(pipefd1);
	pipe(pipefd2);
	pipe(pipefd3);

if(pipe_nb == 1){ //if there is 1 pipe
	pid = fork();
	if(pid == 0){
		close(1);
		close(pipefd1[0]);
		dup(pipefd1[1]);
		close(pipefd1[0]);
		execvp(cmd[0],cmd);
		perror(cmd[0]);

	}

	else
	{
		wait(NULL);
		close(0);
		close(pipefd1[1]);
		dup(pipefd1[0]);
		execvp(pipe1_cmd[0],pipe1_cmd);
		perror(pipe1_cmd[0]);
	}
}



if(pipe_nb ==2){ //if there is 2 pipes

	pid = fork();

	if(pid == 0){
		int pid2 = fork();
		if(pid2 == 0){
			close(1);
			close(pipefd1[0]);
			dup(pipefd1[1]);
			close(pipefd1[1]);
			execvp(cmd[0],cmd);
			perror(cmd[0]);
		}

		if(pid2 > 0){
			close(0);
			close(pipefd1[1]);
			close(pipefd2[0]);
			close(1);
			dup(pipefd1[0]);
			close(pipefd1[0]);
			dup(pipefd2[1]);
			close(pipefd2[1]);
			execvp(pipe1_cmd[0],pipe1_cmd);
			perror(pipe1_cmd[0]);
		}
	}

	else{
		close(0);
		close(pipefd2[1]);
		dup(pipefd2[0]);
		close(pipefd2[0]);
		execvp(pipe2_cmd[0],pipe2_cmd);
		perror(pipe2_cmd[0]);
	}
}


if(pipe_nb==3){//if there is 3 pipes
	pid = fork();

	if(pid == 0){
		int pid2=fork();

		if(pid2==0){
			int pid3=fork();

			if(pid3==0){
				close(1);
				close(pipefd1[0]);
				dup(pipefd1[1]);
				close(pipefd1[1]);
				execvp(cmd[0],cmd);
				perror(cmd[0]);
			}

			else{
				close(0);
				close(pipefd1[1]);
				close(pipefd2[0]);
				close(1);
				dup(pipefd1[0]);
				close(pipefd1[0]);
				dup(pipefd2[1]);
				close(pipefd2[1]);
				execvp(pipe1_cmd[0],pipe1_cmd);
				perror(pipe1_cmd[0]);
			}
		}


		else{ 
			close(0);
			close(pipefd1[0]);
			close(pipefd1[1]);
			close(pipefd2[1]);
			close(pipefd3[0]);
			close(1);
			dup(pipefd2[0]);
			close(pipefd2[0]);
			dup(pipefd3[1]);
			close(pipefd3[1]);
			execvp(pipe2_cmd[0],pipe2_cmd);
			perror(pipe2_cmd[0]);

		}
	}

	else{ 
		close(pipefd1[0]);
		close(pipefd1[1]);
		close(pipefd2[0]);
		close(pipefd2[1]);
		close(0);
		close(pipefd3[1]);
		dup(pipefd3[0]);
		close(pipefd3[0]);
		execvp(pipe3_cmd[0],pipe3_cmd);
		perror(pipe3_cmd[0]);

	}

}

}

int main()
{
	//the arrays of pipes
	char* cmd[WORD];
	char* pipe1_cmd[WORD];
	char* pipe2_cmd[WORD];
	char* pipe3_cmd[WORD];

	char line[CHAR];//input line

	int pipefd1[2];
	int pipefd2[2];
	int pipefd3[2];

	while(get_command(cmd,line,pipe1_cmd,pipe2_cmd, pipe3_cmd)){
		pid_t child = fork();
		if(child == 0){

			if(oflag == 1 && output != NULL){ //output redirection case
				int file2 = open(output,O_RDWR|O_CREAT,0777);
				close(1);
				dup(file2);
			}

			if(iflag == 1 && input != NULL){ //input redirection case
				int file2 = open(input,O_RDWR|O_CREAT,0777);
				close(0);
				dup(file2);
			}

			if(is_piping == 1){ //pipes case
				exec_pipes(cmd , pipe1_cmd,pipe2_cmd,pipe3_cmd, pipefd1, pipefd2, pipefd3);
			}

			else //natural command
				execvp(cmd[0],cmd);

		}

		if(child > 0){
			wait(NULL);
			iflag = 0;
			input = NULL;
			oflag = 0;
			output = NULL;
			is_piping = 0;
			pipe_nb = 0;

		}
	}

	return 0;
}
