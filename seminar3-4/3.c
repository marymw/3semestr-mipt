#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	pid_t  pid, ppid, pid1;
	int a = 0;
	pid = fork();

	if (pid == - 1)
		printf("Error\n");
	else if (pid == 0){
		a = a + 1;
		printf ("Я ребёнок!!");
		pid1 = getpid();
		ppid = getppid();
		printf("My PID = %d, my PPID = %d, result = %d\n",  (int)pid1, (int)ppid, a);
	}

	else {

	a = a + 10;
	pid1 = getpid();
	ppid = getppid();
	printf("Я родитель\n");
	printf("My PID = %d, my PPID = %d, result = %d\n pid = %d\n",  (int)pid1, (int)ppid, a, pid1);
	}
	return 0;
}
