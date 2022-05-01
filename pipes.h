#ifndef PIPES_H_
#define PIPES_H_

typedef struct countryMonitor{
  char* name;
  struct countryMonitor *next;
  int monitor;
}countryMonitor; //to store which country goes to which monitor

countryMonitor* insertCountryM(char*country,countryMonitor **head,int m);
int deleteCountryM(countryMonitor* node);
int deleteCountryMList(countryMonitor**head);
countryMonitor* findCountryM(countryMonitor* head,char *name);
char* readPipe(int bufferSize,int time,int type,int readfd,char*buffer2);
int writePipe(int bufferSize,int writefd,char*msg,unsigned int*data,int type,int bloomSize);
int sendCountries(int bufferSize,int *writefds,int numMonitors,char*directory,countryMonitor**head);
#endif
