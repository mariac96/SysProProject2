#ifndef VIRUS_H_
#define VIRUS_H_
#include "skiplist.h"

typedef struct virusnode{
  char*name;
  unsigned int *bloomfilter;
  struct virusnode *next;
  skiplist *vacc,*notvacc;
}virusnode;

virusnode* insertVirus(char*virus,char* id,virusnode**head,char*date,citizen**record,datenode**headDate);
int deleteVirus(virusnode* node);
int deleteVirusList(virusnode**head);
virusnode* findVirus(virusnode* head,char *virus);
int printListVirus(virusnode**head);
int insertinBloom(char* id, virusnode* node);
int checkinBloom(char*id,virusnode*node);
#endif
