#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "file_utils.h"

struct player_data_ {           // estrutura para auxiliar na impressão do jogador
    int id;
    int idade;
    char *nomeJogador;            // armazena o nome do jogador caso ou caso tenha
    char *nacionalidade;            // armazena a nacionalidade 
    char *nomeClube;            // armazena nome do clube
};

struct header_registry_ {
    char status;        // status do arquivo para saber se ele esta consistente
    int64_t topo;           // byteOff do ultimo registro removido
    int64_t proxByteOffset;         // primeiro byte depois do fim dos registros
    int32_t nroRegArq;          // a quantidade de registros
    int32_t nroRegRem;          // numero de registros removidos
};

struct header_index_{
    char status;        // status do arquivo para saber se ele esta consistente
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

struct data_index_ {
    int32_t id;          // id do campo
    int64_t byteOffset;  // byteoffset do campo
};

// estrutura que guarda o ponteiro para o arquivo e o registro de cabeçalho do arquivo
struct file_object_ {
    header_registry *header;
    FILE *file;
    int32_t fileIndex;
};
struct file_object_ind_ {
    header_index *header;
    FILE *file;
};

// função que inicializa a estrutura de dados file_object
file_object* criarArquivoBin(char *bin_name, char *mode){
    file_object *fileObj = (file_object*) malloc(sizeof(file_object));
    header_registry *header = (header_registry*) malloc(sizeof(header_registry));
    fileObj->header = header;
    FILE* bin = fopen(bin_name, mode);
    fileObj->file = bin;
    if(fileObj->file != NULL && (strcmp(mode, "rb")==0 || strcmp(mode, "rb+")==0)){
        inicializaHeader(fileObj);
    }
    fileObj->fileIndex = 0;
    
    return fileObj;
}

// metodo para obter id de um player
int idbuscado(player_data *player){
    return player->id;
}

// metodo para obter o byteOffset do fim do arq
int64_t tamanhoBin(file_object *bin) {
    fseek(bin->file, 0, SEEK_END);
    return ftell(bin->file);
}

// auxilia na modularizaçao para imprimir cada jogador 
player_data * criarPlayer(){
        player_data* player = (player_data*) malloc(sizeof(player_data));           
        player->nomeJogador = NULL;
        player->nacionalidade = NULL;
        player->nomeClube = NULL;
        player->id=-1;
        player->idade=-1;

        return player;
}


// compara se p2 é subconjunto de p1 ou seja se todo parametro de p2 esta em p1
int comparaPlayer(player_data *p1, player_data *p2){
    if(p2->id!=-1 && p2->id!=p1->id){
        return 0;
    }
    if(p2->idade!=-1 && p1->idade!=p2->idade){
        return 0;
    }
    if(p2->nacionalidade!=NULL){
        if(p1->nacionalidade==NULL){
            return 0;
        }
        if(strlen(p2->nacionalidade)>0 && strcmp(p2->nacionalidade, p1->nacionalidade)!=0){
            return 0;
        }
    }
    if(p2->nomeClube!=NULL){
        if(p1->nomeClube==NULL){
            return 0;
        }
        if(strlen(p2->nomeClube)>0 && strcmp(p2->nomeClube, p1->nomeClube)!=0){
            return 0;
        }
    }
    if(p2->nomeJogador!=NULL){
        if(p1->nomeJogador==NULL){
            return 0;
        }
        if(strlen(p2->nomeJogador)>0 && strcmp(p2->nomeJogador, p1->nomeJogador)!=0){
            return 0;
        }
    }
    return 1;
}

// configura o header do arquivo binario
void inicializaHeader(file_object *bin){
        fseek(bin->file, 0, SEEK_SET);
        fread(&bin->header->status, 1, 1, bin->file);
        fread(&bin->header->topo, 8, 1, bin->file);
        fread(&bin->header->proxByteOffset, 8, 1, bin->file);
        fread(&bin->header->nroRegArq, 4, 1, bin->file);
        fread(&bin->header->nroRegRem, 4, 1, bin->file);
}

void imprimePlayerData(player_data *player){            // função que imprime a nacionalidade, nome e clube do jogador
    printf("Nome do Jogador: ");
    if(player->nomeJogador == NULL || player->nomeJogador[0] == '\0'){
        printf("SEM DADO\n");
    }else{ 
        printf("%s\n", player->nomeJogador);
    }

    printf("Nacionalidade do Jogador: ");
    if(player->nacionalidade == NULL || player->nacionalidade[0] == '\0'){
        printf("SEM DADO\n");
    }else{
        printf("%s\n", player->nacionalidade);
    }

    printf("Clube do Jogador: ");
    if(player->nomeClube == NULL || player->nomeClube[0] == '\0'){
        printf("SEM DADO\n\n");
    }else{
        printf("%s\n\n", player->nomeClube);
    }
}

// checagem basica se o arquivo esta ok
int verificaConsistencia(file_object * bin){
   if(bin->file==NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    if(bin->header->status=='0'){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    return 1;
}

// vai para onde se inicio o registro de dados
void inicioRegistroDeDados(file_object * bin){
    fseek(bin->file, 25, SEEK_SET);
}

// vai para o fim do registro de dados
void fimRegistroDeDados(file_object *bin) {
    fseek(bin->file, 0, SEEK_END);
}

int64_t getTopo(file_object *bin){
    return bin->header->topo;
}

int64_t getFim(file_object *bin) {
    return bin->header->proxByteOffset;
}

int getNroRegArq(file_object * bin){
    return bin->header->nroRegArq;
}

int getNroRegRem(file_object * bin){
    return bin->header->nroRegRem;
}

void liberaPlayer(player_data * player){
     if (player->nomeJogador != NULL)
            free(player->nomeJogador);
        if (player->nacionalidade != NULL)
            free(player->nacionalidade);
        if (player->nomeClube != NULL)
            free(player->nomeClube);
        player->nomeJogador = NULL;
        player->nacionalidade = NULL;
        player->nomeClube = NULL;
}

FILE * getFile(file_object * bin){
    return bin->file;
}

// processaremos o registro e guardaremos o registro
int processaRegistro(file_object *bin, data_registry *data) {

    char a = getc(bin->file);

    if (a == EOF) return -1;

    data->removido = a;
    fread(&data->tamanhoRegistro, 4, 1, bin->file);
    fread(&data->prox, 8, 1, bin->file);
    if (a == '1') return 0;
    fread(&data->id, 4, 1, bin->file);
    fread(&data->idade, 4, 1, bin->file);

    int tamNacionalidade = 0, tamNomeJog = 0, tamNomeClube = 0;        // armazenaram respectivamente tamanho da string de nacionalidade, nome do jogador e nome do clube
    fread(&tamNomeJog, 4, 1, bin->file);
    if (tamNomeJog != 0) {          // aloca 
        data->nomeJogador = (char*) malloc((tamNomeJog+1)*sizeof(char));
        fread(data->nomeJogador, 1, tamNomeJog, bin->file);
        data->nomeJogador[tamNomeJog] = '\0';
    }

    fread(&tamNacionalidade, 4, 1, bin->file);
    if (tamNacionalidade != 0){
        data->nacionalidade = (char*) malloc((tamNacionalidade+1)*sizeof(char));
        fread(data->nacionalidade, 1, tamNacionalidade, bin->file);
        data->nacionalidade[tamNacionalidade] = '\0';

    }

    fread(&tamNomeClube, 4, 1, bin->file);
    if (tamNomeClube != 0) {
        data->nomeClube = (char*) malloc((tamNomeClube+1)*sizeof(char));
        fread(data->nomeClube, 1, tamNomeClube, bin->file);
        data->nomeClube[tamNomeClube] = '\0';
    }
    return 1;
}

// processa-se um registro e configureramos um player a partir disso
int processaRegistroPlayer(file_object * bin, player_data *player){
    char a = getc(bin->file);        // necessario para verificar se chegamos em EOF    
        if (a == EOF)
            return -1;
            
        if(a=='1'){
            int tamReg=0;        // o registro esta logicamente removido portanto vamos pular o registro inteiro
            fread(&tamReg, 4, 1, bin->file);
            fseek(bin->file, tamReg-5, SEEK_CUR);
           return 0;
        }
        fseek(bin->file, 12, SEEK_CUR);        // pulamos direto para os campos de tamanho variavel pois sao os que serão impressos
        fread(&player->id, 4, 1, bin->file);
        fread(&player->idade, 4, 1, bin->file);
        int tamNacionalidade = 0, tamNomeJog = 0, tamNomeClube = 0;        // armazenaram respectivamente tamanho da string de nacionalidade, nome do jogador e nome do clube
        fread(&tamNomeJog, 4, 1, bin->file);
        
        if (tamNomeJog != 0) {          // aloca 
            player->nomeJogador = (char*) malloc((tamNomeJog+1)*sizeof(char));
            fread(player->nomeJogador, 1, tamNomeJog, bin->file);
            player->nomeJogador[tamNomeJog] = '\0';
        }
            
        fread(&tamNacionalidade, 4, 1, bin->file);
        
        if (tamNacionalidade != 0){
            player->nacionalidade = (char*) malloc((tamNacionalidade+1)*sizeof(char));
            fread(player->nacionalidade, 1, tamNacionalidade, bin->file);
            player->nacionalidade[tamNacionalidade] = '\0';

        }
        fread(&tamNomeClube, 4, 1, bin->file);
        
        if (tamNomeClube != 0) {
            player->nomeClube = (char*) malloc((tamNomeClube+1)*sizeof(char));
            fread(player->nomeClube, 1, tamNomeClube, bin->file);
            player->nomeClube[tamNomeClube] = '\0';
        }
        return 1;
}

// processa-se um registro removido
data_registry* processaRegistroRemovido(file_object *bin, int64_t byteOff) {
    data_registry *registro = criarRegistro();
    processaRegistro(bin, registro);
    gotoByteOffArquivoBin(bin, byteOff); // volta pro inicio do registro
    return registro;
}

// a partir da entrada geraremos um player
player_data * lerPlayerData(int op){
    char field_name[50];
    int num_fields;

    int id = -1;
    int idade = -1;
    char nacionalidade[50];
    nacionalidade[0]='\0';
    char clube[50];
    clube[0]='\0';
    char nome[50];
    nome[0]='\0';
    player_data *player = criarPlayer();
    if (op == INSERT) { // operação de insert

        scanf("%d", &id);
        char temp_idade[50];
        scan_quote_string(temp_idade);
        if(temp_idade[0] != '\0')
            idade = atoi(temp_idade);
        scan_quote_string(nome);
        scan_quote_string(nacionalidade);
        scan_quote_string(clube);
    }
    else {
        scanf("%d", &num_fields); 
        for (int j = 0; j < num_fields; j++) {
            scanf("%s", field_name);
            if (strcmp(field_name, "id") == 0) {
                scanf("%d", &id);
            }
            else if (strcmp(field_name, "idade") == 0) {
                scanf("%d", &idade);
            }
            else if (strcmp(field_name, "nacionalidade") == 0) {
                scan_quote_string(nacionalidade);
            }
            else if (strcmp(field_name, "nomeJogador") == 0) {
                scan_quote_string(nome);
            }
            else if (strcmp(field_name, "nomeClube") == 0) {
                scan_quote_string(clube);
            }
        }
    }
    player->id = id;
    player->idade = idade;

    player->nomeJogador = strdup(nome);
    player->nomeClube = strdup(clube);
    player->nacionalidade = strdup(nacionalidade);
    return player;
}

// cria arquivo binario de indice
file_object_ind* criarArquivoBinInd(char *bin_name){
    file_object_ind *fileObj = (file_object_ind*) malloc(sizeof(file_object_ind));
    header_index *header = (header_index*) malloc(sizeof(header_index));
    FILE* bin = fopen(bin_name, "wb+");
    fileObj->header = header;
    fileObj->file = bin;
    
    return fileObj;
}

// um vetor com registros de indice
data_index** criarVetorIndice(int n){
    data_index **arr = (data_index**)malloc(sizeof(data_index*)*n);
    for(int i=0;i<n;i++){
        arr[i]=(data_index*)malloc(sizeof(data_index));
        arr[i]->id=0x7fffff;
        arr[i]->byteOffset=-1;
    }
    return arr;
}

data_index* criarDataIndex(int id, int64_t byteOff) {
    data_index * data = malloc(sizeof(data_index));
    data->id = id;
    data->byteOffset = byteOff;

    return data;
}
int32_t getIndiceId(data_index* a){
    return a->id;
}
int64_t getByteOff(data_index* a){
    return a->byteOffset;
}
void setIndiceId(data_index* a, int32_t id){
    a->id = id;
}
void setIndiceByteOff(data_index* a, int64_t byteOff){
    a->byteOffset = byteOff;
}
// utilizado para a ordenaçao dos vetores de indice
int comparaIndice(const void *a, const void *b) {
    const data_index *da = *(const data_index **)a;
    const data_index *db = *(const data_index **)b;
    
    if (da->id < db->id) return -1;
    if (da->id > db->id) return 1;
    return 0;
}

void setHeaderStatusInd(file_object_ind* fileObj, char status){
    fileObj->header->status = status;
}
void writeRegistroCabecalhoInd(file_object_ind* fileObj){
    fseek(fileObj->file, 0, SEEK_SET);
    fwrite(&fileObj->header->status, 1, 1, fileObj->file);
    fseek(fileObj->file, 0, SEEK_END);
}
void writeRegistroDadosInd(file_object_ind* fileObj, data_index** arr, int i){
    for(int k=0;k<i;k++){
        fwrite(&arr[k]->id, 4, 1, fileObj->file);
        fwrite(&arr[k]->byteOffset, 8, 1, fileObj->file);
    }
}
void fecharArquivoBinInd(file_object_ind** fileObj){
    fclose((*fileObj)->file);
    free((*fileObj)->header);
    free(*fileObj);
    *fileObj = NULL;
}

// busca binaria no arquivo de indice
int64_t indBB(int id, char *bin_name, int nroRegArq){
    int ini=1;
    int fim=nroRegArq;

    FILE *bin = fopen(bin_name, "rb");

    char stats;
        fread(&stats, 1, 1, bin);

        if (stats == '0')
        {
            printf("Falha no processamento do arquivo.\n");
            return -2;
        }

    while(ini<=fim){
        int meio=(ini+fim)/2;

        fseek(bin, 12*(meio-1)+1, SEEK_SET);  // como so temos id e byteOff 4+8=12 bytes o tamnho de cada registro aqui
                                             //sendo assim podemos ir para o inicio de cada registro com 12*(rrn-1)+1
                                            //onde +1 surge por conta do status de consistencia
        int idfound;
        fread(&idfound, 4, 1, bin);
        if(idfound==id){
            int64_t byte;
            fread(&byte, 8, 1, bin);
            fclose(bin);
            return byte;
        }else if(idfound<id){
            ini=meio+1;
        }else{
            fim=meio-1;
        }
    }

    fclose(bin);
    return -1;
    
}
// função que inicializa os registros de dados
data_registry* criarRegistro() {
    data_registry *registro = (data_registry*) malloc(sizeof(data_registry));
    registro->nacionalidade = NULL;
    registro->nomeClube = NULL;
    registro->nomeJogador = NULL;

    return registro;
}

//cria um registro a partir de um player importante na insert_into
data_registry* criarRegistroFromPlayer(player_data *player) {
    data_registry *registro = criarRegistro();
    registro->removido = '0';
    registro->tamanhoRegistro = 33 + strlen(player->nacionalidade)
                                   + strlen(player->nomeJogador)
                                   + strlen(player->nomeClube);
    registro->prox = -1;
    registro->id = player->id;
    registro->idade = player->idade;

    registro->tamNomeJog = strlen(player->nomeJogador);
    if (registro->tamNomeJog)
        registro->nomeJogador = strdup(player->nomeJogador);

    registro->tamNacionalidade = strlen(player->nacionalidade);
    if (registro->tamNacionalidade)
        registro->nacionalidade = strdup(player->nacionalidade);

    registro->tamNomeClube = strlen(player->nomeClube);
    if (registro->tamNomeClube)
        registro->nomeClube = strdup(player->nomeClube);

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
void gotoByteOffArquivoBin(file_object *bin, int64_t byteOff) {
    fseek(bin->file, byteOff, SEEK_SET);
}

// função que fecha o arquivo e libera memoria do registro de cabeçalho
void fecharArquivoBin(file_object** fileObj) {
    fclose((*fileObj)->file);
    free((*fileObj)->header);
    free(*fileObj);
    *fileObj = NULL;
}
int getIdRegistro(data_registry *registro) {
    return registro->id;
}
int getTamRegistro(data_registry *registro) {
    return registro->tamanhoRegistro;
}
int64_t getProx(data_registry *registro) {
    return registro->prox;
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
