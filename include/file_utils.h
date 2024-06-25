#ifndef FILE_UTILS
#define FILE_UTILS
#include <stdio.h>
#include <stdint.h>

typedef struct header_registry_ header_registry;
typedef struct data_registry_ data_registry;
typedef struct file_object_ file_object;
typedef struct file_object_ind_ file_object_ind;
typedef struct header_index_ header_index;
typedef struct data_index_ data_index;
typedef struct player_data_ player_data;

#define CREATE 0
#define INSERT 1
#define DELETE 2
#define SELECT 3
/*
 * Definindo as funções para manipulação do arquivo e setters e getters das estruturas de dados
 * do registro de cabeçalho e registros de dados
 * */

FILE * getFile(file_object * bin);
int getNroRegArq(file_object * bin);
int getNroRegRem(file_object * bin);
int64_t getTopo(file_object * bin);
int64_t getFim(file_object *bin);
int idbuscado(player_data *player);
int comparaPlayer(player_data *p1, player_data *p2);
player_data * lerPlayerData(int op);
file_object* criarArquivoBin(char *bin_name, char *mode);
int64_t tamanhoBin(file_object *bin);
void fecharArquivoBin(file_object** fileObj);
int verificaConsistencia(file_object* bin);
int processaRegistro(file_object *bin, data_registry *data);
int processaRegistroPlayer(file_object *bin, player_data* player);
data_registry* processaRegistroRemovido(file_object *bin, int64_t byteOff);
void liberaPlayer(player_data* player);
void inicializaHeader(file_object *bin);
void inicioRegistroDeDados(file_object *bin);
void fimRegistroDeDados(file_object *bin);
player_data* criarPlayer();
data_registry* criarRegistro();
void gotoByteOffArquivoBin(file_object *bin, int64_t byteOff);
data_registry* criarRegistroFromPlayer(player_data *player);
void writeRegistroDados(file_object* fileObj, data_registry* registro);
void liberarRegistro(data_registry** registro);
int getIdRegistro(data_registry *registro);
int getTamRegistro(data_registry *registro);
int64_t getProx(data_registry *registro);
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
int comparaIndice(const void *a, const void *b);
void setHeaderStatusInd(file_object_ind* fileObj, char status);
void writeRegistroCabecalhoInd(file_object_ind* fileObj);
void writeRegistroDadosInd(file_object_ind* fileObj, data_index** arr, int i);
void setIndiceByteOff(data_index* a, int64_t byteOff);
void fecharArquivoBinInd(file_object_ind** fileObj);
void setIndiceId(data_index* a, int32_t id);
data_index** criarVetorIndice(int n);
int32_t getIndiceId(data_index* a);
int64_t getByteOff(data_index* a);
data_index* criarDataIndex(int id, int byteOff);
int64_t indBB(int id, char *bin_name, int nroRegArq);
file_object_ind* criarArquivoBinInd(char *bin_name);
void imprimePlayerData(player_data *player);


void binarioNaTela(char *nomeArquivoBinario);
void scan_quote_string(char *str);

#endif
