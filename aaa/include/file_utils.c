#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "file_utils.h"

struct header_registry_ {
    char status;        // status do arquivo para saber se ele esta consistente
    int64_t topo;           // byteOff do ultimo registro removido
    int64_t proxByteOffset;         // primeiro byte depois do fim dos registros
    int32_t nroRegArq;          // a quantidade de registros
    int32_t nroRegRem;          // numero de registros removidos
};

struct data_registry_ {
    char removido; // byte que sinaliza que o registro está removido
    int32_t tamanhoRegistro; // tamanho do registro de dados
    int64_t prox; // byteoffset do proximo elemento removido
    int32_t id; // campo id do registro de dados
    int32_t idade; // campo idade do registro de dados
    int32_t tamNomeJog; // campo de tamanho do nome do jogador do registro de dados
    int32_t tamNacionalidade; // campo de tamanho da nacionalidade do registro de dados
    int32_t tamNomeClube; // campo de tamanho do nome do clube do registro de dados
    char *nomeJogador; // campo de nome do jogador do registro de dados
    char *nacionalidade; // campo de nacionalidade do registro de dados
    char *nomeClube; // campo de nome do clube do registro de dados
};

// estrutura que guarda o ponteiro para o arquivo e o registro de cabeçalho do arquivo
struct file_object_ {
    header_registry *header;
    FILE *file;
    int32_t fileIndex;
};
// função que inicializa a estrutura de dados file_object
file_object* criarArquivoBin(char *bin_name){
    file_object *fileObj = (file_object*) malloc(sizeof(file_object));
    header_registry *header = (header_registry*) malloc(sizeof(header_registry));
    FILE* bin = fopen(bin_name, "wb+");
    fileObj->header = header;
    fileObj->file = bin;
    fileObj->fileIndex = 0;
    
    return fileObj;
}

// função que inicializa os registros de dados
data_registry* criarRegistro() {
    data_registry *registro = (data_registry*) malloc(sizeof(data_registry));
    registro->nacionalidade = NULL;
    registro->nomeClube = NULL;
    registro->nomeJogador = NULL;

    return registro;
}

// função que escreve os registros de dados no arquivo binario
void writeRegistroDados(file_object* fileObj, data_registry* registro) {
    fwrite(&registro->removido, 1, 1, fileObj->file);
    fwrite(&registro->tamanhoRegistro, 4, 1, fileObj->file);
    fwrite(&registro->prox, 8, 1, fileObj->file);
    fwrite(&registro->id, 4, 1, fileObj->file);
    fwrite(&registro->idade, 4, 1, fileObj->file);
    fwrite(&registro->tamNomeJog, 4, 1, fileObj->file);
    if (registro->nomeJogador != NULL)
        fwrite(registro->nomeJogador, 1, registro->tamNomeJog, fileObj->file);
    fwrite(&registro->tamNacionalidade, 4, 1, fileObj->file);
    if (registro->nacionalidade != NULL)
        fwrite(registro->nacionalidade, 1, registro->tamNacionalidade, fileObj->file);
    fwrite(&registro->tamNomeClube, 4, 1, fileObj->file);
    if (registro->nomeClube != NULL)
        fwrite(registro->nomeClube, 1, registro->tamNomeClube, fileObj->file);
}
// função que escreve o registro de cabeçalho no arquivo binario
void writeRegistroCabecalho(file_object* fileObj) {
    fseek(fileObj->file, 0, SEEK_SET);
    fwrite(&fileObj->header->status, 1, 1, fileObj->file);
    fwrite(&fileObj->header->topo, 8, 1, fileObj->file);
    fwrite(&fileObj->header->proxByteOffset, 8, 1, fileObj->file);
    fwrite(&fileObj->header->nroRegArq, 4, 1, fileObj->file);
    fwrite(&fileObj->header->nroRegRem, 4, 1, fileObj->file);
    fseek(fileObj->file, 0, SEEK_END);
}

// função que libera memoria dos registros
void liberarRegistro(data_registry **registro) {
    if ((*registro)->nomeJogador != NULL)
        free((*registro)->nomeJogador);
    if ((*registro)->nacionalidade != NULL)
        free((*registro)->nacionalidade);
    if ((*registro)->nomeClube != NULL)
        free((*registro)->nomeClube);
    free(*registro);
    *registro = NULL;
}

// função que fecha o arquivo e libera memoria do registro de cabeçalho
void fecharArquivoBin(file_object** fileObj) {
    fclose((*fileObj)->file);
    free((*fileObj)->header);
    free(*fileObj);
    *fileObj = NULL;
}

// getter do tamanho nacionalidade do registro de dados
int getTamNacionalidade(data_registry *registro) {
    return registro->tamNacionalidade;
}
// getter do tamanho nome clube do registro de dados
int getTamNomeClube(data_registry *registro) {
    return registro->tamNomeClube;
}
// getter do tamanho nome jogador do registro de dados
int getTamNomeJogador(data_registry *registro) {
    return registro->tamNomeJog;
}
// setter do campo id do registro de dados
void setId(data_registry* registro, int id){
    registro->id = id;
}
// setter do campo idade do registro de dados
void setIdade(data_registry* registro, int idade){
    registro->idade = idade;
}
// setter do campo tamanho nome jogador do registro de dados
void setTamNomeJogador(data_registry* registro, int tam){
    registro->tamNomeJog = tam;
}
// setter do campo nome jogador do registro de dados
void setNomeJogador(data_registry* registro, char* nomeJogador){
    if (registro->nomeJogador != NULL)
        free(registro->nomeJogador);
    registro->nomeJogador = nomeJogador;
}

void setTamNacionalidade(data_registry* registro, int tam){         // define o tamanho da string da nacionalidade
    registro->tamNacionalidade = tam;
}
void setNacionalidade(data_registry* registro, char* nacionalidade){         // define a nacionalidade do registro caso haja
    if (registro->nacionalidade != NULL)
        free(registro->nacionalidade);
    registro->nacionalidade = nacionalidade;
}

void setTamNomeClube(data_registry* registro, int tam){         // define o tamanho do nome do clube 
    registro->tamNomeClube = tam;
}

void setNomeClube(data_registry* registro, char* nomeClube){         // define o nome do clube do registro
    if (registro->nomeClube != NULL)
        free(registro->nomeClube);
    registro->nomeClube = nomeClube;
}

void setProx(data_registry* registro, int index){         // define o proximo 
    registro->prox = index;
}

void setRemovido(data_registry* registro, int op){         // define logicamente se o registro foi removido
    registro->removido = op; 
}

void setTamanhoRegistro(data_registry* registro, int tam){         // define o tamanho do registro
    registro->tamanhoRegistro = tam;
}

void setHeaderStatus(file_object* fileObj, char status){         // define o status do arquivo
    fileObj->header->status = status;
}
void setHeaderTopo(file_object* fileObj, int64_t topo){         // define o topo
    fileObj->header->topo = topo;
}
void setHeaderProxByteOffset(file_object* fileObj, int64_t proxByteOffset){         //  define o byteoffset
    fileObj->header->proxByteOffset = proxByteOffset;
}
void setHeaderNroRegArq(file_object* fileObj, int32_t nroRegArq){           // funçao para definir o numero de registros que nao foram removidos
    fileObj->header->nroRegArq = nroRegArq;
}
void setHeaderNroRegRem(file_object* fileObj, int32_t nroRegRem){           // funçao para definir o numero de registros removidos no arquivo
    fileObj->header->nroRegRem = nroRegRem;
}
void binarioNaTela(char *nomeArquivoBinario) { /* Você não precisa entender o código dessa função. */

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}

void scan_quote_string(char *str) {

	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}
