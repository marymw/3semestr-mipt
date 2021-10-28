#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main() {
   char    *array;
   int     shmid;
   char    pathname[] = "2a.c";
   key_t   key;

   if((key = ftok(pathname, 0)) < 0){
     printf("Can\'t generate key\n");
     exit(-1);
   }

   if((shmid = shmget(key, 10000*sizeof(char), 0)) < 0){
      printf("Can\'t find shared memory\n");
      exit(-1);
   }

   if((array = (char *)shmat(shmid, NULL, 0)) == (char *)(-1)){
      printf("Can't attach shared memory\n");
      exit(-1);
   }

   char symbol;
   for(int i = 0; ; i++){
      symbol = array[i];
      if(symbol == EOF)
         break;
      if(symbol == '\0')
         printf("\n");
      else
         printf("%c", symbol);
   }

   if(shmdt(array) < 0){
      printf("Can't detach shared memory\n");
      exit(-1);
   }

   if(shmctl(shmid, IPC_RMID, 0) < 0){
      printf("Can't delete shared memory\n");
      exit(-1);
   }

   return 0;
}
