#include <stdlib.h>
#include "file_utils.h"

struct header_registry_ {
    char status;
    int64_t topo;
    int64_t proxByteOffset;
    int32_t nroRegArq;
    int32_t nroRegRem;
};

struct data_registry_ {
    char removido;
    int32_t tamanhoRegistro;
    int64_t prox;
    int32_t id;
    int32_t idade;
    int32_t tamNomeJog;
    int32_t tamNacionalidade;
    int32_t tamNomeClube;
    char *nomeJogador;
    char *nacionalidade;
    char *nomeClube;
};

struct file_object_ {
    header_registry *header;
    FILE *file;
    int32_t fileIndex;
};

file_object* abrirArquivoBin(char *bin_name){
    file_object *fileObj = (file_object*) malloc(sizeof(file_object));
    header_registry *header = (header_registry*) malloc(sizeof(header_registry));
    FILE* bin = fopen(bin_name, "wb+");
    fileObj->header = header;
    fileObj->file = bin;
    fileObj->fileIndex = 0;
    
    return fileObj;
}
// supondo que o indice do arquivo ta onde a gnt precisa
void writeRegistroDados(file_object* fileObj, data_registry* registro) {
    fwrite(&registro->removido, 1, 1, fileObj->file);
    fwrite(&registro->tamanhoRegistro, 4, 1, fileObj->file);
    fwrite(&registro->prox, 8, 1, fileObj->file);
    fwrite(&registro->id, 4, 1, fileObj->file);
    fwrite(&registro->idade, 4, 1, fileObj->file);
    fwrite(&registro->tamNomeJog, 4, 1, fileObj->file);
    fwrite(registro->nomeJogador, 1, registro->tamNomeJog, fileObj->file);
    fwrite(&registro->tamNacionalidade, 4, 1, fileObj->file);
    fwrite(registro->nacionalidade, 1, registro->tamNacionalidade, fileObj->file);
    fwrite(&registro->tamNomeClube, 4, 1, fileObj->file);
    fwrite(registro->nomeClube, 1, registro->tamNomeClube, fileObj->file);
}

void fecharArquivoBin(file_object** fileObj) {
    fclose((*fileObj)->file);
    free((*fileObj)->header);
    *fileObj = NULL;
}

void setId(data_registry* registro, int id){
    registro->id = id;
}

void setIdade(data_registry* registro, int idade){
    registro->idade = idade;
}

void setTamNomeJogador(data_registry* registro, int tam){
    registro->tamNomeJog = tam;
}
void setNomeJogador(data_registry* registro, char* nomeJogador){
    free(registro->nomeJogador);
    registro->nomeJogador = nomeJogador;
}

void setTamNacionalidade(data_registry* registro, int tam){
    registro->tamNacionalidade = tam;
}
void setNacionalidade(data_registry* registro, char* nacionalidade){
    free(registro->nacionalidade);
    registro->nacionalidade = nacionalidade;
}

void setTamNomeClube(data_registry* registro, int tam){
    registro->tamNomeClube = tam;
}

void setNomeClube(data_registry* registro, char* nomeClube){
    free(registro->nomeClube);
    registro->nomeClube = nomeClube;
}

void setProx(data_registry* registro, int index){
    registro->prox = index;
}

void setRemovido(data_registry* registro, int op){
    registro->removido = op; 
}

void setTamanhoRegistro(data_registry* registro, int tam){
    registro->tamanhoRegistro = tam;
}

void setHeaderStatus(header_registry* cabecalho, char status){
    cabecalho->status = status;
}
void setHeaderTopo(header_registry* cabecalho, int64_t topo){
    cabecalho->topo = topo;
}
void setHeaderProxByteOfffset(header_registry* cabecalho, int64_t proxByteOffset){
    cabecalho->proxByteOffset = proxByteOffset;
}
void setHeaderNroRegArq(header_registry* cabecalho, int32_t nroRegArq){
    cabecalho->nroRegArq = nroRegArq;
}
void setHeaderNroRegRem(header_registry* cabecalho, int32_t nroRegRem){
    cabecalho->nroRegRem = nroRegRem;
}
