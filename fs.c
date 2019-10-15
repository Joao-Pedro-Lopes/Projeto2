/* 
1ª Entrega Projeto Sistemas Operativos 
93588 - Joao Lopes
93598 - Matias Sommer
*/

#include "fs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int obtainNewInumber(tecnicofs* fs) {
	int newInumber = ++(fs->nextINumber);
	return newInumber;
}

tecnicofs* new_tecnicofs(){
	tecnicofs* fs = malloc(sizeof(tecnicofs));
	if (!fs) {
		perror("failed to allocate tecnicofs");
		exit(EXIT_FAILURE);
	}
	fs->bstRoot = NULL;
	fs->nextINumber = 0;
	return fs;
}

void free_tecnicofs(tecnicofs* fs){
	free_tree(fs->bstRoot);
	free(fs);
}

void create(tecnicofs* fs, char *name, int inumber){
	lock();															//Funcao auxiliar para o fecho de trinco dependente da flag introduzida
	fs->bstRoot = insert(fs->bstRoot, name, inumber);
	unlock();														//Funcao auxiliar para a abertura do trinco
}

void delete(tecnicofs* fs, char *name){
	lock();															//E repetido o procedimento para cada funcao que altera ou le qualquer coisa nos ficheiros para evitar erros de outputr
	fs->bstRoot = remove_item(fs->bstRoot, name);
	unlock();
}

int lookup(tecnicofs* fs, char *name){
	rdLock();
	node* searchNode = search(fs->bstRoot, name);
	if ( searchNode ) {
		unlock();
		return searchNode->inumber;
	}
	unlock();
	return 0;
}

void print_tecnicofs_tree(FILE * f_out, tecnicofs *fs){
	print_tree(f_out, fs->bstRoot);
}
