#include <stdio.h>

int main(int argc, char* argv[], char* envp[]) {
	for (int count = 0; count < argc; count++)
		printf("Аргумент командной строки %d =  %s \n", count, argv[count]);
	
	for (int count = 0; envp[count] != NULL; count++)
		printf("Параметр окружения %d =  %s \n", count, envp[count]);
	
	return 0;
}
