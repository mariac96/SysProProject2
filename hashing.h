#ifndef HASHING_H_
#define HASHING_H_
#include "citizens.h"
#include "skiplist.h"
#include "virus.h"
#include "country.h"

unsigned long bloomsize; //size of the bloom filter
citizen**hashtable;
countrynode**countryhash;
virusnode**virushash;

int initialize(int size,int type);
int hashfunction (int key,int size);
int stringhashfunction (char*key,int size);
citizen* insert(int size,int size2,int size3,char*id,char*first,char*last,int age,char*country,char*virus,char*date,datenode**headDate,int flag);
void deleteHashtable(int size,int type);
void printHashtable(int size,int type);
#endif
