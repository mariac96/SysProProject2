#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include "requests.h"
#include "bloom.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

int travelRequest(int* readfds,int*writefds,bloomNode* headBloom,countryMonitor*headCountry,char*id,char*date,char*countryFrom,char*countryTo,char*virus,int bloomSize,int bufferSize){
  bloomNode*temp;
  countryMonitor*temp2;
  temp = findBloom(headBloom,virus);
  char dateV[20],answer[5],date2[20];
  char* day,*month,*year;
  strcpy(date2,date);
  day = strtok(date2,"-");
  if(atoi(day)>30 || atoi(day)<1){
    printf("ERROR WRONG DATE\n");
    return 0;
  }
  month = strtok(NULL,"-");
  if(atoi(month)<1 || atoi(month)>12){
    printf("ERROR WRONG DATE\n");
      return 0;
    }
    year = strtok(NULL,"-");
  if( atoi(year)<1901){
  printf("ERROR WRONG DATE\n");
      return 0;
  }

  if(temp!=NULL){

    if(checkinBloomFilter(id,temp,bloomSize)==0){ //MAYBE
      if((temp2=findCountryM(headCountry,countryFrom))!=NULL){ //if country exists

        writePipe(bufferSize,writefds[temp2->monitor],"REQUEST",NULL,0,0);
        writePipe(bufferSize,writefds[temp2->monitor],id,NULL,0,0);
        writePipe(bufferSize,writefds[temp2->monitor],date,NULL,0,0);
        writePipe(bufferSize,writefds[temp2->monitor],countryFrom,NULL,0,0);
        writePipe(bufferSize,writefds[temp2->monitor],countryTo,NULL,0,0);
        writePipe(bufferSize,writefds[temp2->monitor],virus,NULL,0,0);
        readPipe(bufferSize,0,0,readfds[temp2->monitor],answer);
        if(strcmp(answer,"YES")==0){
          readPipe(bufferSize,0,0,readfds[temp2->monitor],dateV);

          if(compareDates(date,dateV)==0){
            printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");
            return -1;
          }else{ //less than six months from the vaccination
            printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
            return 0;
         }
       }else{ //answer NO from Monitor
          printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n" );
          return -1;
      }
    }
  }
  }
  printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n" );
  return -1;
}


int compareDates(char*date,char*dateV){
  char day[3],month[3],year[5];
  char dayV[3],monthV[3],yearV[5];
  char* word,word2[20];
  strcpy(word2,date);
  word = strtok(word2,"-");
  strcpy(day,word);
  word = strtok(NULL,"-");
  strcpy(month,word);
  word = strtok(NULL,"-");
  strcpy(year,word);

  strcpy(word2,dateV);
  word = strtok(word2,"-");
  strcpy(dayV,word);
  word = strtok(NULL,"-");
  strcpy(monthV,word);
  word = strtok(NULL,"-");
  strcpy(yearV,word);



  if(atoi(yearV) > atoi(year)){ //if the vaccination date is after the travel date
    return 0; //need anothe vaccination
  }else if((atoi(year) == atoi(yearV)) && (atoi(monthV) > atoi(month)))
  {
    return 0;
  }else if((atoi(year) == atoi(yearV)) && (atoi(monthV) == atoi(month)) && (atoi(dayV) >atoi(day)))
  {
    return 0;
  }

  if(atoi(yearV) == atoi(year) ) {
    if( ((atoi(month)-atoi(monthV))<=6 && (atoi(month)-atoi(monthV))>=0)) {
      if((atoi(month)-atoi(monthV) == 6) && (atoi(day)>=atoi(dayV))) {
        return 0;
      }else{
        return -1;
      }
    }
  }else if((atoi(year) -1) == atoi(yearV)) {
    if((atoi(monthV)-atoi(month))>=6){
      if((atoi(monthV)-atoi(month)==6) && (atoi(day) >= atoi(dayV))) {
        return 0;
      }else{
        return -1;
      }
    }
  }
  return 0; //nead another vaccination
}



//insert request stat in list
stats* insertStat(char*country,stats **head,char*virus,int answer,char*date){
    stats *new;

      new = (stats*) malloc (sizeof(stats));

      if(new==NULL){
        perror("Error with malloc");
        exit(2);
      }


      new->country = (char*) malloc((strlen(country)+1)*sizeof(char));
      if(new->country==NULL){
        perror("Error with malloc");
        exit(2);
      }
      strcpy(new->country,country);

      if(answer==0){
        new->status = 0;
      }else{
        new->status = 1;
      }

      new->virus = (char*) malloc((strlen(virus)+1)*sizeof(char));
      if(new->virus==NULL){
        perror("Error with malloc");
        exit(2);
      }
      strcpy(new->virus,virus);

      new->date = (char*) malloc((strlen(date)+1)*sizeof(char));
      if(new->date==NULL){
        perror("Error with malloc");
        exit(2);
      }
      strcpy(new->date,date);

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


//delete stats  node
int deleteStats(stats* node){
  free(node->country);
  free(node->virus);
  free(node->date);
  free(node);
  return 0;
}


//delete stats list
int deleteStatsList(stats**head){
  stats* current = *head;
  stats* next;

   while (current != NULL)
   {
     next = current->next;
     deleteStats(current);
     current = next;
   }
   return 0;
}

//find and return stats node
stats* findStats(stats* head,char *name){
    stats *temp;

    temp = head;
    while(temp!= NULL && strcmp(temp->country,name)!=0){

      temp = temp->next;
    }

    return temp;

}

int printStats(char*virus,char*date1,char*date2,char*country,stats*head){
  stats *temp;
  int total=0,accepted=0,rejected=0;


  if(country!=NULL){
    temp = head;
    while(temp!= NULL){
      if(strcmp(temp->country,country)==0 && strcmp(temp->virus,virus)==0){
        if(checkDates(date1,date2,temp->date)==0){
          total++;
          if(temp->status ==0){
            accepted++;
          }else{
            rejected ++;
          }
        }
      }
      temp = temp->next;
    }
  }else{
    temp = head;
    while(temp!= NULL){
      if(strcmp(temp->virus,virus)==0){
        if(checkDates(date1,date2,temp->date)==0){
          total++;
          if(temp->status ==0){
            accepted++;
          }else{
            rejected ++;
          }
        }
      }
      temp = temp->next;
    }
  }

  printf("TOTAL REQUESTS %d\n",total);
  printf("ACCEPTED %d\n",accepted);
  printf("REJECTED %d\n",rejected);
}

//check if date2 is between date and date3
int checkDates(char *date,char *date3,char *date2){
  char date4[20],*word;
  char day[3],month[3],year[5];
  char day2[3],month2[3],year2[5];
  char day3[3],month3[3],year3[5];

  strcpy(date4,date);
  word = strtok(date4,"-");
  strcpy(day,word);
  if(atoi(day)>30 || atoi(day)<1){
    printf("ERROR WRONG DATE\n");
    return -1;
  }
  word = strtok(NULL,"-");
  strcpy(month,word);
  if(atoi(month)<1 || atoi(month)>12){
    printf("ERROR WRONG DATE\n");
      return -1;
    }
  word = strtok(NULL,"-");
  strcpy(year,word);
  if( atoi(year)<1901){
    printf("ERROR WRONG DATE\n");
      return -1;
  }

  strcpy(date4,date2);
  word = strtok(date4,"-");
  strcpy(day2,word);
  if(atoi(day2)>30 || atoi(day2)<1){
    printf("ERROR WRONG DATE\n");
    return -1;
  }
  word = strtok(NULL,"-");
  strcpy(month2,word);
  if(atoi(month2)<1 || atoi(month2)>12){
    printf("ERROR WRONG DATE\n");
      return -1;
    }
  word = strtok(NULL,"-");
  strcpy(year2,word);
  if( atoi(year2)<1901){
    printf("ERROR WRONG DATE\n");
      return -1;
  }

  strcpy(date4,date3);
  word = strtok(date4,"-");
  strcpy(day3,word);
  if(atoi(day3)>30 || atoi(day3)<1){
    printf("ERROR WRONG DATE\n");
    return -1;
  }
  word = strtok(NULL,"-");
  strcpy(month3,word);
  if(atoi(month3)<1 || atoi(month3)>12){
    printf("ERROR WRONG DATE\n");
      return -1;
    }
  word = strtok(NULL,"-");
  strcpy(year3,word);
  if( atoi(year3)<1901){
    printf("ERROR WRONG DATE\n");
      return -1;
  }

  if(atoi(year)>atoi(year2) ){
    return -1;
  }
  if(atoi(year)==atoi(year2)){
    if(atoi(month)>atoi(month2)){
        return -1;
    }
  }
  if(atoi(month)==atoi(month2)){
    if(atoi(day)>atoi(day2)){
        return -1;
    }
  }

  if(atoi(year2)>atoi(year3) ){
    return -1;
  }
  if(atoi(year2)==atoi(year3)){
    if(atoi(month2)>atoi(month3)){
        return -1;
    }
  }
  if(atoi(month2)==atoi(month3)){
    if(atoi(day2)>atoi(day3)){
        return -1;
    }
  }

  return 0;
}


int  printInFile(stats*headStats,char*directory){
  FILE *fp;
  pid_t id = getpid();
  char name[20],id2[4];
  stats *temp = headStats;
  int total=0,accepted=0,rejected=0,numFiles,i;
  struct dirent **dir;

  sprintf(name, "log_file.%d", getpid());
  if((fp = fopen(name,"w"))==NULL){
    perror("Error opening file");
    return 0;
  }

  numFiles = scandir(directory,&dir,NULL,alphasort);
  if(numFiles==-1){
    perror("Error with scandir");
    exit(2);
  }

  for(i=2;i<numFiles;i++){
    fprintf(fp, "%s\n",dir[i]->d_name);
  }
  while(temp!= NULL ){
    total++;
    if(temp->status ==0){
      accepted++;
    }else{
      rejected++;
    }
    temp = temp->next;
  }
  fprintf(fp, "TOTAL TRAVEL REQUESTS %d\n",total);
  fprintf(fp, "ACCEPTED %d\n",accepted);
  fprintf(fp, "REJECTED %d\n",rejected);
  fclose(fp);
}


int ExitParent(stats **headStats,char*directory,int numMonitors,pid_t *children,int *writefds,int*readfds,char**fromMonitor,char**toMonitor,bloomNode**headBloom,countryMonitor**headCountry){
  int i,status;
  printInFile(*headStats,directory);  //log_file
  for(i=0;i<numMonitors;i++){
    kill(children[i],SIGKILL); //kill children
  }
  i=0;
  while(i<numMonitors){ //wait for every child
    if ( wait(&status) == -1 ) {
        perror("Error with wait\n" );
        exit(2);
    }
    i++;
  }


  for(i=0;i<numMonitors;i++){
    close(writefds[i]);
    close(readfds[i]);
    unlink(fromMonitor[i]);
    unlink(toMonitor[i]);
    free(fromMonitor[i]);
    free(toMonitor[i]);
  }
  free(readfds);
  free(writefds);
  free(toMonitor);
  free(fromMonitor);
  deleteBloomList(headBloom);
  deleteStatsList(headStats);
  deleteCountryMList(headCountry);

  exit(0);
}


int searchVaccination(char*id,int*writefds,int*readfds,int bufferSize,int numMonitors){
  int i,retval,j,maxfd;
  struct timeval tv;
  fd_set fds;
  char virus[25],first[15],last[15],country[50],answer[10],date[20],age[5];
  for(i=0;i<numMonitors;i++){
    writePipe(bufferSize,writefds[i],"STATUS",NULL,0,0);  //send the request to every monitor
    writePipe(bufferSize,writefds[i],id,NULL,0,0);
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

    tv.tv_sec = 2;
    tv.tv_usec = 0;
    retval = select(maxfd + 1, &fds, NULL, NULL, &tv);
    if(retval == -1){
      perror("Error with select");
    }
    else if(retval >0){
      for(i=0; i<numMonitors; i++){
       if(FD_ISSET(readfds[i], &fds)){ //find in which pipe

            readPipe(bufferSize,0,0,readfds[i],first);
          mo[i] = 1;

          if(strcmp(first,"NOT")!=0){
            readPipe(bufferSize,0,0,readfds[i],last);
            readPipe(bufferSize,0,0,readfds[i],country);
            readPipe(bufferSize,0,0,readfds[i],age);
            printf("%s %s %s %s\nAGE %s\n",id,first,last,country,age );
            readPipe(bufferSize,0,0,readfds[i],virus);

            while(strcmp(virus,"ENDEND")!=0){
              readPipe(bufferSize,0,0,readfds[i],answer);
              if(strcmp(answer,"YES")==0){
                readPipe(bufferSize,0,0,readfds[i],date);
                printf("%s VACCINATED ON %s\n",virus,date );
              }else{
                printf("%s NOT YET VACCINATED\n",virus);
              }
              readPipe(bufferSize,0,0,readfds[i],virus);

            }

            break;
          }
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
}
