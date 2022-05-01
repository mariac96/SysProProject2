#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hashing.h"
#include "pipes.h"
#include "functions.h"
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include "requests.h"

int flagSignal =0;
void SignalHandler(int signum)
{
    signal(SIGINT,SignalHandler);
    signal(SIGQUIT, SignalHandler);
    printf("Inside siganl handler\n" );
    flagSignal = 1;
}

int main(int argc, char *argv[]){
  int i=1,recordsfile=-1,lines=0,flag,readfd,writefd,numCountries=0,j;
  int length,length2,length3,age;
  int retval,bufferSize;
  fd_set fds;
  FILE *fp;
  char *buffer=NULL,*buffer2=NULL,*word,id[15],virus[25],first[15],last[15],country[50],answer[10],date[20],*day,*month,*year,**files,**buffer3;
  size_t size=0;
  datenode*head=NULL; //list fot the dates
  srand(time(0));
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  struct dirent *dir;
  DIR *subDir;
  stats *headStats = NULL;
  signal(SIGINT,SignalHandler);
  signal(SIGQUIT, SignalHandler);

  if(argc != 3){
    fprintf(stderr, "Not enough arguments\n" );
    return -1;
  }

  if((readfd = open(argv[1],O_RDONLY|O_NONBLOCK))<0){
    perror("Error with open");
    exit(2);
  }


  if((writefd = open(argv[2],O_WRONLY))<0){
    perror("Error with open");
    exit(2);
  }


  FD_ZERO(&fds);
  FD_SET(readfd, &fds);

  //reading buffer size
  retval = select(readfd + 1, &fds, NULL, NULL, NULL);

  if(retval == -1){
    perror("Error with select");
  }
  else if(retval >0){ //data is available
    if (FD_ISSET(readfd, &fds)){
      if(read (readfd,&bufferSize,4)<0){  //reading buffer size
        perror("Error with read");
        exit(2);
      }
      if(read (readfd,&bloomsize,4)<0){ //reading bloom size
        perror("Error with read");
        exit(2);
      }
    }
  }


  i=0;
  char buffer5[200];

  files = (char**) malloc(sizeof(char*));

  do{

    readPipe(bufferSize,0,0,readfd,buffer5);

    files[i] = (char*) malloc(sizeof(char)*(strlen(buffer5)+1));
    strcpy(files[i],buffer5);

    i++;
    files = (char**) realloc (files,sizeof(char*)*(i+1));

  }while(strcmp(buffer5,"ENDEND")!=0);
  numCountries = i-1;

  for(i=0;i<numCountries;i++){

    subDir = opendir(files[i]);
    if(subDir==NULL){
      perror( "Error opening directory\n");
      exit(2);
    }
    while((dir = readdir(subDir))!= NULL){

      if(strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0){
          char* buffer4 = malloc(sizeof(char) *(strlen(files[i]) + strlen(dir->d_name)+2));
          strcpy(buffer4,files[i]);
          strcat(buffer4,"/");
          strcat(buffer4,dir->d_name);

          fp = fopen(buffer4,"r");
          if(fp==NULL){
            perror( "Error opening file\n");
            exit(2);
          }

          while(getline(&buffer,&size,fp)!=-1){
            lines++;  //count the lines in the files
          }
          free(buffer4);
          fclose(fp);
      }
    }

    closedir(subDir);

  }

  free(buffer);
  if(lines <5){
    lines = 10;
  }
  //initialize citizen hashtable, country hashtable and virus hashtable
  length = initialize(lines,0);
  length2= initialize(200,1);
  length3= initialize(30,2);


  for(i=0;i<numCountries;i++){

    if(flagSignal==1){
      ExitChild(files,readfd,writefd,length,length2,length3,&head,buffer2,&headStats,numCountries);
    }
    subDir = opendir(files[i]);
    if(subDir==NULL){
      perror( "Error opening directory\n");
      exit(2);
    }
    while((dir = readdir(subDir))!= NULL){

      if(strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0){
          char* buffer4 = malloc(sizeof(char) *(strlen(files[i]) + strlen(dir->d_name)+2));
          strcpy(buffer4,files[i]);
          strcat(buffer4,"/");
          strcat(buffer4,dir->d_name);



          readfile(buffer4,length,length2,length3,&head);
          free(buffer4);

      }
    }
    closedir(subDir);
  }

  sendBloom(bufferSize,length3,writefd);
  char countryTo[50],countryFrom[50],dateV[20];
  while(2){
    if(flagSignal==1){
      ExitChild(files,readfd,writefd,length,length2,length3,&head,buffer2,&headStats,numCountries);
    }

    readPipe(bufferSize,0,0,readfd,buffer5);

    if(strcmp(buffer5,"REQUEST")==0){

      readPipe(bufferSize,0,0,readfd,id);
      readPipe(bufferSize,0,0,readfd,date);
      readPipe(bufferSize,0,0,readfd,countryFrom);
      readPipe(bufferSize,0,0,readfd,countryTo);
      readPipe(bufferSize,0,0,readfd,virus);

      if(vaccineStatusVirus(id,virus,length3,dateV,countryFrom)==0){

          if(compareDates(date,dateV)==0){
            insertStat(countryTo,&headStats,virus,1,date);
          }else{
            insertStat(countryTo,&headStats,virus,0,date);
          }
          writePipe(bufferSize,writefd,"YES",NULL,0,0);
          writePipe(bufferSize,writefd,dateV,NULL,0,0);
      }else{

        writePipe(bufferSize,writefd,"NO",NULL,0,0);
        insertStat(countryTo,&headStats,virus,1,date);
      }
    }else if(strcmp(buffer5,"STATUS")==0){
      readPipe(bufferSize,0,0,readfd,id);

      vaccineStatus(id,length3,writefd,bufferSize);
    }

  }

}
