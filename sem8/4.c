#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MUTEX 0 /* number of sem in semset*/

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

long getNumber(char *numString) {

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

int getSemVal(int semId, int semNum) {

	struct semid_ds ds;
	union semun arg;

	arg.buf = &ds;

	if (semctl(semId, 0, IPC_STAT, arg) == -1) {
		perror("semctl");
		exit(EXIT_FAILURE);
	}

	arg.array = calloc(ds.sem_nsems, sizeof(arg.array[0]));

	if (arg.array == NULL)
	{
		perror("calloc");
		exit(EXIT_FAILURE);
	}
	if (semctl(semId, 0, GETALL, arg) == -1)
	{
		perror("se,ctl (GETALL)");
		exit(EXIT_FAILURE);
	}

	//fprintf(stderr, "semNum = %d\n", semNum);

	int retval = arg.array[semNum];
	free(arg.array);

	return retval;
}

int main(int argc, char *argv[])
{
	int fd1[2], fd2[2], isParent;
	size_t size;
	char resstring[14];

	int semId = semget(IPC_PRIVATE, 1, 0666);
	
	int mary;

	if (argc == 2)
		mary = getNumber(argv[1]);
	else
	{
		fprintf(stderr, "program needs 2 arguments\n");
		exit(EXIT_FAILURE);
	}

	struct sembuf sops = {
			.sem_num  = MUTEX,
			.sem_op  = 1,
			.sem_flg = 0 };


	if (semop(semId, &sops, 1) == -1)
	{
		perror("semop + 1");
		exit(EXIT_FAILURE);
	}

	int parentPid = getpid();

	if (pipe(fd1) == -1)
	{
		perror("Can\'t create pipe\n");
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
		//close(fd1[0]); 	// close read

		int retval;

		for (int i = 0; i < mary; ++i)
		{
			while ( (retval = getSemVal(semId, MUTEX)) != 1) /* there isn't writer turn*/
			{
				//printf("Parent: test sleep\n");
				usleep(10);
			}

			if ( (size = write(fd1[1], "Hello, pussy!", 14)) != 14){
				perror("Can\'t write all string\n");
				exit(EXIT_FAILURE);
			}

			sops.sem_op = +1; // 1 + 1 = 2

			if (semop(semId, &sops, 1) == -1)
			{
				perror("semop  +1");
				exit(EXIT_FAILURE);
			}

			usleep(1000);

			if ((size = read(fd1[0], resstring, 14)) == -1)
			{
				perror("read");
				exit(EXIT_FAILURE);
			}
		
			printf("Parent: %s\n", resstring);
		}

		close(fd1[1]);
		close(fd1[0]);
		
		//printf("Parent exit\n");
	} 
	else /* Child */
	{
		//close(fd1[1]); 	// close write

		sops.sem_op = -1;
		/*struct sembuf sops = {
			.sem_num  = MUTEX,
			.sem_op  = -1,
			.sem_flg = 0};
		*/
		int retval;

		for (int i = 0; i < mary; ++i)
		{

			while ( (retval = getSemVal(semId, MUTEX)) != 2) /* there isn't writer turn*/
			{
			//	printf("child test sleep\n");
				usleep(10);
			}

			//printf("before read\n");

			if ((size = read(fd1[0], resstring, 14)) == -1)
			{
				perror("Can\'t read 	string\n");
				exit(EXIT_FAILURE);
			}

			//printf("C:before -1");

			if (semop(semId, &sops, 1) == -1)
			{
				perror("semop - 1");
				exit(EXIT_FAILURE);
			}

			//printf("C:after -1");

			printf("Child: %s\n",resstring);

			usleep(1000);

			//printf("C: before write\n");

			if ( (size = write(fd1[1], "Hello, world!", 14)) != 14)
			{
					perror("Can\'t write all string\n");
					exit(EXIT_FAILURE);
			}

			//printf("C: after write\n");
			
		}
		/* Закрываем входной поток и завершаем работу */
		close(fd1[0]);
		close(fd1[1]);


	}

	return EXIT_SUCCESS;
}


