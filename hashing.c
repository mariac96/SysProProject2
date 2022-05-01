#include <string.h>
#include <stdio.h>
#include "citizens.h"
#include <stdlib.h>
#include "hashing.h"
#include "country.h"
#include "virus.h"


//initialize hash tables
int initialize(int size,int type){
    size = size>>2;
    int i;

  if(type==0){ //citizen hashtable
    hashtable = malloc(sizeof(citizen*)*size);

    if(hashtable==NULL){
        fprintf(stderr, "Error - unable to allocate required memory for the hash table\n");
        return -1;
    }

   for ( i = 0; i < size; i++){
        hashtable[i]=NULL;
    }
  }else if(type==1){ //country hashtable
    countryhash = malloc(sizeof(countrynode*)*size);

    if(countryhash==NULL){
        fprintf(stderr, "Error - unable to allocate required memory for the hash table\n");
        return -1;
    }

    for ( i = 0; i < size; i++){
        countryhash[i]=NULL;
    }
  }else if(type==2){ //virus hashtable
    virushash = malloc(sizeof(virusnode*)*size);

    if(virushash==NULL){
        fprintf(stderr, "Error - unable to allocate required memory for the hash table\n");
        return -1;
    }

    for ( i = 0; i < size; i++){
        virushash[i]=NULL;
    }
  }
  return size;

}

int hashfunction (int key,int size){
    int k;
    k=  key%size;
    return k;
}

int stringhashfunction (char*key,int size){
  unsigned int k=0;
  unsigned char *p;

   for (p = (unsigned char*)key; *p != '\0'; p++)
      k = 17 * k + *p;
   k = k % size;
   return k;

}

//insert record in every structure
citizen* insert(int size,int size2,int size3,char*id,char*first,char*last,int age,char*country,char*virus,char*date,datenode**headDate,int flag)
{
    citizen *temp=NULL;
    countrynode *temp2=NULL;
    virusnode *temp3=NULL;
    skipnode *temp4=NULL,*temp5=NULL;
    int k,l;

  k =stringhashfunction(id,size);
  if((temp=findCitizen(hashtable[k],id))==NULL){ //if citizen with that id not in hashtable

    l = stringhashfunction(country,size2);
    if((temp2=findCountry(countryhash[l],country))==NULL){ //if country not in hashtable
      if((temp2=insertCountry(country,&countryhash[l]))==NULL)
      {
          fprintf(stderr, "Error - unable to insert node\n");
          return NULL;
      }
    }

    if((temp=insertCitizen(first,last,id,age,&temp2,&hashtable[k]))==NULL){
        fprintf(stderr, "Error - unable to insert node\n");
        return NULL;
    }

  k =stringhashfunction(virus,size3);

  if((temp3=findVirus(virushash[k],virus))==NULL){ //if virus not in hashtable
    if((temp3=insertVirus(virus,id,&virushash[k],date,&temp,headDate))==NULL)
    {
        fprintf(stderr, "Error - unable to insert node\n");
        return NULL;
    }
  }else{ //if virus already in insert in bloom and skiplist
    if(date!=NULL){ //if answer YES
      insertinBloom(id,temp3);
      insertSkipList(&temp3->vacc,&temp,date,id,headDate);
    }else{
      insertSkipList(&temp3->notvacc,&temp,date,id,headDate);
    }
  }

  }else{   //if citizen with the same id already exists
    k =stringhashfunction(virus,size3);

    if(sameCitizen(temp,id,first,last,age,country)==0){ //if the record citizen is exactly the same

      if((temp3=findVirus(virushash[k],virus))==NULL){ //if virus not in hashtable

        if((temp3=insertVirus(virus,id,&virushash[k],date,&temp,headDate))==NULL)
        {
            fprintf(stderr, "Error - unable to insert node\n");
            return NULL;
        }
      }else{
        if((temp4=findSkipList(temp3->vacc,id))!=NULL || (temp5=findSkipList(temp3->notvacc,id))!=NULL){ //if the citizen is already in one of the lists

          if(flag==0){ //if reading from the file discard
            return temp;
          }
          if(temp5!=NULL && date!=NULL){  //if in list not vaccinated and now vaccinated change lists
            removeSkipNode(temp3->notvacc,id);
            insertinBloom(id,temp3);
            insertSkipList(&temp3->vacc,&temp,date,id,headDate);

          }else if(temp4!=NULL){  //if already in vaccinated list discard
              printf("ERROR: CITIZEN %s ALREADY VACCINATED ON %s\n",id,temp4->date->date );
          }
        }else{ //if not in the list add it
          if(date!=NULL){ //answer YES
            insertinBloom(id,temp3);
            insertSkipList(&temp3->vacc,&temp,date,id,headDate);
          }else{
            insertSkipList(&temp3->notvacc,&temp,date,id,headDate);
          }
        }
      }
    }else{ //if a citizen with the same id but different info exists discard
      printf("ERROR: CITIZEN WITH THE SAME ID %s",id );
    }
  }

  return temp;

}

//delete hashtable
void deleteHashtable(int size,int type){
  int i ;
  if(type==0){
    for ( i = 0; i < size; i++)
    {
        deleteList(&hashtable[i]);
    }
    free(hashtable);
  }else if(type==1){
    for ( i = 0; i < size; i++)
    {
        deleteCountryList(&countryhash[i]);
    }
    free(countryhash);
  }else if (type==2){
    for ( i = 0; i < size; i++)
    {
       deleteVirusList(&virushash[i]);
    }
    free(virushash);
  }
}

//print every node in the hashtable
void printHashtable(int size,int type){
  int i;
  if(type==0){
    for ( i = 0; i < size; i++){
        printList(&hashtable[i]);
    }
  }else if(type==1){
    for (i = 0; i < size; i++) {
      printListCountry(&countryhash[i]);
    }
  }else if(type==2){
    for (i = 0; i < size; i++) {
      printf("level = %d\n",i );
      printListVirus(&virushash[i]);
    }
  }
}
