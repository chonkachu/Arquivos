#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "file_utils.h"

int create_table(char* csv_name, char* bin_name){
    FILE* csv_file = fopen(csv_name, "r");
    file_object* fileObj = abrirArquivoBin(bin_name);
    setHeaderStatus(fileObj, 0);
    setHeaderTopo(fileObj, 0);
    setHeaderProxByteOfffset(fileObj, 0);
    setHeaderNroRegArq(fileObj, 0);
    setHeaderNroRegRem(fileObj, 0);
    writeRegistroCabecalho(fileObj);

    int cnt = 0;
    while(1){
        int reachedEOF=0;
        char* str=(char *)malloc(50*sizeof(char));
        int rotation=0;
        data_registry* registro = criarRegistro();
        for(int i=0;;i++){
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

        if(reachedEOF){
            free(str);
            liberarRegistro(&registro);
            break;
        }

        if(rotation==0){
            if(str[0]=='\0'){
                setId(registro, -1);
            }else{
                setId(registro, atoi(str));
            }
        }else if(rotation==1){
            if(str[0]=='\0'){
               setIdade(registro, -1);
            }else{
               setIdade(registro, atoi(str));
            }
        }else if(rotation==2){
            setTamNomeJogador(registro, strlen(str));
            if(str[0]!='\0'){
                setNomeJogador(registro, str);  
            }

        }else if(rotation==3){
            setTamNacionalidade(registro, strlen(str));
            if(str[0]!='\0'){
                setNacionalidade(registro, str);
            }
        }else{
            setTamNomeClube(registro, strlen(str));
            if(str[0]!='\0'){
                setNomeClube(registro, str);
            }
        }

        rotation++;

        if(rotation==5){
            rotation = 0;
            setProx(registro, -1);
            setRemovido(registro, 0);
            setTamanhoRegistro(registro, 33 + getTamNomeClube(registro)
                    + getTamNacionalidade(registro) + getTamNomeJogador(registro));
            writeRegistroDados(fileObj, registro);
            liberarRegistro(&registro);
            cnt++;
        }
    }
    setHeaderStatus(fileObj, 1);
    setHeaderNroRegArq(fileObj, cnt);
    writeRegistroCabecalho(fileObj);

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
