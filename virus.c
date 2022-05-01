#include "virus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hash_funcs.h"
#include "hashing.h"
#include "skiplist.h"

//insert virus in list
virusnode* insertVirus(char*virus,char* id,virusnode**head,char*date,citizen**record,datenode**headDate){
    virusnode * new = (virusnode*) malloc (sizeof(virusnode));

    if(new==NULL){
      fprintf(stderr, "Error - unable to allocate required memory\n");
      return NULL;
    }


    new->name = (char*) malloc((strlen(virus)+1)*sizeof(char));
    if(new->name==NULL){
      fprintf(stderr, "Error - unable to allocate required memory\n");
      return NULL;
    }
    strcpy(new->name,virus);
    new->bloomfilter = NULL; //initialize bloom filter and lists
    new->vacc=(skiplist*)malloc(sizeof(skiplist));
    new->notvacc=(skiplist*)malloc(sizeof(skiplist));
    initializeList(&new->vacc,NULL,0);
    initializeList(&new->notvacc,NULL,0);

    if(date!=NULL){ //if answer YES
      insertinBloom(id,new);
      insertSkipList(&new->vacc,record,date,id,headDate);
    }else{
      insertSkipList(&new->notvacc,record,date,id,headDate);
    }

    if(*head == NULL){
        new->next = NULL;
        *head = new;
        return *head;
    }

    new->next = *head;
    *head= new;
    return *head;
}


//delete virus node
int deleteVirus(virusnode* node){
  free(node->name);
  if(node->bloomfilter!=NULL){
    free(node->bloomfilter);
  }
  deleteSkipList(&node->vacc);
  deleteSkipList(&node->notvacc);
  free(node->vacc);
  free(node->notvacc);
  free(node);
  return 0;
}


//delete virus list
int deleteVirusList(virusnode**head){
  virusnode* current = *head;
  virusnode* next;

   while (current != NULL)
   {
     next = current->next;
     deleteVirus(current);
     current = next;
   }
   return 0;
}

//find and return virus
virusnode* findVirus(virusnode* head,char *virus){
    virusnode *temp;

    temp = head;
    while(temp!= NULL && strcmp(temp->name,virus)!=0){
      temp = temp->next;
      }

    return temp;

}

int printListVirus(virusnode**head){
  virusnode* current = *head;

  while (current != NULL){
    printf("%s \n",current->name);
    current = current->next;
  }
  return 0;
}
//insert id in bloom
int insertinBloom(char* id, virusnode* node){
  int i;
  int bits = sizeof(int)*8;
  unsigned long h;
  unsigned int b,p;
  int size = ceil((bloomsize/sizeof(int))); //size of bloom

  if(node->bloomfilter ==NULL){ //if empty
    node->bloomfilter = (unsigned int*) malloc(size*sizeof(int));

    if(node->bloomfilter==NULL){
      fprintf(stderr, "Error - unable to allocate required memory\n");
      return -1;
    }

    for(i=0;i<size;i++)
      node->bloomfilter[i]=0;
  }


  for(i=0;i<16;i++){
    b=1;
    h = hash_i(id,i);
    h = h%(bloomsize*8);
    b = b << (h%bits);
    p = h/bits;
    node->bloomfilter[p] = node->bloomfilter[p] | b;
  }
  return 0;
}

//check if id in bloom
int checkinBloom(char*id,virusnode*node){
  int i ;
  int bits = sizeof(int)*8;
  unsigned long h;
  unsigned int b,p;
  printf("((((IN child CHECKING IN VIRUS =%s ID=%s\n",node->name,id );
  if(node->bloomfilter!=NULL){
    for(i=0;i<16;i++){
      b=1;
      h = hash_i(id,i);
      h = h%(bloomsize*8);
      b = b << (h%bits);
      p = h/bits;
      if((node->bloomfilter[p]&b)==0){
        return -2;
      }
    }
    return 0;
  }
  return -2;
}
