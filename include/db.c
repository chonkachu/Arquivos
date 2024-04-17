#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "db.h"
#include "file_utils.h"

struct player_data_ {
    char *nomeJogador;
    char *nacionalidade;
    char *nomeClube;
};

int create_table(char* csv_name, char* bin_name){
    FILE* csv_file = fopen(csv_name, "r");
    fseek(csv_file, 45, SEEK_SET); // desconsidera a primeira linha do csv
    file_object* fileObj = criarArquivoBin(bin_name);
    setHeaderStatus(fileObj, '0');
    setHeaderTopo(fileObj, -1);
    setHeaderProxByteOffset(fileObj, 0);
    setHeaderNroRegArq(fileObj, 0);
    setHeaderNroRegRem(fileObj, 0);
    writeRegistroCabecalho(fileObj);

    int cnt = 0;
    int rotation = 0;
    int byteOff = 25;
    data_registry* registro;
    while(1){
        int reachedEOF = 0;
        char* str = (char *) malloc(50*sizeof(char));
        if (rotation == 0)
            registro = criarRegistro();
        for(int i = 0;;i++){
            char a = getc(csv_file);
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

        int len = strlen(str);

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
                //printf("%s\n", str);
                setId(registro, atoi(str));
                free(str);
            }
        }
        else if(rotation==1){
            if(str[0]=='\0'){
                setIdade(registro, -1);
            }
            else{
                //printf("%s\n", str);
                setIdade(registro, atoi(str));
                free(str);
            }
        }
        else if(rotation==2){
            setTamNomeJogador(registro, len);
            if(str[0]!='\0'){
                //printf("%s\n", str);
                setNomeJogador(registro, str);  
            }
            else {
                free(str);
            }
        }
        else if(rotation==3){
            setTamNacionalidade(registro, len);
            if(str[0]!='\0'){
                //printf("%s\n", str);
                setNacionalidade(registro, str);
            }
            else {
                free(str);
            }
        }
        else{
            setTamNomeClube(registro, len);
            if(str[0]!='\0'){
                //printf("%s\n", str);
                setNomeClube(registro, str);
            }
            else {
                free(str);
            }
            setProx(registro, -1);
            setRemovido(registro, '0');
	    int tamReg = 33 + getTamNomeClube(registro)
                    + getTamNacionalidade(registro) + getTamNomeJogador(registro);
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

void select_from(char* bin_name){
    FILE *bin = fopen(bin_name, "rb");

    if(bin==NULL){
        return;
    }

    fseek(bin, 25, SEEK_SET);

    player_data* player = (player_data*) malloc(sizeof(player_data));
        player->nomeJogador = NULL;
        player->nacionalidade = NULL;
        player->nomeClube = NULL;
    while(1){
        char a = getc(bin);
        if (a == EOF)
            break;
        fseek(bin, 20, SEEK_CUR);
        int tamNacionalidade = 0, tamNomeJog = 0, tamNomeClube = 0;
        fread(&tamNomeJog, 4, 1, bin);
        //printf("%d\n", tamNomeJog);
        if (tamNomeJog != 0) {
            player->nomeJogador = (char*) malloc((tamNomeJog+1)*sizeof(char));
            fread(player->nomeJogador, 1, tamNomeJog, bin);
            player->nomeJogador[tamNomeJog] = '\0';
        }
            
        fread(&tamNacionalidade, 4, 1, bin);
        //printf("%d\n", tamNacionalidade);
        if (tamNacionalidade != 0){
            player->nacionalidade = (char*) malloc((tamNacionalidade+1)*sizeof(char));
            fread(player->nacionalidade, 1, tamNacionalidade, bin);
            player->nacionalidade[tamNacionalidade] = '\0';

        }
        fread(&tamNomeClube, 4, 1, bin);
        //printf("%d\n", tamNomeClube);
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

void select_from_where(FILE *bin_file, int id, int idade, 
                                char *nacionalidade, char *nomeClube, char *nomeJogador){
    return;
}

void imprimePlayerData(player_data *player){
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
