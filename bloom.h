#ifndef BLOOM_H_
#define BLOOM_H_
//for the bloom filter in the parent
typedef struct bloomNode{
  unsigned int *bloom;
  char *virus;
  struct bloomNode *next;
}bloomNode;


bloomNode* insertBloom(char*virus,unsigned int *filter,bloomNode **head,int bloomsize);
int deleteBloom(bloomNode* node);
int deleteBloomList(bloomNode**head);
bloomNode* findBloom(bloomNode* head,char *virus);
int printListBloom(bloomNode**head,int bloomsize);
int checkinBloomFilter(char*id,bloomNode*node,int bloomsize);
int readBloomFilters(int bufferSize,int bloomSize,bloomNode**headBloom,int readfd);
#endif
