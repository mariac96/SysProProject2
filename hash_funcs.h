#ifndef HASH_FUNCS_H_
#define HASH_FUNCS_H_

unsigned long djb2(unsigned char *str);
unsigned long sdbm(unsigned char *str);
unsigned long hash_i(unsigned char *str, unsigned int i);

#endif
