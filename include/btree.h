#ifndef BTREE_H
#define BTREE_H
#include "file_utils.h"

typedef struct PAGE_ PAGE;
typedef struct WPAGE_ WPAGE;
typedef struct header_btree_ header_btree;
typedef struct file_object_btree_ file_object_btree;


WPAGE *criarWorkingPage(PAGE *pag);

int getAndIncrementaRRN(file_object_btree * bTree);

PAGE * criarPagina();

void lerPagina(int rrn, PAGE *page, file_object_btree *bTree);

void inicializaHeaderBTree(file_object_btree *bTree);

file_object_btree * criarArquivoBinBtree(char *bin_name, char *mode);

int searchKeyOnPage(PAGE *page, data_index *data);

int full(PAGE *page);

void insereDadoPage(PAGE *page, data_index *data, int newP);

void jumpToRRN(file_object_btree * bTree, int RRN);

void escrevePaginaNaBTree(file_object_btree * bTree, PAGE * pag);

void escreveRegistroBtree(PAGE * pag, int RRN, file_object_btree * bTree);

int insert(int rrnAtual, data_index *data, int * promoRchild,  data_index* promoKey, file_object_btree * bTree);

void insereOrdenadoOnWPage(WPAGE *wPag, data_index * data, int newP);

data_index * getMiddle(WPAGE * wPag, int *promoRChild);

void split(data_index *newKey, int newP, PAGE * pag, data_index *promoKey, int *promoRChild, PAGE * newPag);

void copyFromWPageToPageBeforePromo(WPAGE *wPag, PAGE *pag, int promoKey);

void copyFromWPageToPageFollowingPromo(WPAGE *wPag, PAGE *pag, int promoKey);

void driver(char *btreeFile, int chave);

#endif

