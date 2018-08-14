#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(void) 
{
	pid_t child;
	int fd[2];
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
	else if (child == 0)
	{
		// replace standard output with output part of pipe
		dup2(fd[1], 1);
		// close unused unput end of pipe
		close(fd[0]);

		printf("hola juancito como estas");
		
		exit(1);
	}
	else
	{
		// replace standard input with input part of pipe
		dup2(fd[0], 0);
		// close unused end of pipe
		close(fd[1]);
		
		char s[30];

		fgets(s, 1024, stdin);
		if ((strlen(s) > 0) && (s[strlen (s) - 1] == '\n'))
			s[strlen (s) - 1] = '\0';

		printf("%s\n", s);

		exit(1);
	}
	return 0;
}


