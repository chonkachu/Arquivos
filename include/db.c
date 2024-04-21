#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "db.h"
#include "file_utils.h"

struct player_data_ {           // estrutura para auxiliar na impressão do jogador
    char *nomeJogador;            // armazena o nome do jogador caso ou caso tenha
    char *nacionalidade;            // armazena a nacionalidade 
    char *nomeClube;            // armazena nome do clube
};

struct parametrosDeBusca_{            // estrututura que armazenos os parametros buscado em cada busca
    int id;            // armazena o id buscado
    int idade;            // armazena a idade buscada
    char nacionalidade[50];            // armazena a nacionalidade buscada
    char nome[50];            // armazena o nome buscado
    char clube[50];            // armazena o nome 
};

int create_table(char* csv_name, char* bin_name){            // funçao que transforma o arquivo csv em binario (operação 1)
    FILE* csv_file = fopen(csv_name, "r");            // abre o arquivo csv
    fseek(csv_file, 45, SEEK_SET);            // desconsidera a primeira linha do csv
    file_object* fileObj = criarArquivoBin(bin_name);            // cria o arquivo bin
    setHeaderStatus(fileObj, '0');
    setHeaderTopo(fileObj, -1);
    setHeaderProxByteOffset(fileObj, 0);
    setHeaderNroRegArq(fileObj, 0);
    setHeaderNroRegRem(fileObj, 0);
    writeRegistroCabecalho(fileObj);

    int cnt = 0;            // conta a quantidade de registros
    int rotation = 0;            // variavel auxiliar para sabermos qual campo estamos lendo agora ou se o jogar foi completamente lido
    int byteOff = 25;   
    data_registry* registro;            // montamos registro para escrever certinho no arquivo binario
    while(1){
        int reachedEOF = 0;            // variavel para saber se chegamos em EOF
        char* str = (char *) malloc(50*sizeof(char));            //string para ler o campo  
        if (rotation == 0)
            registro = criarRegistro();
        for(int i = 0;;i++){
            char a = getc(csv_file);            // lemos caracter por caracter
            if (a == EOF){
                reachedEOF=1;
                break;
            }
            if(a==',' || a=='\n'){
                str[i]='\0';
                break;
            }
            else{
                str[i]=a;
            }
        }

        int len = strlen(str);            //tamanho do campo para utilizar nos campos de tamanho variavel

        if(reachedEOF){
            free(str);
            liberarRegistro(&registro);
            break;
        }

        if(rotation==0){
            if(str[0]=='\0'){
                setId(registro, -1);
            }
            else{
                setId(registro, atoi(str));
                free(str);
            }
        }
        else if(rotation==1){
            if(str[0]=='\0'){
                setIdade(registro, -1);
            }
            else{
                setIdade(registro, atoi(str));
                free(str);
            }
        }
        else if(rotation==2){
            setTamNomeJogador(registro, len);
            if(str[0]!='\0'){
                setNomeJogador(registro, str);  
            }
            else {
                free(str);
            }
        }
        else if(rotation==3){
            setTamNacionalidade(registro, len);
            if(str[0]!='\0'){
                setNacionalidade(registro, str);
            }
            else {
                free(str);
            }
        }
        else{
            setTamNomeClube(registro, len);
            if(str[0]!='\0'){
                setNomeClube(registro, str);
            }
            else {
                free(str);
            }
            setProx(registro, -1);
            setRemovido(registro, '0');
        // variavel que armazena o tamanho do registro 33 fio + os campos de tamanho variavel
	    int tamReg = 33 + getTamNomeClube(registro) + getTamNacionalidade(registro) + getTamNomeJogador(registro);
	    byteOff += tamReg;
            setTamanhoRegistro(registro, tamReg);
            writeRegistroDados(fileObj, registro);
            liberarRegistro(&registro);
            cnt++;
            rotation = 0;
            continue;
        }
        rotation++;
    }
    setHeaderStatus(fileObj, '1');
    setHeaderProxByteOffset(fileObj, byteOff);
    setHeaderNroRegArq(fileObj, cnt);
    writeRegistroCabecalho(fileObj);
    fecharArquivoBin(&fileObj);
    fclose(csv_file);

    binarioNaTela(bin_name);
    return 1;
}

void select_from(char* bin_name){            // função que imprime os registros do arquivo binario na forma pedida (operação 2)
    FILE *bin = fopen(bin_name, "rb");             // abre o arquivo binario

    if(bin==NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    fseek(bin, 25, SEEK_SET);            

    player_data* player = (player_data*) malloc(sizeof(player_data));            // auxilia na modularizaçao para imprimir cada jogador 
        player->nomeJogador = NULL;
        player->nacionalidade = NULL;
        player->nomeClube = NULL;
    while(1){
        char a = getc(bin);        // necessario para verificar se chegamos em EOF    
        if (a == EOF)
            break;
            
        if(a=='1'){
            int tamReg=0;        // o registro esta logicamente removido portanto vamos pular o registro inteiro
            fread(&tamReg, 4, 1, bin);
            fseek(bin, tamReg-5, SEEK_CUR);
            continue;
        }
        fseek(bin, 20, SEEK_CUR);        // pulamos direto para os campos de tamanho variavel pois sao os que serão impressos
        int tamNacionalidade = 0, tamNomeJog = 0, tamNomeClube = 0;        // armazenaram respectivamente tamanho da string de nacionalidade, nome do jogador e nome do clube
        fread(&tamNomeJog, 4, 1, bin);
        
        if (tamNomeJog != 0) {
            player->nomeJogador = (char*) malloc((tamNomeJog+1)*sizeof(char));
            fread(player->nomeJogador, 1, tamNomeJog, bin);
            player->nomeJogador[tamNomeJog] = '\0';
        }
            
        fread(&tamNacionalidade, 4, 1, bin);
        
        if (tamNacionalidade != 0){
            player->nacionalidade = (char*) malloc((tamNacionalidade+1)*sizeof(char));
            fread(player->nacionalidade, 1, tamNacionalidade, bin);
            player->nacionalidade[tamNacionalidade] = '\0';

        }
        fread(&tamNomeClube, 4, 1, bin);
        
        if (tamNomeClube != 0) {
            player->nomeClube = (char*) malloc((tamNomeClube+1)*sizeof(char));
            fread(player->nomeClube, 1, tamNomeClube, bin);
            player->nomeClube[tamNomeClube] = '\0';

        }
        imprimePlayerData(player);
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
    free(player);
    fclose(bin);
}

void select_from_where(char *bin_name, int num_queries){        // função que imprime os registros especificos de cada busca do arquivo binario na forma pedida (operação 3)
        parametrosDeBusca parametros[1024];        // vetor que ira armazenar as especificaçoes de cada busca para lermos primeiro a entrada e somente depois imprimir
        int num_fields;        // quantidade de campos que sao requisitados na busca
        char field_name[20];        // para ler o nome do campo

        // int it=num_queries;        // auxiliar que recebe a quantidade de buscas
        for(int i=0;i<num_queries;i++){
            parametros[i].id=-1;
            parametros[i].idade=-1;
            parametros[i].nacionalidade[0]='\0';
            parametros[i].clube[0]='\0';
            parametros[i].nome[0]='\0';

            scanf("%d", &num_fields); 
            for (int j = 0; j < num_fields; j++) {
                scanf("%s", field_name);
                if (strcmp(field_name, "id") == 0) {
                    scanf("%d", &parametros[i].id);
                }
                else if (strcmp(field_name, "idade") == 0) {
                    scanf("%d", &parametros[i].idade);
                }
                else if (strcmp(field_name, "nacionalidade") == 0) {
                    scan_quote_string(parametros[i].nacionalidade);
                }
                else if (strcmp(field_name, "nomeJogador") == 0) {
                    scan_quote_string(parametros[i].nome);
                }
                else if (strcmp(field_name, "nomeClube") == 0) {
                     scan_quote_string(parametros[i].clube);
                }
            }
        }           // Fim da computação da entrada

        // inicio da operação

        for(int i=0;i<num_queries;i++){
            int EXIST=0;
            FILE *bin = fopen(bin_name, "rb");

            if(bin==NULL){
                printf("Falha no processamento do arquivo.\n");
                return;
            }

            printf("Busca %d\n\n", i+1);

            int idBuscado=parametros[i].id;         // o id buscado
            int idadeBuscada=parametros[i].idade;       // a idade buscada
            char* nacionalidadeBuscada=parametros[i].nacionalidade;         // a nacionalidade
            char* nomeBuscado=parametros[i].nome;         // nome
            char* clubeBuscado=parametros[i].clube;         // clube

            fseek(bin, 25, SEEK_SET);

            player_data* player = (player_data*) malloc(sizeof(player_data)); 
                player->nomeJogador = NULL;
                player->nacionalidade = NULL;
                player->nomeClube = NULL;
                
            while(1){
                char a = getc(bin);
                if (a == EOF)
                    break;
                if(a=='1'){
                    int tamReg=0;
                    fread(&tamReg, 4, 1, bin);
                    fseek(bin, tamReg-5, SEEK_CUR);
                    continue;
                }

                fseek(bin, 12, SEEK_CUR); // skippa tamreg e prox

                int id=-1;          // id lido do registro atual
                int idade=-1;          // idade lida do registro atual

                fread(&id, 4, 1, bin);
                fread(&idade, 4, 1, bin);

                int tamNacionalidade = 0, tamNomeJog = 0, tamNomeClube = 0;         // armazenaram respectivamente tamanho da string de nacionalidade, nome do jogador e nome do clube do registro atual
                fread(&tamNomeJog, 4, 1, bin);
                
                if (tamNomeJog != 0) {
                    player->nomeJogador = (char*) malloc((tamNomeJog+1)*sizeof(char));
                    fread(player->nomeJogador, 1, tamNomeJog, bin);
                    player->nomeJogador[tamNomeJog] = '\0';
                }

                fread(&tamNacionalidade, 4, 1, bin);
                if (tamNacionalidade != 0){
                    player->nacionalidade = (char*) malloc((tamNacionalidade+1)*sizeof(char));
                    fread(player->nacionalidade, 1, tamNacionalidade, bin);
                    player->nacionalidade[tamNacionalidade] = '\0';

                }
                fread(&tamNomeClube, 4, 1, bin);
                if (tamNomeClube != 0) {
                    player->nomeClube = (char*) malloc((tamNomeClube+1)*sizeof(char));
                    fread(player->nomeClube, 1, tamNomeClube, bin);
                    player->nomeClube[tamNomeClube] = '\0';

                }

                
                int contadorDeFit=0;            // contador para saber se o registro possui todos os campos que estao sendo procurados
                int neededFit=0;            // a quantidade de campos necessarias;
                if(idadeBuscada!=-1){
                  //  printf("idade buscada eh %d\n", idadeBuscada);
                    if(idadeBuscada==idade){
                        contadorDeFit++;
                    }
                    neededFit++;
                }
                if(idBuscado!=-1){
                  //  printf("id buscado eh %d\n", idBuscada);
                    if(idBuscado==id){
                        contadorDeFit++;
                    }
                    neededFit++;
                }
                if(strlen(nacionalidadeBuscada)>0){
                   // printf("nacionalidade buscada eh %s\n", nacionalidadeBuscada);
                    if(strcmp(nacionalidadeBuscada, player->nacionalidade)==0){
                        contadorDeFit++;
                    }
                    neededFit++;
                }
                if(strlen(nomeBuscado)>0){
                  //  printf("nacionalidade buscada eh %s\n", nacionalidadeBuscada);
                    if(strcmp(nomeBuscado, player->nomeJogador)==0){
                        contadorDeFit++;
                    }
                    neededFit++;
                }
                if(strlen(clubeBuscado)>0){
                    if(strcmp(clubeBuscado, player->nomeClube)==0){
                        contadorDeFit++;
                    }
                    neededFit++;
                }


                if(neededFit==contadorDeFit){
                    EXIST=1;
                    imprimePlayerData(player);
                    if(idBuscado!=-1){          // como so há 1 id para cada se o jogador com o id foi encontrado fim-se
                        if (player->nomeJogador != NULL)
                         free(player->nomeJogador);
                        if (player->nacionalidade != NULL)
                         free(player->nacionalidade);
                            if (player->nomeClube != NULL)
                                free(player->nomeClube);
                            player->nomeJogador = NULL;
                            player->nacionalidade = NULL;
                            player->nomeClube = NULL;
                            break;
                    }
                }
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
            if(!EXIST){
                printf("Registro inexistente.\n\n");
            }
            free(player);
            fclose(bin);
        }
}

void imprimePlayerData(player_data *player){            // função que imprime a nacionalidade, nome e clube do jogador
    printf("Nome do Jogador: ");
    if(player->nomeJogador == NULL){
        printf("SEM DADO\n");
    }else{ 
        printf("%s\n", player->nomeJogador);
    }

    printf("Nacionalidade do Jogador: ");
    if(player->nacionalidade == NULL){
        printf("SEM DADO\n");
    }else{
        printf("%s\n", player->nacionalidade);
    }

    printf("Clube do Jogador: ");
    if(player->nomeClube == NULL){
        printf("SEM DADO\n\n");
    }else{
        printf("%s\n\n", player->nomeClube);
    }

}
