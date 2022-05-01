#ifndef SKIPLIST_H_
#define SKIPLIST_H_
#include "citizens.h"


typedef struct datenode{
  char*date;
  struct datenode *next;
}datenode;

typedef struct skipnode{
  citizen* data;
  char* id;
  datenode* date;
  struct skipnode *next,*down; //down points to the node in the down level 
}skipnode;

typedef struct skiplist{
  int levels;
  skipnode *head;
}skiplist;


skiplist* initializeList(skiplist **list,skipnode**down,int flag);
int randomLevel();
int insertSkipList(skiplist **list, citizen** record,char*date,char*id,datenode **head);
skipnode* addtoList(skipnode**position,skipnode**down,citizen** record,datenode**date,char*id);
int printSkipList(skiplist**list);
int deleteSkipList(skiplist**list);
int deleteSkipnode(skipnode*node);
skipnode* findSkipList(skiplist*list,char*key);
datenode * insertDate(char*date,datenode **head);
int deleteDate(datenode* node);
int deleteDateList(datenode **head);
datenode* findDate(char *date,datenode **head);
int removeSkipNode(skiplist*list,char*key);

#endif
