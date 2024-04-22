#ifndef FILE_UTILS
#define FILE_UTILS
#include <stdio.h>
#include <stdint.h>

typedef struct header_registry_ header_registry;
typedef struct data_registry_ data_registry;
typedef struct file_object_ file_object;

/*
 * Definindo as funções para manipulação do arquivo e setters e getters das estruturas de dados
 * do registro de cabeçalho e registros de dados
 * */
file_object* criarArquivoBin(char *bin_name);
void fecharArquivoBin(file_object** fileObj);
data_registry* criarRegistro();
void writeRegistroDados(file_object* fileObj, data_registry* registro);
void liberarRegistro(data_registry** registro);
int getTamNomeClube(data_registry* registro);
int getTamNacionalidade(data_registry* registro);
int getTamNomeJogador(data_registry* registro);
void setId(data_registry* registro, int id);
void setIdade(data_registry* registro, int idade);
void setTamNomeJogador(data_registry* registro, int tam);
void setNomeJogador(data_registry* registro, char* nomeJogador);
void setTamNacionalidade(data_registry* registro, int tam);
void setNacionalidade(data_registry* registro, char* nacionalidade);
void setTamNomeClube(data_registry* registro, int tam);
void setNomeClube(data_registry* registro, char* nomeClube);
void setProx(data_registry* registro, int index);
void setRemovido(data_registry* registro, int op);
void setTamanhoRegistro(data_registry* registro, int tam);
void writeRegistroCabecalho(file_object* fileObj);
void setHeaderStatus(file_object* fileObj, char status);
void setHeaderTopo(file_object* fileObj, int64_t topo);
void setHeaderProxByteOffset(file_object* fileObj, int64_t proxByteOffset);
void setHeaderNroRegArq(file_object* fileObj, int32_t nroRegArq);
void setHeaderNroRegRem(file_object* fileObj, int32_t nroRegRem);
void binarioNaTela(char *nomeArquivoBinario);
void scan_quote_string(char *str);
#endif
