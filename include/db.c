#include "db.h"

int create_table(FILE *csv_file, FILE *bin_file){
    data_registry *a=(data_registry*)malloc(sizeof(data_registry));

    data_registry **table=(data_registry**)malloc(sizeof(data_registry*));

    while(1){
        char str[1024];
        int rotation=0;
        for(int i=0;;i++){
            char a;
            if(a=getc(csv_file)){
                if(a==EOF){
                    return 1; //nao era pra retornar table
                }
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
                setId(a, -1) = -1;
            }else{
                setId(a, atoi(str));
            }
        }else if(rotation==1){
            if(str[0]=='\0'){
               setIdade(a, -1);
            }else{
               setIdade(a, atoi(str));
            }
        }else if(rotation==2){
            setTamNomeJogador(a, strlen(str));
            if(str[0]!='\0'){
                setNomeJogador(a, str);  
            }

        }else if(rotation==3){
            setTamNacionalidade(a, strlen(str)) = strlen(str);
            if(str[0]!='\0'){
                setNacionalidade(a, str);
            }
        }else{
             setTamNomeClube(a, strlen(str));
            if(str[0]!='\0'){
                setNomeClube(a, str);
            }
        }

        rotation++;

        if(rotation==5){
             rotation = 0;
             setProx(a, -1);
             setRemovido(a, 0);
             setTamanhoRegistro(a, 33 + getTamNomeClube(a) + getTamNacionalidade(a) + getTamNomeJogador(a));
            
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