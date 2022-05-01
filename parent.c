#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <unistd.h>
#include <sys/wait.h>
#include "pipes.h"
#include "bloom.h"
#include "requests.h"
#include <signal.h>

int flagSignal =0;
void SignalHandler(int signum)
{
    signal(SIGINT,SignalHandler);
    signal(SIGQUIT, SignalHandler);

    printf("Inside signal handler\n");
    flagSignal = 1;
}

int main(int argc, char* argv[]){
  int i=0, numMonitors, bufferSize, bloomSize, inputDir;
  int *writefds,*readfds,status,maxfd=0,j,t;
  pid_t childid;
  fd_set fds;
  char **fromMonitor,**toMonitor,*day,*month,*year;
  char buffer2[50];
  struct timeval tv;
  int retval;
  int sizeofMsg,n;
  unsigned int **bloomFilters;
  bloomNode *headBloom = NULL;
  countryMonitor *headCountry = NULL;
  stats *headStats = NULL;

  signal(SIGINT,SignalHandler);
  signal(SIGQUIT, SignalHandler);

  if(argc!=9){
    fprintf(stderr, "Not enough arguments\n" );
    exit(2);
  }

  while (i<argc )
  {
      if(strcmp(argv[i],"-s")==0){
          bloomSize = atoi(argv[i+1]);
      }
      else if (strcmp(argv[i],"-i")==0){
          inputDir=i+1;
      }
      else if (strcmp(argv[i],"-b")==0){
        bufferSize = atoi(argv[i+1]);
      }
      else if (strcmp(argv[i],"-m")==0){
        numMonitors = atoi(argv[i+1]);
      }
      i++;
   }


  pid_t children[numMonitors];
  char buffer[bufferSize];
  fromMonitor = (char**)malloc(numMonitors * sizeof(char*)); //pipes to write the monitor and the parent reads
  if (fromMonitor == NULL) {
      perror("Memory not allocated.\n");
      exit(2);
  }
  toMonitor = (char**)malloc(numMonitors * sizeof(char*)); //pipes to write the parent and the monitor reads
  if (toMonitor == NULL) {
      perror("Memory not allocated.\n");
      exit(2);
  }

  writefds = (int*)malloc(numMonitors * sizeof(int)); //fds to write the parent
  if (writefds == NULL) {
      perror("Memory not allocated.\n");
      exit(2);
  }
  readfds = (int*)malloc(numMonitors * sizeof(char*)); //fds to read the parent
  if (readfds == NULL) {
      perror("Memory not allocated.\n");
      exit(2);
  }

  for(i=0;i<numMonitors;i++){
    fromMonitor[i] = (char*)malloc(50 * sizeof(char));
		sprintf(fromMonitor[i], "from%d", i);

		if(mkfifo(fromMonitor[i],0666) == -1){
      perror("Error with mkfifo");
      exit(2);
    }

    toMonitor[i] = (char*)malloc(50 * sizeof(char));
		sprintf(toMonitor[i], "to%d", i);

    if(mkfifo(toMonitor[i],0666) == -1){
      perror("Error with mkfifo");
      exit(2);
    }
  }

  for(i=0;i<numMonitors;i++){

    if((readfds[i] = open(fromMonitor[i],O_RDONLY|O_NONBLOCK))<0){
      perror("Error with open");
      exit(2);
    }
      if(maxfd < readfds[i]){
        maxfd = readfds[i];
      }

    childid=fork();
    if(childid == 0){ //in child
        break;
    }
    else if( childid ==-1){
      perror("Error with fork");
      exit(2);
    }
    else{ //in parent
      children[i] = childid;
      if((writefds[i] = open(toMonitor[i],O_WRONLY))<0){
        perror("Error with open");
        exit(2);
      }

    }


  }


  if(childid == 0){ //in child

    execl("./Monitor", "./Monitor",toMonitor[i],fromMonitor[i],(char*)0);
  }

  //in parent

  for(i=0;i<numMonitors;i++){  //send buffer size & bloom size in every monitor
    if(write(writefds[i],&bufferSize,sizeof(bufferSize) )==-1){
      perror("Error in writing");
      exit(2);
    }
    if(write(writefds[i],&bloomSize,sizeof(bloomSize) )==-1){
      perror("Error in writing");
      exit(2);
    }
  }
  sendCountries(bufferSize,writefds,numMonitors,argv[inputDir],&headCountry);

  if(flagSignal==1){
    ExitParent(&headStats,argv[inputDir],numMonitors,children,writefds,readfds,fromMonitor,toMonitor,&headBloom,&headCountry);
  }
  int mo[numMonitors];
  for(i=0;i<numMonitors;i++){
    mo[i] = 0;
  }
  int flag = 0;
  while(flag==0){
    FD_ZERO(&fds);

    maxfd = -1;
    for(j=0; j<numMonitors; j++){

      if(mo[j]==0){  //if bloom of child i not read

        if(maxfd<readfds[j]){
          maxfd = readfds[j];
        }
        FD_SET(readfds[j], &fds);
      }
    }

    retval = select(maxfd + 1, &fds, NULL, NULL, NULL);
    if(retval == -1){
      perror("Error with select");
    }
    else if(retval >0){
      for(i=0; i<numMonitors; i++){
       if(FD_ISSET(readfds[i], &fds)){ //find in which pipe

          int k =  readBloomFilters(bufferSize,bloomSize,&headBloom,readfds[i]);
          mo[i] = 1;


            break;
        }
      }
    }
    flag = 1;
    for(j=0; j<numMonitors; j++){
      if(mo[j]==0){  //if bloom of child i not read

        flag =0;
      }
    }
  }
  if(flagSignal==1){
    ExitParent(&headStats,argv[inputDir],numMonitors,children,writefds,readfds,fromMonitor,toMonitor,&headBloom,&headCountry);
  }

  //waiting for commands
  char *buffer1=NULL,*word;
  char id[15],date[20],countryFrom[50],countryTo[50],virus[25],date2[20];
  size_t size3=0;

    while(1){

      printf("waiting for requests\n" );

      getline(&buffer1,&size3,stdin);
      word = strtok(buffer1, " ");

      if(strcmp(word,"/travelRequest")==0){

        if((word = strtok(NULL, " "))!=NULL){
          strcpy(id,word);
          if((word = strtok(NULL, " "))!=NULL){
            strcpy(date,word);
            if((word = strtok(NULL, " "))!=NULL){
              strcpy(countryFrom,word);
              if((word = strtok(NULL, " "))!=NULL){
                strcpy(countryTo,word);
                if((word = strtok(NULL, " "))!=NULL){
                  strcpy(virus,word);
                  virus[strlen(virus)-1]='\0';

                  if(travelRequest(readfds,writefds,headBloom,headCountry,id,date,countryFrom,countryTo,virus,bloomSize,bufferSize)==-1){
                    insertStat(countryTo,&headStats,virus,1,date);

                  }else{
                      insertStat(countryTo,&headStats,virus,0,date);

                  }
                }
              }
            }
          }
        }
      }else if(strcmp(word,"/exit\n")==0){
        free(buffer1);
        ExitParent(&headStats,argv[inputDir],numMonitors,children,writefds,readfds,fromMonitor,toMonitor,&headBloom,&headCountry);
      }else if(strcmp(word,"/travelStats")==0){
        if((word = strtok(NULL, " "))!=NULL){
          strcpy(virus,word);
          if((word = strtok(NULL, " "))!=NULL){
            strcpy(date,word);
            if((word = strtok(NULL, " "))!=NULL){
              strcpy(date2,word);
              if((word = strtok(NULL, " "))!=NULL){
                strcpy(countryTo,word);
                countryTo[strlen(countryTo)-1]='\0';
                printStats(virus,date,date2,countryTo,headStats);

              }else{
                date2[strlen(date2)-1]='\0';
                printStats(virus,date,date2,NULL,headStats);

              }
            }
          }
        }
      }else if(strcmp(word,"/searchVaccinationStatus")==0){
        if((word = strtok(NULL, " "))!=NULL){
          strcpy(id,word);
          id[strlen(id)-1]='\0';

          searchVaccination(id,writefds,readfds,bufferSize,numMonitors);
        }
      }

      if(flagSignal==1){
        free(buffer1);
        ExitParent(&headStats,argv[inputDir],numMonitors,children,writefds,readfds,fromMonitor,toMonitor,&headBloom,&headCountry);
    }

  }
}
