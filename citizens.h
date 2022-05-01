#ifndef CITIZENS_H_
#define CITIZENS_H_
#include "country.h"

typedef struct citizen{
    char *firstname, *lastname,*id;
    int age;
    struct countrynode *countryname; //pointer to node with country info
    struct citizen *next;
}citizen;


citizen* insertCitizen(char*first,char*last,char*id,int age,countrynode**country,citizen**head);
citizen* findCitizen(citizen* head,char* key);
int deleteCitizen(citizen* node);
int deleteList(citizen**head);
int printList(citizen**head);
int sameCitizen(citizen*temp,char*id,char*first,char*last,int age,char*country);
#endif
