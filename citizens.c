#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "citizens.h"
#include <ctype.h>
#include "hashing.h"
#include "country.h"
#include "virus.h"


//inserting citizen to the list
citizen* insertCitizen(char*first,char*last,char*id,int age,countrynode**country,citizen**head){
    citizen * new = (citizen*) malloc (sizeof(citizen));

    if(new==NULL){
      fprintf(stderr, "Error - unable to allocate required memory\n");
      return NULL;
    }

    new->age = age;
    new->id =(char*) malloc((strlen(id)+1)*sizeof(char));
    if(new->id==NULL){
      fprintf(stderr, "Error - unable to allocate required memory\n");
      return NULL;
    }
    strcpy(new->id,id);

    new->firstname = (char*) malloc((strlen(first)+1)*sizeof(char));
    if(new->firstname==NULL){
      fprintf(stderr, "Error - unable to allocate required memory\n");
      return NULL;
    }
    strcpy(new->firstname,first);

    new->lastname = (char*) malloc((strlen(last)+1)*sizeof(char));
    if(new->lastname==NULL){
      fprintf(stderr, "Error - unable to allocate required memory\n");
      return NULL;
    }
    strcpy(new->lastname,last);
    new->countryname = *country;

    if(*head == NULL){ //if the list is empty
        new->next = NULL;
        *head = new;
        return *head;
    }

    new->next = *head;
    *head= new;

    return *head;
}

//delete citizen node
int deleteCitizen(citizen* node){
  free(node->id);
  free(node->firstname);
  free(node->lastname);
  free(node);
  return 0;
}

//delete every node in the list
int deleteList(citizen**head){
  citizen* current = *head;
  citizen* next;

   while (current != NULL)
   {
     next = current->next;
     deleteCitizen(current);
     current = next;
   }
   return 0;
}

//print every node in the list
int printList(citizen**head){
  citizen* current = *head;

  while (current != NULL){
    printf("%s %s %s %s %d\n",current->id,current->lastname,current->firstname,current->countryname->name,current->age );
    current = current->next;
  }
  return 0;
}

//find citizen with id = key and return it
citizen* findCitizen(citizen* head,char* key){
    citizen *temp;
    temp = head;

    if(temp!=NULL){
        while(temp!= NULL && strcmp(key,temp->id)!=0){
             temp = temp->next;
        }
    }

    return temp;
}

//return 0 if the citizens have the same info
int sameCitizen(citizen*temp,char*id,char*first,char*last,int age,char*country){
  if(strcmp(temp->id,id)==0 && strcmp(temp->firstname,first)==0 && strcmp(temp->lastname,last)==0 && age == temp->age && strcmp(temp->countryname->name,country)==0  ){
    return 0;
  }
  return 1;
}
