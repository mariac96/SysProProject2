#ifndef REQUESTS_H_
#define REQUESTS_H_

#include "bloom.h"
#include "pipes.h"

typedef struct stats{
  char* country; //countryTo
  int status; //if 0 accepted if 1 rejected
  char*virus;//vaccine
  char*date;
  struct stats *next;
}stats;

int ExitParent(stats **headStats,char*directory,int numMonitors,pid_t *children,int *writefds,int*readfds,char**fromMonitor,char**toMonitor,bloomNode**headBloom,countryMonitor**headCountry);
int compareDates(char*date,char*dateV);
int  printInFile(stats*headStats,char*directory);
stats* insertStat(char*country,stats **head,char*virus,int answer,char*date);
int printStats(char*virus,char*date1,char*date2,char*country,stats*head);
int checkDates(char *date,char *date3,char *date2);
int deleteStats(stats* node);
int deleteStatsList(stats**head);
stats* findStats(stats* head,char *name);
int searchVaccination(char*id,int*writeds,int*readfds,int bufferSize,int numMonitors);
int travelRequest(int* readfds,int*writefds,bloomNode* headBloom,countryMonitor*headCountry,char*id,char*date,char*countryFrom,char*countryTo,char*virus,int bloomSize,int bufferSize);
#endif
