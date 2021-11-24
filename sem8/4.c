#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

long getNumber(char *numString) 
{

	if (*numString == '\0') {
		fprintf(stderr, "empty number argument\n");
		exit(EXIT_FAILURE);
	}

	long gNumber;
	char* endOfEnter;

	const int baseOfNumber = 10;
	gNumber = strtol(numString, &endOfEnter, baseOfNumber);

	if(*endOfEnter != '\0') {
		fprintf(stderr, "strtol error\n");
		exit(EXIT_FAILURE);
	}

	return gNumber;

}

int main(int argc, char *argv[])
{
	int fd[2], isParent;
	size_t size;
	char resstring[20];

	int semId = semget(IPC_PRIVATE, 1, 0666);
	
	int mary;

	if (argc == 2)
		mary = getNumber(argv[1]);
	else
	{
		fprintf(stderr, "program needs 2 arguments\n");
		exit(EXIT_FAILURE);
	}

	struct sembuf sops = { 0, +2, 0 };

	if (semop(semId, &sops, 1) == -1)
	{
		perror("semop + 1");
		exit(EXIT_FAILURE);
	}

	int parentPid = getpid();

	if (pipe(fd) == -1)
	{
		perror("Can\'t create pipe\n");
		exit(EXIT_FAILURE);
	}

	if ( (size = write(fd[1], "Hi from parent!", strlen("Hi from parent!"))) != strlen("Hi from parent!"))
	{
		perror("Can't write string\n");
		exit(EXIT_FAILURE);
	}

	isParent = fork();

	if (isParent == -1) /*parent */
	{
		perror("fork");
		exit(EXIT_FAILURE);
	} 
	else if (isParent) 
	{							

		int sendMsgLen  = strlen("Hi from parent!");
		int recMsgLen   = strlen("Mary, Mary, Mary");

		for (int i = 0; i < mary; ++i)
		{
			sops.sem_op = 0; // we will wait till value of sem decrease to 0

			if (semop(semId, &sops, 1) == -1)
			{
				perror("semop + 1");
				exit(EXIT_FAILURE);
			}

			//printf("P: after 0\n");

			if ( (size = write(fd[1], "Hi from parent!\0", sendMsgLen)) != sendMsgLen)
			{
				perror("Can\'t write all string\n");
				exit(EXIT_FAILURE);
			}

			if ((size = read(fd[0], resstring, recMsgLen)) == -1)
			{
				perror("read");
				exit(EXIT_FAILURE);
			}
		
			sops.sem_op = 2;

			printf("Parent: %.*s\n", recMsgLen, resstring);

			//printf("P: +2\n");

			if (semop(semId, &sops, 1) == -1)
			{
				perror("semop + 1");
				exit(EXIT_FAILURE);
			}
		}

		close(fd[1]);
		close(fd[0]);
	} 
	else /* Child */
	{
		sops.sem_op = -1;

		int retval;

		int recMsgLen  = strlen("Hi from parent!");
		int sendMsgLen = strlen("Mary, Mary, Mary");

		//printf( "HI...   = %d\n"
		//	    "Mary... = %d\n", recMsgLen, sendMsgLen);

		for (int i = 0; i < mary; ++i)
		{
			if (semop(semId, &sops, 1) == -1)
			{
				perror("semop + 1");
				exit(EXIT_FAILURE);
			}

			//printf("C: after -1\n");

			if ((size = read(fd[0], resstring, recMsgLen)) == -1)
			{
				perror("Can\'t read 	string\n");
				exit(EXIT_FAILURE);
			}

			printf("Child: %.*s\n",recMsgLen, resstring);

			if ( (size = write(fd[1], "Mary, Mary, Mary\0", sendMsgLen)) != sendMsgLen)
			{
					perror("Can\'t write all string\n");
					exit(EXIT_FAILURE);
			}

			//printf("Child: -1\n");
			
			if (semop(semId, &sops, 1) == -1)
			{
				perror("semop + 2");
				exit(EXIT_FAILURE);
			}
			
		}

		/* Закрываем входной поток и завершаем работу */
		close(fd[0]);
		close(fd[1]);
	}

	return EXIT_SUCCESS;
}


