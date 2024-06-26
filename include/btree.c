#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "file_utils.h"
#include "btree.h"

enum Mode {
    ERRO = 1,
    NO_PROMOTION,
    PROMOTION
};

struct file_object_btree_ {
    header_btree *header;
    FILE *file;
};

struct header_btree_{
    char status;        // status do arquivo para saber se ele esta consistente
    int noRaiz;         // RRN do nó raiz
    int proxRRN;        // proximo RRN para ser usado para armazenar um nó
    int nroChaves;      //  numero de chaves que estão na arvore
};

struct PAGE_ {
    int alturaNo;           // altura de um nó
    int nroChaves;          // numero de chaves do nodo
    int c[3];         // chaves ID
    int64_t pr[3];  // byteOff do registro com tal id no registro de dados
    int p[4];     // rrn para alcançar outros registros no indice
};

struct WPAGE_ {
    int alturaNo;           // altura de um nó
    int nroChaves;          // numero de chaves do nodo
    int c[4];         // chaves ID
    int64_t pr[4];  // byteOff do registro com tal id no registro de dados
    int p[5];     // rrn para alcançar outros registros no indice
};

WPAGE *criarWorkingPage(PAGE *pag) {
    WPAGE *wPage = (WPAGE *)malloc(sizeof(WPAGE));

    wPage->alturaNo = pag->alturaNo;
    wPage->nroChaves = pag->nroChaves;

    for (int i = 0; i < 3; i++) {
        wPage->c[i] = pag->c[i];
        wPage->pr[i] = pag->pr[i];
        wPage->p[i] = pag->p[i];
    }

    wPage->p[3] = pag->p[3];

    wPage->c[3] = -1;
    wPage->pr[3] = -1;
    wPage->p[4] = -1;

    return wPage;
}

int getAndIncrementaRRN(file_object_btree * bTree){
    return bTree->header->proxRRN++; 
}

PAGE * criarPagina() {
    PAGE *page = malloc(sizeof(PAGE));
    if (page != NULL) {
        page->alturaNo = 0;
        page->nroChaves = 0;
        for (int i = 0; i < 3; i++) page->c[i] = -1;
        for (int i = 0; i < 3; i++) page->pr[i] = -1;
        for (int i = 0; i < 4; i++) page->p[i] = -1;
    }
    return page;
}

void lerPagina(int rrn, PAGE *page, file_object_btree *bTree) {
    jumpToRRN(bTree, rrn);
    fread(&(page->alturaNo), sizeof(int), 1, bTree->file);
    fread(&(page->nroChaves), sizeof(int), 1, bTree->file);
    fread(page->c, sizeof(int), 3, bTree->file);
    fread(page->pr, sizeof(int64_t), 3, bTree->file);
    fread(page->p, sizeof(int), 4, bTree->file);
}

void fecharArquivoBinBTree(file_object_btree** bTree){
    fclose((*bTree)->file);
    free((*bTree)->header);
    free(*bTree);
    *bTree = NULL;
}

int verificaConsistenciaBTree(file_object_btree *bTree){
   if(bTree->file==NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    if(bTree->header->status=='0'){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    return 1;
}

void inicializaHeaderBTree(file_object_btree *bTree) {
    jumpToRRN(bTree, 0);
    fread(&(bTree->header->status), sizeof(char), 1, bTree->file);
    fread(&(bTree->header->noRaiz), sizeof(int), 1, bTree->file);
    fread(&(bTree->header->proxRRN), sizeof(int), 1, bTree->file);
    fread(&(bTree->header->nroChaves), sizeof(int), 1, bTree->file);
}

void escreverHeaderBTree(file_object_btree *bTree) {
    jumpToRRN(bTree, 0);
    fwrite(&(bTree->header->status), sizeof(char), 1, bTree->file);
    fwrite(&(bTree->header->noRaiz), sizeof(int), 1, bTree->file);
    fwrite(&(bTree->header->proxRRN), sizeof(int), 1, bTree->file);
    fwrite(&(bTree->header->nroChaves), sizeof(int), 1, bTree->file);
}

int getRaizRRN(file_object_btree * bTree){
    return bTree->header->noRaiz;
}

file_object_btree * criarArquivoBinBtree(char *bin_name, char *mode){
    file_object_btree *bTree = (file_object_btree*) malloc(sizeof(file_object_btree));
    header_btree *header = (header_btree*) malloc(sizeof(header_btree));
    bTree->header = header;
    FILE* bin = fopen(bin_name, mode);
    bTree->file = bin;
    if(strcmp(mode, "rb")==0 || strcmp(mode, "rb+")==0){
        inicializaHeaderBTree(bTree);
    }
    
    return bTree;
}

int searchKeyOnPage(PAGE *page, data_index *data) {
    int pos = 0;

    int key = getIndiceId(data);
    for (int i = 0; i < 3; i++) {
        if (key == page->c[i]) {
            return -1;
        }
        else if (key > page->c[i]){
            pos++;
        }
    }
    return pos;
}

int full(PAGE *page) { // retorna se a pagina esta cheia ou nao
    return page->nroChaves == 3 ? 1 : 0;
}

void insereDadoPage(PAGE *page, data_index *data, int newP) {
    int pos = 0;
    int newKey = getIndiceId(data);
    int newPr = getByteOff(data);

    while (pos < page->nroChaves && page->c[pos] < newKey) {
        pos++;
    }

    for (int i = page->nroChaves; i > pos; i--) {
        page->c[i] = page->c[i - 1];
        page->pr[i] = page->pr[i - 1];
        page->p[i + 1] = page->p[i];
    }

    page->c[pos] = newKey;
    page->pr[pos] = newPr;
    page->p[pos + 1] = newP;

    page->nroChaves++;

    if (pos == 0) {
        page->p[0] = newP;
    }
}

void jumpToRRN(file_object_btree * bTree, int RRN){
    fseek(bTree->file, 13+RRN*60, SEEK_SET);
}

void escrevePaginaNaBTree(file_object_btree * bTree, PAGE * pag){
    fwrite(&(pag->alturaNo), 4, 1, bTree->file);
    fwrite(&(pag->nroChaves), 4, 1, bTree->file);
    fwrite(pag->c, 4, 3, bTree->file);
    fwrite(pag->pr, 8, 3, bTree->file);
    fwrite(pag->p, 4, 4, bTree->file);
}

void escreveRegistroBtree(PAGE * pag, int RRN, file_object_btree * bTree){
    jumpToRRN(bTree, RRN);
    escrevePaginaNaBTree(bTree, pag);
}

int insert(int rrnAtual, data_index *data, int * promoRchild,  data_index* promoKey, file_object_btree * bTree, int* altura){
    if(rrnAtual==-1){
        promoKey = data;
        *promoRchild = -1;
       // altura = 0;
        return PROMOTION;
    }else{
        PAGE * page = criarPagina();
        lerPagina(rrnAtual, page, bTree);
        int pos = searchKeyOnPage(page, data);

        if(pos==-1){
            return ERRO;
        }

        data_index *pbKEY = criarDataIndex(-1, -1);
        int pbRRN;

        int retorno = insert(page->p[pos], data, &pbRRN, pbKEY, bTree, NULL);

        if(retorno == NO_PROMOTION || retorno == ERRO){
            return retorno;
        }else if(!full(page)){
            insereDadoPage(page, pbKEY, pbRRN);
            return NO_PROMOTION;
        }else{
            PAGE * newPage = criarPagina();
            newPage->alturaNo = page->alturaNo;
            split(pbKEY, pbRRN, page, promoKey, promoRchild, newPage, bTree);
            escreveRegistroBtree(page, rrnAtual, bTree);
            escreveRegistroBtree(newPage,*promoRchild, bTree);
            if (altura != NULL) *altura = page->alturaNo+1;
            return PROMOTION;
        }
    }
}

void insereOrdenadoOnWPage(WPAGE *wPag, data_index * data, int newP) {
    // Find the correct position to insert the new key
    int pos = 0;
    int newKey = getIndiceId(data);
    int newPr = getByteOff(data);

    while (pos < wPag->nroChaves && wPag->c[pos] < newKey) {
        pos++;
    }

    // Shift keys and pointers to the right to make space for the new key
    for (int i = wPag->nroChaves; i > pos; i--) {
        wPag->c[i] = wPag->c[i - 1];
        wPag->pr[i] = wPag->pr[i - 1];
        wPag->p[i + 1] = wPag->p[i];
    }

    // Insert the new key and its data pointer
    wPag->c[pos] = newKey;
    wPag->pr[pos] = newPr;

    wPag->p[pos + 1] = newP;
    // Update the number of keys in the working page
    wPag->nroChaves++;
}

data_index * getMiddle(WPAGE * wPag, int *promoRChild, file_object_btree *bTree){
    data_index * data = criarDataIndex(wPag->c[2], wPag->pr[2]);
    *promoRChild = getAndIncrementaRRN(bTree);
    return data;
}

void split(data_index *newKey, int newP, PAGE * pag, data_index *promoKey, int *promoRChild, PAGE * newPag, file_object_btree *bTree){
    WPAGE * wPag = criarWorkingPage(pag);

    insereOrdenadoOnWPage(wPag, newKey, newP);

    promoKey = getMiddle(wPag, promoRChild, bTree);

    int chave = getIndiceId(promoKey);
    
    // copia da working page para a pagina atual da insert
    copyFromWPageToPageBeforePromo(wPag, pag, chave);

    // copia da working page para a nova pagina de split
    copyFromWPageToPageFollowingPromo(wPag, newPag, chave);
}

void copyFromWPageToPageBeforePromo(WPAGE *wPag, PAGE *pag, int promoKey) {
    // Copy keys and associated data pointers and child pointers
    int pos = 0;
    while (pos < wPag->nroChaves && wPag->c[pos] < promoKey) {
        pag->c[pos] = wPag->c[pos];
        pag->pr[pos] = wPag->pr[pos];
        pag->p[pos] = wPag->p[pos];
        pos++;
    }

    // Copy remaining child pointer if any
    pag->p[pos] = wPag->p[pos];
    pag->nroChaves = pos; // Update number of keys in the page
}

void copyFromWPageToPageFollowingPromo(WPAGE *wPag, PAGE *pag, int promoKey) {
    // Start copying after the promotion key
    int pos=3;

    int start = pos;
    pos = 0;

    pag->c[pos] = wPag->c[start];
    pag->pr[pos] = wPag->pr[start];
    pag->p[pos] = wPag->p[start];
    pos++;
    start++;

    pag->p[pos] = wPag->p[start];
    pag->nroChaves = pos; // Update number of keys in the page
}

void driver(char *btreeFile, data_index** arr, int i){
    // Open or create the B-tree file
    file_object_btree *bTree = criarArquivoBinBtree(btreeFile, "rb+");
    if (bTree == NULL) {
        // File doesn't exist, create a new B-tree file
        bTree = criarArquivoBinBtree(btreeFile, "wb+");
        bTree->header->status = '1';
        bTree->header->noRaiz = -1;
        bTree->header->proxRRN = 0;
        bTree->header->nroChaves = 0;

    } else {
        if(!verificaConsistenciaBTree(bTree))
            return;
        // File exists, read the root RRN
        fseek(bTree->file, 13, SEEK_SET);
        fread(&(bTree->header->noRaiz), 4, 1, bTree->file);
    }

    int ROOT = bTree->header->noRaiz; // get RRN of root

    int nroReg=i;
    for(int i=0;i<nroReg;i++){ // Continue while there are keys to insert
        data_index * KEY = arr[i];// Initialize with the first key value to insert

        int PROMO_R_CHILD;
        data_index *PROMO_KEY = criarDataIndex(-1, -1);
        int alturaPag = 0;
        if (insert(ROOT, KEY, &PROMO_R_CHILD, PROMO_KEY, bTree, &alturaPag) == PROMOTION) {
            // Create a new root page with key := PROMO_KEY, left child := ROOT and right child := PROMO_R_CHILD
            PAGE *newRootPage = criarPagina();
            newRootPage->alturaNo = alturaPag; // Height of the new root
            newRootPage->c[0] = getIndiceId(PROMO_KEY);
            newRootPage->pr[0] = getByteOff(PROMO_KEY);
            newRootPage->p[0] = ROOT;
            newRootPage->p[1] = PROMO_R_CHILD;
            newRootPage->nroChaves = 1;

            // Set ROOT to RRN of the new root page
            ROOT = getAndIncrementaRRN(bTree);
            bTree->header->noRaiz = ROOT;

            // Write the new root page to the file
            escreveRegistroBtree(newRootPage, ROOT, bTree);
            bTree->header->nroChaves++;
            free(newRootPage);
        }

        // Get the next key and store it in KEY
        // You will need a way to get the next key, for example, from user input or a file
        // KEY = ; // Read the next key to insert
    }

    // Write the updated root RRN and other header info back to B-tree file
    fseek(bTree->file, 0, SEEK_SET);
    fwrite(bTree->header, sizeof(header_btree), 1, bTree->file);

    // Close B-tree file
    fecharArquivoBinBTree(&bTree);
}

int64_t search(int rrn, int chave, int *found_RRN, int *found_POS, file_object_btree *bTree) {
    if (rrn == -1) {
        return -1; // Key not found
    } else {
        PAGE *page = criarPagina();
        lerPagina(rrn, page, bTree); // Read the page into the PAGE struct

        int pos = 0;
        while (pos < page->nroChaves && chave > page->c[pos]) {
            pos++;
        }

        if (pos < page->nroChaves && chave == page->c[pos]) {
            *found_RRN = rrn;  // The current RRN contains the key
            *found_POS = pos;  // The position in the current page
            return page->pr[pos]; // Return the data pointer (byte offset)
        } else {
            int next_rrn = page->p[pos];
            return search(next_rrn, chave, found_RRN, found_POS, bTree);
        }
    }
}
