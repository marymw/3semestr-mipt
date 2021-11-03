#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main () {
	int PID = getpid();
	int PPID = getppid();
	printf ("PID = %d, PPID = %d\n", PID, PPID);
	return 0;
}
