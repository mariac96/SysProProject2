#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#include "skiplist.h"
#include "requests.h"

int readfile(char* filename,int length,int length2,int length3,datenode**head);
int vaccineStatusBloom(char *id, char *virus,int size);
int exitAll(int length,int length2,int length3,datenode **head);
int vaccineStatusVirus(char*id,char*virus,int length3,char*dateV,char*countryFrom);
int  vaccineStatus(char*id,int length3,int writefd,int bufferSize );
int ExitChild(char**files,int readfd,int writefd,int length,int length2,int length3,datenode **head,char*buffer2,stats**headStats,int numCountries);
int  FileMonitor(int length2,stats*headStats);
int printNotVaccinated(char*virus,int size);
int sendBloom(int bufferSize,int length3,int writefd);
#endif
