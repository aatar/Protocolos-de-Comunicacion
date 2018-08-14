#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

char commandToExecute[100];
char commandArgument[50];
char inputString[50];
char outputString[50];
pid_t child;
int status;
int fd[2];
char parityInput = 0;
char parityOutput = 0;
int readingInput = 0;
int readingOutput = 0;
int i;

int main(const int argc, const char **argv) {
	
	if(argc < 2) {
		fprintf(stderr, "Usage: %s command_to_execute\n", argv[0]);
		return 1;
	}

	strcpy( commandArgument, argv[1] );
	
	scanf("%s", inputString);
	sprintf(commandToExecute, "echo %s|%s", inputString, commandArgument);
	
	if(pipe(fd) < 0)
	{
		fprintf(stderr, "Pipe Failed\n");
		perror("Pipe Failed\n");
		exit(1);
	}

	child = fork();
	if(child < 0)
	{
		fprintf(stderr, "Fork Failed\n");
		perror("Fork Failed\n");
		exit(1);
	}
	else if (child == 0) //child
	{
		// replace standard output with output part of pipe
		dup2(fd[1], 1);
		// close unused unput end of pipe
		close(fd[0]);

		system(commandToExecute);
		
		exit(1);
	}
	else //parent
	{
		waitpid(child, &status, 0);
		// replace standard input with input part of pipe
		dup2(fd[0], 0);
		// close unused end of pipe
		close(fd[1]);
		
		fgets(outputString, 1024, stdin);
		if ((strlen(outputString) > 0) && (outputString[strlen (outputString) - 1] == '\n'))
			outputString[strlen (outputString) - 1] = '\0';

		fprintf(stdout, "%s\n", outputString);
		
		//Not efficient
		/*for(i = 0; inputString[i] != 0; i++) {
			if(i == 0) parityInput = inputString[0];
			else {
				parityInput ^= inputString[i];
			}
		}
		for(i = 0; outputString[i] != 0; i++) {
			if(i == 0) parityOutput = outputString[0];
			else {
				parityOutput ^= outputString[i];
			}
		}*/

		for(i = 0, readingInput = 1, readingOutput = 1; readingInput || readingOutput; i++) {
			
			if(readingInput) {
				if(inputString[i] == 0) readingInput = 0;
				else if(i == 0) parityInput = inputString[0];
				else {
					parityInput ^= inputString[i];
				}
			}

			if(readingOutput) {
				if(outputString[i] == 0) readingOutput = 0;
				else if(i == 0) parityOutput = outputString[0];
				else {
					parityOutput ^= outputString[i];
				}
			}
		}


		fprintf(stderr, "in parity: 0x%02X\n", parityInput);
		fprintf(stderr, "out parity: 0x%02X\n", parityOutput);

		exit(1);
	}
	
	return 0;
}