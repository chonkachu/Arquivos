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

/* funçao que cria a working page da Btree a partir da current page conforme definiçao
*/
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

// funçao auxiliar que retorna e incrementa RRN numa inserçao na BTree
int getAndIncrementaRRN(file_object_btree * bTree){
    return bTree->header->proxRRN++; 
}

// funçao que retorna uma nova pagina vazia 
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

// funçao que le a pagina num dado RRN no arquivo de indice da Btree
void lerPagina(int rrn, PAGE *page, file_object_btree *bTree) {
    jumpToRRN(bTree, rrn);
    fread(&(page->alturaNo), sizeof(int), 1, bTree->file);
    fread(&(page->nroChaves), sizeof(int), 1, bTree->file);
    for (int i = 0; i < 3; i++) {
        fread(&page->c[i], sizeof(int), 1, bTree->file);
        fread(&page->pr[i], sizeof(int64_t), 1, bTree->file);
    }
     for (int i = 0; i < 4; i++) {
        fread(&page->p[i], sizeof(int), 1, bTree->file);
    }
}

// funçao que fecha o arquivo da Btree e libera a memoria
void fecharArquivoBinBTree(file_object_btree** bTree){
    fclose((*bTree)->file);
    free((*bTree)->header);
    free(*bTree);
    *bTree = NULL;
}

// funcao que verifica consistencia e existencia do indice
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

// funçao que inicializa o cabeçalho da Btree
void inicializaHeaderBTree(file_object_btree *bTree) {
    fseek(bTree->file, 0, SEEK_SET);
    fread(&(bTree->header->status), 1, 1, bTree->file);
    fread(&(bTree->header->noRaiz), 4, 1, bTree->file);
    fread(&(bTree->header->proxRRN), 4, 1, bTree->file);
    fread(&(bTree->header->nroChaves), 4, 1, bTree->file);
}

// funçao que escreve o header da Btree de volta no arquivo de indice
void escreverHeaderBTree(file_object_btree *bTree) {
    fseek(bTree->file, 0, SEEK_SET);
    fwrite(&(bTree->header->status), 1, 1, bTree->file);
    fwrite(&(bTree->header->noRaiz), 4, 1, bTree->file);
    fwrite(&(bTree->header->proxRRN), 4, 1, bTree->file);
    fwrite(&(bTree->header->nroChaves), 4, 1, bTree->file);
    char dollar = '$';
    for (int i = 13; i < 60; i++) {
        fwrite(&dollar, 1, 1, bTree->file);
    }
}

// metodo para se obter da Raiz da Btree
int getRaizRRN(file_object_btree * bTree){
    return bTree->header->noRaiz;
}

// funçao que cria o arquivo de indice da Btree e inicializa o header
file_object_btree * criarArquivoBinBtree(char *bin_name, char *mode){
    file_object_btree *bTree = (file_object_btree*) malloc(sizeof(file_object_btree));
    header_btree *header = (header_btree*) malloc(sizeof(header_btree));
    bTree->header = header;
    FILE* bin = fopen(bin_name, mode);
    bTree->file = bin;
    if(bTree->file != NULL && (strcmp(mode, "rb")==0 || strcmp(mode, "rb+")==0 || strcmp(mode, "r")==0)){
        inicializaHeaderBTree(bTree);
    }
    
    return bTree;
}

// funçao de busca de uma chave numa pagina da Btree
int searchKeyOnPage(PAGE *page, data_index *data) {
    int pos = 0;

    int key = getIndiceId(data);
    for (int i = 0; i < 3; i++) {
        if (key == page->c[i]) {
            return -1;
        }
        else if (page->c[i] != -1 && key > page->c[i]){
            pos++;
        }
    }
    return pos;
}

// funçao que checa se pagina esta cheia ou nao
int full(PAGE *page) { // retorna se a pagina esta cheia ou nao
    return page->nroChaves == 3 ? 1 : 0;
}

// funçao que insere ordenado na pagina
void insereDadoPage(PAGE *page, data_index *data, int newP) {
    int pos = 0;
    int newKey = getIndiceId(data);
    int newPr = getByteOff(data);

    // percorremos ate chegar num cara maior
    while (pos < page->nroChaves && page->c[pos] < newKey) {
        pos++;
    }

    // shifta os caras a direita da chave em uma posicao
    for (int i = page->nroChaves; i > pos; i--) {
        page->c[i] = page->c[i - 1];
        page->pr[i] = page->pr[i - 1];
        page->p[i + 1] = page->p[i];
    }

    // insere nova chave e byteOff
    page->c[pos] = newKey;
    page->pr[pos] = newPr;
    page->p[pos + 1] = newP;

    page->nroChaves++;
}

// funcao que pula para o byteOff de um RRN desejado
void jumpToRRN(file_object_btree * bTree, int RRN){
    fseek(bTree->file, 60+RRN*60, SEEK_SET);
}

// funçao que escreve uma pagina na Btree conforme especificado
void escrevePaginaNaBTree(file_object_btree * bTree, PAGE * pag){
    fwrite(&(pag->alturaNo), sizeof(pag->alturaNo), 1, bTree->file);
    fwrite(&(pag->nroChaves), sizeof(pag->nroChaves), 1, bTree->file);
    for (int i = 0; i < 3; i++){
        fwrite(&(pag->c[i]), sizeof(pag->c[i]), 1, bTree->file);
        fwrite(&(pag->pr[i]), sizeof(pag->pr[i]), 1, bTree->file);
    } 
    for (int i = 0; i < 4; i++){
        fwrite(&pag->p[i], sizeof(pag->p[i]), 1, bTree->file);
    } 
}

// funcao que escreve registro na Btree
void escreveRegistroBtree(PAGE * pag, int RRN, file_object_btree * bTree){
    jumpToRRN(bTree, RRN);
    escrevePaginaNaBTree(bTree, pag);
}

//funçao de insert na Btree conforme definiçao
int insert(int rrnAtual, data_index *data, int * promoRchild,  data_index* promoKey, file_object_btree * bTree, int* altura){
    if(rrnAtual==-1){ // caso que estamos numa folha
        setIndiceId(promoKey, getIndiceId(data));
        setIndiceByteOff(promoKey, getByteOff(data));
        *promoRchild = -1;
        return PROMOTION;
    }else{
        PAGE * page = criarPagina(); // vamos ler a pagina do RRN atual
        lerPagina(rrnAtual, page, bTree);
        int pos = searchKeyOnPage(page, data);

        if(pos==-1){
            return ERRO;
        }

        data_index *pbKEY = criarDataIndex(-1, -1);
        int pbRRN;

        // descemos na arvore ate achar a posicao desejada para o novo dado
        int retorno = insert(page->p[pos], data, &pbRRN, pbKEY, bTree, NULL); 

        if(retorno == NO_PROMOTION || retorno == ERRO){
            return retorno;
        }
        else if(!full(page)){ // se nao estiver cheio ao promover, insere nesta pagina
            insereDadoPage(page, pbKEY, pbRRN);
            escreveRegistroBtree(page, rrnAtual, bTree);
            return NO_PROMOTION;
        }
        else{
            // ao estar cheio, vamos splitar no segundo elemento e promove-lo
            PAGE * newPage = criarPagina();
            newPage->alturaNo = page->alturaNo;
            split(pbKEY, pbRRN, page, promoKey, promoRchild, newPage, bTree);
            // escreve new page e page de volta no disco
            escreveRegistroBtree(page, rrnAtual, bTree);
            escreveRegistroBtree(newPage,*promoRchild, bTree);
            if (altura != NULL) *altura = page->alturaNo+1; // altura apenas muda se promovermos para raiz
            return PROMOTION;
        }
    }
}

// funçao que insere ordenado numa working page, semelhante a variante da page normal
void insereOrdenadoOnWPage(WPAGE *wPag, data_index * data, int newP) {
    int pos = 0;
    int newKey = getIndiceId(data);
    int newPr = getByteOff(data);

    while (pos < wPag->nroChaves && wPag->c[pos] < newKey) {
        pos++;
    }

    for (int i = wPag->nroChaves; i > pos; i--) {
        wPag->c[i] = wPag->c[i - 1];
        wPag->pr[i] = wPag->pr[i - 1];
        wPag->p[i + 1] = wPag->p[i];
    }

    wPag->c[pos] = newKey;
    wPag->pr[pos] = newPr;

    wPag->p[pos + 1] = newP;
    wPag->nroChaves++;
}

// funçao que pega o segundo elemento da working page e o promove
data_index * getMiddle(WPAGE * wPag, int *promoRChild, file_object_btree *bTree){
    data_index * data = criarDataIndex(wPag->c[1], wPag->pr[1]);
    *promoRChild = getAndIncrementaRRN(bTree);
    return data;
}

// funçao que copia os elementos a esquerda do promovido de working page para page
void copyFromWPageToPageBeforePromo(WPAGE *wPag, PAGE *pag, int promoKey) {
    int pos = 0;
    // vamos re-inicializar tudo para -1
    for (int i = 0; i < 3; i++) pag->c[i] = -1;
    for (int i = 0; i < 3; i++) pag->pr[i] = -1;
    for (int i = 0; i < 4; i++) pag->p[i] = -1;

    while (pos < wPag->nroChaves && wPag->c[pos] < promoKey) {
        pag->c[pos] = wPag->c[pos];
        pag->pr[pos] = wPag->pr[pos];
        pag->p[pos] = wPag->p[pos];
        pos++;
    }

    pag->p[pos] = wPag->p[pos];
    pag->nroChaves = pos; 
}
// funçao que copia os elementos a direita do promovido de working page para new page
void copyFromWPageToPageFollowingPromo(WPAGE *wPag, PAGE *pag, int promoKey) {

    int start = 2; // splita no 1, comeca no 2
    int pos = 0;

    while (start < wPag->nroChaves) {
        pag->c[pos] = wPag->c[start];
        pag->pr[pos] = wPag->pr[start];
        pag->p[pos] = wPag->p[start];
        pos++;
        start++;
    }

    pag->p[pos] = wPag->p[start];
    pag->nroChaves = pos;
}

void split(data_index *newKey, int newP, PAGE * pag, data_index *promoKey, int *promoRChild, PAGE * newPag, file_object_btree *bTree){
    WPAGE * wPag = criarWorkingPage(pag);

    insereOrdenadoOnWPage(wPag, newKey, newP);

    // ṕegamos a do meio para promover
    data_index *mid = getMiddle(wPag, promoRChild, bTree);
    setIndiceId(promoKey, getIndiceId(mid));
    setIndiceByteOff(promoKey, getByteOff(mid));
    
    //id da chave a ser promovida
    int chave = getIndiceId(promoKey);
    
    // copia da working page para a pagina atual da insert antes da chave promovida
    copyFromWPageToPageBeforePromo(wPag, pag, chave);

    // copia da working page para a nova pagina de split as chaves depois das chaves promovidas
    copyFromWPageToPageFollowingPromo(wPag, newPag, chave);
}
void driver(char *btreeFile, data_index** arr, int nroReg){
    // abre ou se cria a  btree
    file_object_btree *bTree = criarArquivoBinBtree(btreeFile, "rb+");
    if (bTree->file == NULL) {
        // nao existe arquivo entao se cria
        bTree = criarArquivoBinBtree(btreeFile, "wb+");
        bTree->header->status = '0';
        bTree->header->noRaiz = -1;
        bTree->header->proxRRN = 0;
        bTree->header->nroChaves = 0;
        escreverHeaderBTree(bTree);

    } else {
        if(!verificaConsistenciaBTree(bTree))
            return;
    }

    int ROOT = bTree->header->noRaiz; // pega-se o rrn da raiz atual

    int num = nroReg;
    // inserindo as chaves
    for(int i = 0; i < num; i++){ 
        data_index * KEY = arr[i];// pegamos o proximo valor  a ser inserido

        int PROMO_R_CHILD;
        data_index *PROMO_KEY = criarDataIndex(-1, -1);
        int alturaPag = 0;
        bTree->header->nroChaves++;
        if (insert(ROOT, KEY, &PROMO_R_CHILD, PROMO_KEY, bTree, &alturaPag) == PROMOTION) {
            // nova pagina raiz com a chave sendo PROMO_KEY filho esquerdo sendo o rrn ROOT e esquerdo sendo PROMO_R_CHILD
            PAGE *newRootPage = criarPagina();
            newRootPage->alturaNo = alturaPag; 
            newRootPage->c[0] = getIndiceId(PROMO_KEY);
            newRootPage->pr[0] = getByteOff(PROMO_KEY);
            newRootPage->p[0] = ROOT;
            newRootPage->p[1] = PROMO_R_CHILD;
            newRootPage->nroChaves = 1;

            // pegamos o proximoRRN
            ROOT = getAndIncrementaRRN(bTree);
            bTree->header->noRaiz = ROOT;

            // escrevemos a nova pagina raiz
            escreveRegistroBtree(newRootPage, ROOT, bTree);
            free(newRootPage);
        }
    }

    // escrevemos o cabeçalho atualizado
    bTree->header->status = '1';
    escreverHeaderBTree(bTree);

    fecharArquivoBinBTree(&bTree);
}

// funçao de pesquisa na btree
int64_t search(int rrn, int chave, int *found_RRN, int *found_POS, file_object_btree *bTree) {
    if (rrn == -1) {
        return -1; // nao foi encontrado a chave (id) portanto retorna-se -1
    } else {
        PAGE *page = criarPagina();
        lerPagina(rrn, page, bTree); // lemos a pagina e guardamos

        int pos = 0;
        while (pos < page->nroChaves && chave > page->c[pos]) {
            pos++;
        } // encontra-se a posiçao que deve estar ou o prox rrn que tenho que ir

        if (pos < page->nroChaves && chave == page->c[pos]) {
            *found_RRN = rrn;  // guarda o rrn que contem a chave
            *found_POS = pos;  // a pos dela na pagina
            return page->pr[pos]; // retorno o byteoffset
        } else {
            int next_rrn = page->p[pos];
            return search(next_rrn, chave, found_RRN, found_POS, bTree); // chamo search para o proximo rrn
        }
    }
}
