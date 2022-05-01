#include "country.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//insert country to the list
countrynode* insertCountry(char*country,countrynode**head){
    countrynode * new = (countrynode*) malloc (sizeof(countrynode));

    if(new==NULL){
      fprintf(stderr, "Error - unable to allocate required memory\n");
      return NULL;
    }


    new->name = (char*) malloc((strlen(country)+1)*sizeof(char));
    if(new->name==NULL){
      fprintf(stderr, "Error - unable to allocate required memory\n");
      return NULL;
    }
    strcpy(new->name,country);


    if(*head == NULL){ //if the list is empty
      new->next = NULL;
        *head = new;
        return *head;
    }

    new->next = *head;
    *head= new;

    return *head;
}

//delete node
int deleteCountry(countrynode* node){
  free(node->name);
  free(node);
  return 0;
}

//delete every node in list
int deleteCountryList(countrynode**head){
  countrynode* current = *head;
  countrynode* next;

   while (current != NULL)
   {
     next = current->next;
     deleteCountry(current);
     current = next;
   }
   return 0;
}

//find and return country with name = country
countrynode* findCountry(countrynode* head,char *country){
    countrynode *temp;
    temp = head;

    while(temp!= NULL && strcmp(temp->name,country)!=0){
             temp = temp->next;
     }
     return temp;

}

//print every node in the list
int printListCountry(countrynode**head){
  countrynode* current = *head;


  while (current != NULL){
    printf("%s \n",current->name);
    current = current->next;
  }
  return 0;
}
