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

   FILE* myFile = 0;
   if((myFile = fopen("2a.c", "r")) < 0){
      printf("Can\'t open file\n");
      exit(-1);
   }

   if((shmid = shmget(key, 10000*sizeof(char), 0666|IPC_CREAT|IPC_EXCL)) < 0){

      if(errno != EEXIST){
         printf("Can\'t create shared memory\n");
         exit(-1);
      } 
   }
 

   if((array = (char *)shmat(shmid, NULL, 0)) == (char *)(-1)){
      printf("Can't attach shared memory\n");
      exit(-1);
   }

   char symbol;
   for(int i = 0; symbol != EOF; i++){
      symbol = fgetc(myFile);
      array[i] = symbol;
   }

   if(shmdt(array) < 0){
      printf("Can't detach shared memory\n");
      exit(-1);
   }

   if(fclose(myFile) < 0){
      printf("Can't close file\n");
      exit(-1);
   }

   return 0;
}

  
