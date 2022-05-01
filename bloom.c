#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bloom.h"
#include "hash_funcs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <unistd.h>

//insert bloom in list
bloomNode* insertBloom(char*virus,unsigned int *filter,bloomNode **head,int bloomsize){
    bloomNode *new;
    int i;
    int size = ceil((bloomsize/sizeof(int))); //size of bloom

    new = findBloom(*head,virus);


    if(new == NULL){  //if there is a bloom node for the virus
      new = (bloomNode*) malloc (sizeof(bloomNode));

      if(new==NULL){
        perror("Error with malloc");
        exit(2);
      }


      new->virus = (char*) malloc((strlen(virus)+1)*sizeof(char));
      if(new->virus==NULL){
        perror("Error with malloc");
        exit(2);
      }
      strcpy(new->virus,virus);

      new->bloom = (unsigned int*) malloc(size*sizeof(int));

      if(new->bloom==NULL){
        perror("Error with malloc");
        exit(2);
      }
      for(i=0;i<size;i++){
        new->bloom[i]=0;
      }

      if(*head == NULL){
          new->next = NULL;
          *head = new;
      }
      else{
      new->next = *head;
      *head= new;
    }

    }
    for(i=0;i<size;i++){
      new->bloom[i] = new->bloom[i] | filter[i];
    }


    return *head;
}


//delete bloom node
int deleteBloom(bloomNode* node){
  free(node->virus);

  free(node->bloom);

  free(node);
  return 0;
}


//delete bloom list
int deleteBloomList(bloomNode**head){
  bloomNode* current = *head;
  bloomNode* next;

   while (current != NULL)
   {
     next = current->next;
     deleteBloom(current);
     current = next;
   }
   return 0;
}

//find and return virus
bloomNode* findBloom(bloomNode* head,char *virus){
    bloomNode *temp;

    temp = head;
    while(temp!= NULL && strcmp(temp->virus,virus)!=0){

      temp = temp->next;
    }

    return temp;

}

int printListBloom(bloomNode**head,int bloomsize){
  bloomNode* current = *head;
  int i;
  int size = ceil(bloomsize/sizeof(int));
  while (current != NULL){

    for(i=0;i<size;i++){
      printf("virus = %s bloom[%d]=%d\n",current->virus,i,current->bloom[i] );
    }
    current = current->next;
  }
  return 0;
}


//check if id in bloom
int checkinBloomFilter(char*id,bloomNode*node,int bloomsize){
  int i ;
  int bits = sizeof(int)*8;
  unsigned long h;
  unsigned int b,p;

  if(node->bloom!=NULL){
    for(i=0;i<16;i++){
      b=1;
      h = hash_i(id,i);
      h = h%(bloomsize*8);
      b = b << (h%bits);
      p = h/bits;
      if((node->bloom[p]&b)==0){
        return -2;
      }
    }
    return 0;
  }
  return -2;
}


//read and store the bloom filters send by the monitors
int readBloomFilters(int bufferSize,int bloomSize,bloomNode**headBloom,int readfd){
  int ni=0,j,sizeofMsg,n,k,l,mo,flag;
  struct timeval tv;
  int retval;
  fd_set fds;
  int size,size2 ; //size of bloom

  while(1){
  if(read (readfd,&sizeofMsg,sizeof(int))<0){  //reading message size
      perror("Error with read");
      exit(2);
  }
  char virus[sizeofMsg];
  n = 0;

  while(n < sizeofMsg){  //reading virus name
      char buffer[bufferSize+1];
      FD_ZERO(&fds);
      FD_SET(readfd, &fds);
      retval = select(readfd + 1, &fds, NULL, NULL, NULL);
      if(retval >0 ){ //data available
          if((k=read (readfd,&buffer,bufferSize))<0){
            perror("Error with read");
            exit (2);
          }
          buffer[bufferSize] = '\0';
          if(n==0){
            strcpy(virus,buffer);
          }else{
            strcat(virus,buffer);
          }
          n = n + k ;

        }
  }

    if(strcmp(virus,"ENDEND") == 0){  //all the filters are sent

        return 0;
    }


    size = ceil(bloomSize/sizeof(int));
    unsigned int bloom[size];
    size2 = floor(bufferSize/sizeof(int));

    n = 0;
    while(n < size){  //reading bloom
        unsigned int  buffer[size2];
          FD_ZERO(&fds);
          FD_SET(readfd, &fds);
          retval = select(readfd + 1, &fds, NULL, NULL, NULL);
          if(retval >0 && FD_ISSET(readfd, &fds)){ //data available
              if((k=read (readfd,&buffer,bufferSize))<0){
                  perror("Error with read");
                  exit (2);
              }
              memcpy(&bloom[n],buffer,bufferSize);
              n = n + size2 ;
          }
        }


      insertBloom(virus,bloom,headBloom,bloomSize);
  }
  return -1;
}
