#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "db.h"
#include "file_utils.h"

int create_table(char* csv_name, char* bin_name){
    FILE* csv_file = fopen(csv_name, "r");
    fseek(csv_file, 45, SEEK_SET); // desconsidera a primeira linha do csv
    file_object* fileObj = abrirArquivoBin(bin_name);
    setHeaderStatus(fileObj, 0);
    setHeaderTopo(fileObj, 0);
    setHeaderProxByteOfffset(fileObj, 0);
    setHeaderNroRegArq(fileObj, 0);
    setHeaderNroRegRem(fileObj, 0);
    writeRegistroCabecalho(fileObj);

    int cnt = 0;
    int rotation = 0;
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
            setRemovido(registro, 0);
            setTamanhoRegistro(registro, 33 + getTamNomeClube(registro)
                    + getTamNacionalidade(registro) + getTamNomeJogador(registro));
            writeRegistroDados(fileObj, registro);
            liberarRegistro(&registro);
            cnt++;
            rotation = 0;
            continue;
        }
        rotation++;
    }
    setHeaderStatus(fileObj, 1);
    setHeaderNroRegArq(fileObj, cnt);
    writeRegistroCabecalho(fileObj);
    fecharArquivoBin(&fileObj);
    fclose(csv_file);

    binarioNaTela(bin_name);
    return 1;
}

player_data** select_from(FILE *bin_file){
    return NULL;
}

player_data** select_from_where(FILE *bin_file, int id, int idade, 
                                char *nacionalidade, char *nomeClube, char *nomeJogador){
    return NULL;
}
