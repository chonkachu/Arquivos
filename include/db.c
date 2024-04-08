#include "db.h"

int create_table(FILE *csv_file, FILE *bin_file){
    data_registry *a=(data_registry*)malloc(sizeof(data_registry));
    data_registry **table=(data_registry**)malloc(sizeof(data_registry*));
    while(1){
        char str[300];
        int rotation=0;
        for(int i=0;;i++){
            char a;
            if(a=getc(csv_file)){
                if(a==',' || a=='\n'){
                    str[i]='\0';
                    break;
                }else{
                    str[i]=a;
                }
            }
        }
        if(rotation==0){
            if(str[0]=='\0'){
                a->id = -1;
            }else{
                a->id = atoi(str);
            }
        }else if(rotation==1){
            if(str[0]=='\0'){
                a->idade = -1;
            }else{
                a->idade = atoi(str);
            }
        }else if(rotation==2){
            a->tamNomeJog = strlen(str);
            if(str[0]=='\0'){
                a->nomeJogador = '$';
            }else{
                a->nomeJogador = str;
            }

        }else if(rotation==3){
            a->tamNacionalidade = strlen(str);
            if(str[0]=='\0'){
                a->nacionalidade = '$';
            }else{
                a->nacionalidade = str;
            }
        }else{
             a->tamNomeClube = strlen(str);
            if(str[0]=='\0'){
                a->nomeClube = '$';
            }else{
                a->nomeClube = str;
            }
        }

        rotation++;

        if(rotation==5){
             rotation=0;
             a->prox;
             a->removido=0;
             a->tamanhoRegistro;
            
            // maloca nova posiçao no vetor d eponteiro e mete o ponteiro pra a

            
             a=(data_registry*)malloc(sizeof(data_registry));
        }
    }
}

player_data** select_from(FILE *bin_file){

}

player_data** select_from_where(FILE *bin_file, int id, int idade, 
                                char *nacionalidade, char *nomeClube, char *nomeJogador){

}