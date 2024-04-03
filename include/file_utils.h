#ifndef FILE_UTILS
#define FILE_UTILS
#include <stdio.h>
#include <stdint.h>

typedef struct header_registry_ header_registry;
typedef struct data_registry_ data_registry;

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
#endif
