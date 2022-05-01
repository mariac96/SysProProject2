#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "citizens.h"
#include "skiplist.h"

//initialize list
skiplist* initializeList(skiplist **list,skipnode**down,int flag){
  skipnode* new;
  new = (skipnode*) malloc(sizeof(skipnode));
  if(new ==NULL){
    fprintf(stderr, "Error - unable to allocate required memory \n");
    return NULL;
  }

  new->data =NULL;
  new->id= (char*) malloc((strlen("-2")+1)*sizeof(char));
  if( new->id==NULL){
    fprintf(stderr, "Error - unable to allocate required memory \n");
    return NULL;
  }
  strcpy(new->id,"-2");
  new->date=NULL;
  new->next=NULL;

  if(flag==0){ //empty
    new->down=NULL;
    (*list)->head=new;
    (*list)->levels=1;
  }else{//adding level
    new->down=(*list)->head;
    (*list)->head=new;
    (*list)->levels++;
  }

  return *list;
}


int randomLevel(){
  return (rand()%2);
}

//insert record in skiplist
int insertSkipList(skiplist **list, citizen** record,char*date,char*id,datenode **head){
  skipnode* temp,*temp2;
  datenode*newdate;
  skipnode *position[(*list)->levels];
  int i,j,k;
  temp = (*list)->head;


  if(temp!=NULL){ //if the list is initialized

    if(date!=NULL){
      if((newdate=findDate(date,head))==NULL){ //find date in list
        if((newdate=insertDate(date,head))==NULL){
          fprintf(stderr, "Error - unable to insert node\n");
          return -1;
        }
      }
    }

    i=(*list)->levels-1;
    for(k=0;k<(*list)->levels;k++){
      while(temp->next!=NULL && strcmp(temp->next->id,id)<0 ){
        temp = temp->next;
      }

      if(temp->next!=NULL){
        if(strcmp(temp->next->id,id)==0){ //node already in the list
          return 2;
        }
      }
      position[i] = temp; //find the position of the node in level i
      i--;
      if(temp->down!=NULL){
        temp = temp->down;
      }
    }
    addtoList(&position[0],NULL,record,&newdate,id);  //add to lvl 0
    j=1;
    while(j<(*list)->levels && randomLevel()==1){ //if random add to next level
      addtoList(&position[j],&position[j-1],record,&newdate,id);
      j++;
    }
    if(j==(*list)->levels){ //new lvl to the list
      if(randomLevel()==1){

        initializeList(list,&(*list)->head,2);
        temp2=addtoList(&(*list)->head,&position[j-1],record,&newdate,id);
        while(randomLevel()==1){
          initializeList(list,&(*list)->head,2);  //if random add to next level
          temp2=addtoList(&(*list)->head,&temp2,record,&newdate,id);
        }
      }
    }
    return 0;
  }

  return -1;
}

//adding record in level - list of skip list in position
skipnode* addtoList(skipnode**position,skipnode**down,citizen** record,datenode**date,char*id){
  skipnode* new;
  new = (skipnode*) malloc (sizeof(skipnode));
  if(new==NULL){
    fprintf(stderr, "Error - unable to allocate required memory \n");
    return NULL;
  }
  new->data = *record;

  new->id = (char*)malloc((strlen(id)+1)*sizeof(char));
  if(new->id==NULL){
    fprintf(stderr, "Error - unable to allocate required memory \n");
    return NULL;
  }
  strcpy(new->id,id);

  new->date=*date;
  if(down==NULL){
    new->down = NULL;
  }else{
    new->down = *down;
  }
  new->next = (*position)->next;
  (*position)->next = new;
  return (*position)->next;
}

//print every node in list
int printSkipList(skiplist**list){
  skipnode *temp = (*list)->head;
  int k,i;

  for (k=0;k<(*list)->levels;k++){
    if(temp->down!=NULL){
      temp = temp->down;
    }
  }
  temp = temp->next;
  while(temp!=NULL){
    printf("%s %s %s %s %d\n",temp->id,temp->data->firstname, temp->data->lastname,temp->data->countryname->name,temp->data->age);
    temp = temp->next;
  }
  return 0;
}

//free memory of list
int deleteSkipList(skiplist**list){
  skipnode *current,*next,*down;
  current = (*list)->head;
  int i;

  for(i=0; i<(*list)->levels;i++){
    down=current->down;
    while(current !=NULL){
      next=current->next;
      deleteSkipnode(current);
      current=next;
    }
    current=down;
  }
  return 0;
}

//delete node
int deleteSkipnode(skipnode*node){
  free(node->id);
  free(node);
  return 0;
}

//find node in skiplist
skipnode* findSkipList(skiplist*list,char*key){
  skipnode*temp;
  int i,k;
  temp = list->head;
  i=0;
  for(k=0;k<list->levels;k++){
    while(temp->next!=NULL && strcmp(temp->next->id,key)<0 ){
      temp = temp->next;                     //find the position of the node
    }

    if(temp->next!=NULL){
      if(strcmp(temp->next->id,key)==0){
        return temp->next;
      }
    }
    i++;
    if(temp->down!=NULL){
      temp = temp->down;
    }
  }
  return NULL;
}

//remove node form every level in list
int removeSkipNode(skiplist*list,char*key){
  skipnode*temp,*temp2;
  int i,k;
  temp = list->head;
  i=0;
  for(k=0;k<list->levels;k++){
    while(temp->next!=NULL && strcmp(temp->next->id,key)<0 ){
      temp = temp->next;                     //find the position of the node
    }

    if(temp->next!=NULL){
      if(strcmp(temp->next->id,key)==0){
        temp2 = temp->next;
        temp->next = temp->next->next;
        deleteSkipnode(temp2);
      }
    }
    i++;
    if(temp->down!=NULL){
      temp = temp->down;
    }
  }

  return 0;
}

//insert date in list
datenode * insertDate(char*date,datenode **head){
  datenode * new = (datenode*) malloc (sizeof(datenode));

  if(new==NULL){
    fprintf(stderr, "Error - unable to allocate required memory\n");
    return NULL;
  }


  new->date = (char*) malloc((strlen(date)+1)*sizeof(char));
  if(new->date==NULL){
    fprintf(stderr, "Error - unable to allocate required memory\n");
    return NULL;
  }
  strcpy(new->date,date);

  if(*head == NULL){
       new->next = NULL;
      *head = new;
      return *head;
  }

  new->next = *head;
  *head = new;

  return *head;

}

//delete node date
int deleteDate(datenode* node){
  free(node->date);
  free(node);
  return 0;
}

//delete list
int deleteDateList(datenode **head){
  datenode* current = *head;
  datenode* next;

   while (current != NULL)
   {
     next = current->next;
     deleteDate(current);
     current = next;
   }
   return 0;
}

//find date in list 
datenode* findDate(char *date,datenode **head){
    datenode *temp;

    temp = *head;

    while(temp!= NULL && strcmp(temp->date,date)!=0){
      temp = temp->next;
    }


    return temp;

}
