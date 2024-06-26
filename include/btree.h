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

int insert(int rrnAtual, data_index *data, int * promoRchild,  data_index* promoKey, file_object_btree * bTree, int* altura);

void insereOrdenadoOnWPage(WPAGE *wPag, data_index * data, int newP);

data_index * getMiddle(WPAGE * wPag, int *promoRChild, file_object_btree *bTree);

void split(data_index *newKey, int newP, PAGE * pag, data_index *promoKey, int *promoRChild, PAGE * newPag, file_object_btree *bTree);

void copyFromWPageToPageBeforePromo(WPAGE *wPag, PAGE *pag, int promoKey);

void copyFromWPageToPageFollowingPromo(WPAGE *wPag, PAGE *pag, int promoKey);

void driver(char *btreeFile, data_index** arr, int nroReg);

int64_t search(int rrn, int chave, int *found_RRN, int *found_POS, file_object_btree *bTree);

int getRaizRRN(file_object_btree * bTree);

void fecharArquivoBinBTree(file_object_btree** bTree);

int verificaConsistenciaBTree(file_object_btree *bTree);

#endif

