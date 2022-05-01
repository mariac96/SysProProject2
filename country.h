#ifndef COUNTRY_H_
#define COUNTRY_H_

typedef struct countrynode{
  char*name;
  struct countrynode *next;
}countrynode;

countrynode* insertCountry(char*country,countrynode**head);
int deleteCountry(countrynode* node);
int deleteCountryList(countrynode**head);
countrynode* findCountry(countrynode* head,char *country);
int printListCountry(countrynode**head);

#endif
