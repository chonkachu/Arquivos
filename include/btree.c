#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "file_utils.h"
#include "db.h"
#include "btree.h"

enum Mode {
    ERRO = 1,
    NO_PROMOTION,
    PROMOTION
};

struct file_object_btree_ {
    header_index *header;
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
    int c1, c2, c3;         // chaves ID
    int64_t pr1, pr2, pr3;  // byteOff do registro com tal id no registro de dados
    int p1, p2, p3, p4;     // rrn para alcançar outros registros no indice
};

PAGE * criarPagina(){
    PAGE * page = malloc(sizeof(page));
    page->alturaNo=0;
    page->nroChaves=0;
    page->c1=-1;
    page->c2=-1;
    page->c3=-1;
    page->pr1=-1;
    page->pr2=-1;
    page->pr3=-1;
    page->p1=-1;
    page->p2=-1;
    page->p3=-1;
    page->p4=-1;
    return page;
}

void lerPagina(int rrn, PAGE * page){
    file_object_btree * bin = criarArquivoBinBTree(); 
}

int insert(int rrnAtual, int chave, int * promoRchild, int * promoKey){
    if(rrnAtual==-1){
        promoRchild=chave;
        promoRchild=-1;
        return PROMOTION;
    }else{
        PAGE * page = criarPagina();
        lerPagina(rrnAtual, page);
        int pos = searchKey(page);

        if(pos==-1){
            return ERRO;
        }

        int pbRRN;
        int pbKEY;

        retorno = insert(page.child[pos], chave, &pbRRN, &pbKEY);

        if(retorno == NO_PROMOTION || retorno == ERRO){
            return retorno;
        }else if(!full(page)){
            page.insert(pbRRN, pbKEY);
            return NO_PROMOTION;
        }else{
            PAGE * newPage = criarPagina();
            split(pbKEY, pbRRN, page, promoKey, promoRchild, newPage);
            escreveRegistroBtree(page, rrnAtual);
            escreveRegisroBtree(newPage,*promoRchild);
            return PROMOTION;
        }
    }
}