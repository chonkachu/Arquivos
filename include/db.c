#include "db.h"

int create_table(FILE *csv_file, FILE *bin_file){
    while(1){
        char str[300];
        int rotation=0;
        for(int i=0;;i++){
            char a;
            if(a=getc(csv_file)){
                if(a==','){
                    str[i]='\0';
                    break;
                }else{
                    str[i]=a;
                }
            }
        }
        if(rotation==0){
            if(str[0]!='\0'){
                int x=-1;
            }
           int x = atoi(str);
        }else if(rotation==1){
           int x = atoi(str);
        }else if(rotation==2){

        }else if(rotation==3){

        }else{

        }
        rotation++;
        if(rotation==5) rotation=0;
    }
}

player_data** select_from(FILE *bin_file){

}

player_data** select_from_where(FILE *bin_file, int id, int idade, 
                                char *nacionalidade, char *nomeClube, char *nomeJogador){

}