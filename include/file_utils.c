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

void setId(data_registry* registro, int id){
    registro->id = id;
}

void setIdade(data_registry* registro, int idade){
    registro->idade = idade;
}

void setTamNomeJogador(data_registry* registro, int tam){
    registro->tamNomeJog = tam;
}
void setNomeJogador(){

}

void setTamNacionalidade(data_registry* registro, int tam){
    registro->tamNacionalidade = tam;
}
void setNacionalidade(){

}

void setTamNomeClube(data_registry* registro, int tam){
    registro->tamNomeClube = tam;
}

void setNomeClube(){

}

void setProx(){

}

void setRemovido(){

}

void setTamanhoRegistro(data_registry* registro, int tam){
    registro->tamanhoRegistro = tam;
}
