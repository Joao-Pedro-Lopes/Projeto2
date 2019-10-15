/* 
1ª Entrega Projeto Sistemas Operativos 
93588 - Joao Lopes
93598 - Matias Sommer
*/

#ifndef FS_H
#define FS_H
#include "lib/bst.h"
#include <pthread.h>

typedef struct tecnicofs {
    node* bstRoot;
    int nextINumber;
    
    
    pthread_mutex_t mutex;
    pthread_mutex_t mutexVec;
    
    pthread_rwlock_t lock;
    pthread_rwlock_t lockVec;

} tecnicofs;

int obtainNewInumber(tecnicofs* fs);
tecnicofs* new_tecnicofs();
void free_tecnicofs(tecnicofs* fs);
void create(tecnicofs* fs, char *name, int inumber);
void delete(tecnicofs* fs, char *name);
int lookup(tecnicofs* fs, char *name);
void print_tecnicofs_tree(FILE * fp, tecnicofs *fs);

void init();
void lock();
void rdLock();
void lockVec();
void unlock();
void unlockVec();

#endif /* FS_H */
