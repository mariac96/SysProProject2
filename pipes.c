#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include "pipes.h"

//insert country  in list
countryMonitor* insertCountryM(char*country,countryMonitor **head,int m){
    countryMonitor *new;

      new = (countryMonitor*) malloc (sizeof(countryMonitor));

      if(new==NULL){
        perror("Error with malloc");
        exit(2);
      }


      new->name = (char*) malloc((strlen(country)+1)*sizeof(char));
      if(new->name==NULL){
        perror("Error with malloc");
        exit(2);
      }
      strcpy(new->name,country);

      new->monitor = m;


      if(*head == NULL){
          new->next = NULL;
          *head = new;
      }
      else{
      new->next = *head;
      *head= new;
      }



    return *head;
}


//delete country monitor node
int deleteCountryM(countryMonitor* node){
  free(node->name);

  free(node);
  return 0;
}


//delete country monitor list
int deleteCountryMList(countryMonitor**head){
  countryMonitor* current = *head;
  countryMonitor* next;

   while (current != NULL)
   {
     next = current->next;
     deleteCountryM(current);
     current = next;
   }
   return 0;
}

//find and return country monitor
countryMonitor* findCountryM(countryMonitor* head,char *name){
    countryMonitor *temp;

    temp = head;
    while(temp!= NULL && strcmp(temp->name,name)!=0){

      temp = temp->next;
    }

    return temp;

}
//read pipe and store data in buffer2
char* readPipe(int bufferSize,int time,int type,int readfd,char*buffer2){

  struct timeval tv;
  int sizeofMsg,n,retval,k;
  fd_set fds;

  FD_ZERO(&fds);
  FD_SET(readfd, &fds);

  if(time == 0){
    retval = select(readfd + 1, &fds, NULL, NULL, NULL);
  }
  else{
    tv.tv_sec = time;
    tv.tv_usec = 0;
    retval = select(readfd + 1, &fds, NULL, NULL, &tv);
  }


  if(retval == -1){
    perror("Error with select");
  }
  else if(retval >0){ //data is available
    if (FD_ISSET(readfd, &fds)){
      if(read (readfd,&sizeofMsg,sizeof(int))<0){  //reading message size
        perror("Error with read");
        exit(2);
      }

    }
  }



  n = 0;
  while(n < sizeofMsg){  //reading message
    FD_ZERO(&fds);
    FD_SET(readfd, &fds);
    retval = select(readfd + 1, &fds, NULL, NULL, NULL);
    if(retval >0){ //data available
      char buffer[bufferSize+1];
      if((k=read (readfd,&buffer,bufferSize))<0){
        perror("Error with read");
        exit (2);
      }
      buffer[bufferSize] = '\0';
      if(n==0){
        strcpy(buffer2,buffer);
      }else{
        strcat(buffer2,buffer);
      }
      n = n + k ;
    }
  }
}

//write in pipe if type = 0 data is string if 1 bloom
int writePipe(int bufferSize,int writefd,char*msg,unsigned int*data,int type,int bloomSize){

  int sizeofMsg, size = floor(bufferSize/sizeof(int));
  unsigned int buffer3[size];

  int n,k;
  char buffer[bufferSize];
  if(type == 0){  //write string
    sizeofMsg = strlen(msg)+1;

    if(write(writefd,&sizeofMsg,sizeof(int))==-1){ //write size of the message
      perror("Error in writing");
      exit(2);
      }
      n=0;
      while(n < sizeofMsg){
        strncpy(buffer,&msg[n],bufferSize);

        if((k=write(writefd,&buffer,bufferSize))==-1){ //write  message
            perror("Error in writing");
            exit(2);
          }

        n = n + k;
      }
    }else if(type == 1){ // write bloom

        int sizeofMsg = ceil(bloomSize/sizeof(int)), size = floor(bufferSize/sizeof(int));
        unsigned int buffer3[size];
        int i;

        n=0;
        while(n < sizeofMsg){
          memcpy(buffer3,&data[n],bufferSize);


          if((k=write(writefd,&buffer3,bufferSize))==-1){ //write  message
              perror("Error in writing");
              exit(2);
            }

          n = n + size;
        }
    }
}
//send countries to the children
int sendCountries(int bufferSize,int *writefds,int numMonitors,char*directory,countryMonitor**head){
  int i,numFiles,c=0;
  struct dirent **dir;
  char buffer[200];


  numFiles = scandir(directory,&dir,NULL,alphasort);
  if(numFiles==-1){
    perror("Error with scandir");
    exit(2);
  }

  for(i=2;i<numFiles;i++){
    strcpy(buffer,directory);
    strcat(buffer,"/");
    strcat(buffer,dir[i]->d_name);

    insertCountryM(dir[i]->d_name,head,c);
   writePipe(bufferSize,writefds[c],buffer,NULL,0,0);
    c++;
    if(c==numMonitors){  //for the round robin
      c = 0;
    }
  }
  strcpy(buffer,"ENDEND");
  for(i=0;i<numMonitors;i++){
    writePipe(bufferSize,writefds[i],buffer,NULL,0,0); //to know the monitor when all the countries are sent

  }
  for(i=0;i<numFiles;i++){
    free(dir[i]);
  }
  free(dir);
  return 0;
}
