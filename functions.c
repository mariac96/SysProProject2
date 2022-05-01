#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "citizens.h"
#include "hashing.h"
#include <ctype.h>
#include "skiplist.h"
#include "virus.h"
#include "pipes.h"
#include <math.h>
#include "requests.h"
#include <unistd.h>
//reading and inserting the records in the file filename
int readfile(char* filename,int length,int length2,int length3,datenode**head){
  citizen *temp;
  int age,i;
  FILE *fp;
  char *buffer=NULL,*word,*last,*first,*country,*virus,*date,*answer,*id;
  char *day,*month,*year;
  size_t size=0;
  int flag,flag2,flag3;

   fp = fopen(filename,"r");
   if(fp==NULL){
     fprintf(stderr, "Error opening file\n");
     return -1;
   }

    while(getline(&buffer,&size,fp)!=-1){  //reading lines untile the end of file
      flag2=0;
      flag3=0;
      word = strtok(buffer, " ");

      for(i=0;i<strlen(word);i++){
        if(isdigit(word[i])==0){  //id must be integer
            printf("ERROR IN RECORD %s\n",word);
            flag3=1;
        }
      }
      id = (char*)malloc(sizeof(char) * (strlen(word) + 1));
      if(id==NULL){
        fprintf(stderr, "Error allocating memory\n" );
        return -1;
      }
      strcpy(id,word);

      word = strtok(NULL, " ");
      first = (char*)malloc(sizeof(char) * (strlen(word) + 1));
      if(first==NULL){
        fprintf(stderr, "Error allocating memory\n" );
        return -1;
      }
      strcpy(first,word);

      word = strtok(NULL, " ");
      last = (char*)malloc(sizeof(char) * (strlen(word) + 1));
      if(last==NULL){
        fprintf(stderr, "Error allocating memory\n" );
        return -1;
      }
      strcpy(last,word);

      word = strtok(NULL, " ");
      country = (char*)malloc(sizeof(char) * (strlen(word) + 1));
      if(country==NULL){
        fprintf(stderr, "Error allocating memory\n" );
        return -1;
      }
      strcpy(country,word);


    word = strtok(NULL, " ");
    for(i=0;i<strlen(word);i++){
      if(isdigit(word[i])==0){ //age must be integer
          printf("ERROR IN RECORD %s\n",id);
          flag3=1;
      }
    }
    if(flag3==0){
      age = atoi(word);
      if(age>120 || age<0){
        printf("ERROR IN RECORD %s\n",id);
        flag3=1;
      }
    }

    word = strtok(NULL, " ");
    virus = (char*)malloc(sizeof(char) * (strlen(word) + 1));
    if(virus==NULL){
      fprintf(stderr, "Error allocating memory\n" );
      return -1;
    }
    strcpy(virus,word);
    flag=0;
    for (i=0; i <strlen(virus);  i++) { //check that the virus name is correct
      if(isalnum(virus[i])==0){

         if( virus[i]=='-'){

            if (flag ==0){//only one -
              flag =2;
            }else{
             printf("ERROR IN RECORD %s\n",id);
             flag3=1;
           }

         }else{
           printf("ERROR IN RECORD %s\n",id);
           flag3=1;
         }
      }
    }

    word = strtok(NULL, " ");
    answer = (char*)malloc(sizeof(char) * (strlen(word) + 1));
    if(answer==NULL){
      fprintf(stderr, "Error allocating memory\n" );
      return -1;
    }
    strcpy(answer,word);

    word = strtok(NULL, " ");
    if(word ==NULL ){
      answer[strlen(answer)-1]= '\0';  //withouth the /n
    }

    if(strcmp(answer,"YES")==0 ){
      if(word==NULL|| strcmp(word,"\n")==0){ //if there is no date
        printf("ERROR IN RECORD %s\n",id);
        flag3=1;

      }else{
        date = (char*)malloc(sizeof(char) * (strlen(word) + 1));
        if(date==NULL){
          fprintf(stderr, "Error allocating memory\n" );
          return -1;
        }
        flag2=2;
        word[strlen(word)-1]= '\0';
        strcpy(date,word);

        day = strtok(word,"-");
        if(atoi(day)>30 || atoi(day)<1){ //check that date is correct
          printf("ERROR IN RECORD %s\n",id);
          flag3=1;
        }
        month = strtok(NULL,"-");
        if(atoi(month)<1 || atoi(month)>12){
            printf("ERROR IN RECORD %s\n",id);
            flag3=1;
        }
        year = strtok(NULL,"-");
        if(atoi(year)>2021 || atoi(year)<1901){
          printf("ERROR IN RECORD %s\n",id);
          flag3=1;
        }
      }

    }else if(strcmp(answer,"NO")==0){ //if no then record must not have a date
      if(word!=NULL){
          printf("ERROR IN RECORD %s\n",id);
          flag3=1;
      }

    }else{ //if the answer is neither YES nor NO
      printf("ERROR IN RECORD %s\n",id);
      flag3=1;
    }


//     printf("Record %s %s %s %d %s %s %s \n",id,first,last,age,country,virus,answer );

  if(flag3==0){ //if the record is valid
    if(flag2==0){ //if there is no date set it to null
      date = NULL;
    }
    temp=insert(length,length2,length3,id,first,last,age,country,virus,date,head,0);
  }
    free(id);
    free(first);
    free(last);
    free(virus);
    free(country);
   if(flag2>0){
      free(date);
    }
    free(answer);
  }
  free(buffer);
  fclose(fp);
  return 0;
}



//checking bloom filter for virus and id
int vaccineStatusBloom(char *id, char *virus,int size){
  int k;
  virusnode*temp;

  k = stringhashfunction(virus,size);

  temp = findVirus(virushash[k],virus);

  if(temp!=NULL){

    if(checkinBloom(id,temp)==0){
      printf("MAYBE\n" );
      return 0;
    }
  }
  printf("NOT VACCINATED\n" );
  return -1;
}

//free memory and exit
int exitAll(int length,int length2,int length3,datenode **head){
  deleteHashtable(length,0);
  deleteHashtable(length2,1);
  deleteHashtable(length3,2);
  deleteDateList(head);
  exit(0);
}

//check vaccinated skip list for id
int vaccineStatusVirus(char*id,char*virus,int length3,char*dateV,char*countryFrom){
  int k;
  virusnode*temp;
  skipnode *temp2;

  k = stringhashfunction(virus,length3);
  if((temp = findVirus(virushash[k],virus))!=NULL){
    if((temp2=findSkipList(temp->vacc,id))!=NULL){
      if(strcmp(temp2->data->countryname->name,countryFrom)==0){
        strcpy(dateV,temp2->date->date);

        return 0;
      }
    }
  }

  return 1;

}

//check vaccinated skip list and not vaccinated skip list
int  vaccineStatus(char*id,int length3,int writefd,int bufferSize ){
  int i,f=0;
  char age[5];
  virusnode*current;
  skipnode *temp2;

  for(i=0; i<length3;i++){
    current = virushash[i];
    while (current != NULL){
      if((temp2=findSkipList(current->vacc,id))!=NULL){
        if(f==0){
          f=1;
          writePipe(bufferSize,writefd,temp2->data->firstname,NULL,0,0);
          writePipe(bufferSize,writefd,temp2->data->lastname,NULL,0,0);
          writePipe(bufferSize,writefd,temp2->data->countryname->name,NULL,0,0);
          snprintf(age,sizeof(int),"%d",temp2->data->age);
          writePipe(bufferSize,writefd,age,NULL,0,0);
        }
        writePipe(bufferSize,writefd,current->name,NULL,0,0);
        writePipe(bufferSize,writefd,"YES",NULL,0,0);
        writePipe(bufferSize,writefd,temp2->date->date,NULL,0,0);

      }else if((temp2=findSkipList(current->notvacc,id))!=NULL){
        if(f==0){
          f=1;
          writePipe(bufferSize,writefd,temp2->data->firstname,NULL,0,0);
          writePipe(bufferSize,writefd,temp2->data->lastname,NULL,0,0);
          writePipe(bufferSize,writefd,temp2->data->countryname->name,NULL,0,0);
          snprintf(age,sizeof(int),"%d",temp2->data->age);
          writePipe(bufferSize,writefd,age,NULL,0,0);
        }
        writePipe(bufferSize,writefd,current->name,NULL,0,0);
        writePipe(bufferSize,writefd,"NO",NULL,0,0);

      }
      current = current->next;
    }
  }
  if(f==0){

      writePipe(bufferSize,writefd,"NOT",NULL,0,0);
  }else{

      writePipe(bufferSize,writefd,"ENDEND",NULL,0,0);
  }
  return 1;
}

//print citizens in the not vaccinated skip list of virus
int printNotVaccinated(char*virus,int size){
  int k;
  virusnode *temp3=NULL;
  k = stringhashfunction (virus,size);
  temp3 = findVirus(virushash[k],virus);

  if(temp3==NULL){
    printf("ERROR VIRUS %s DOES NOT EXIST\n",virus );
    return 0;
  }
  printSkipList(&temp3->notvacc);
  return 1;
}

//after reading all the files sending the bloom filters to the parent
int sendBloom(int bufferSize,int length3,int writefd){
  int i,j;
  virusnode* current ;
  int size = ceil(bloomsize/sizeof(int));
  unsigned int b[size];
  char buffer[50];
  for(i=0;i<size;i++){
    b[i]=0;
  }

  for(i=0; i<length3; i++){
    current = virushash[i];
    while (current != NULL){

       current = current->next;
    }

  }
  for(i=0; i<length3; i++){

    current = virushash[i];
    while (current != NULL){

      if(current->bloomfilter!=NULL){


      writePipe(bufferSize,writefd,current->name,NULL,0,0);  //write virus name
      writePipe(bufferSize,writefd,NULL,current->bloomfilter,1,bloomsize);  //write bloom filter

    }else{
      writePipe(bufferSize,writefd,current->name,NULL,0,0);  //write virus name
      writePipe(bufferSize,writefd,NULL,b,1,bloomsize);  //write a zero bloom filter
    }
      current = current->next;
    }
  }


  strcpy(buffer,"ENDEND");
  writePipe(bufferSize,writefd,buffer,NULL,0,0);
}




int  FileMonitor(int length2,stats*headStats){
  FILE *fp;
  countrynode* current;
  pid_t id = getpid();
  char name[20],id2[4];
  stats *temp = headStats;
  int total=0,accepted=0,rejected=0,numFiles,i;


  sprintf(name, "log_file.%d", getpid());
  if((fp = fopen(name,"w"))==NULL){
    perror("Error opening file");
    return 0;
  }

  for(i=0;i<length2;i++){
    current = countryhash[i];
    while (current != NULL){
      fprintf(fp, "%s\n",current->name);

      current = current->next;
    }
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


int ExitChild(char**files,int readfd,int writefd,int length,int length2,int length3,datenode **head,char*buffer2,stats**headStats,int numCountries){
  int i;
  FileMonitor(length2,*headStats);
  for(i=0;i<=numCountries;i++){
    free(files[i]);

  }
  free(files);
  close(readfd);
  close(writefd);
  free(buffer2);
  deleteStatsList(headStats);
  exitAll(length,length2,length3,head);
  exit(0);
}
