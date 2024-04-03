#ifndef DB_H
#define DB_H
#include "file_utils.h"
typedef struct player_data_ player_data;
int create_table(FILE *csv_file, FILE *bin_file);
player_data** select_from(FILE *bin_file);
player_data** select_from_where(FILE *bin_file, int id, int idade, 
                                char *nacionalidade, char *nomeClube, char *nomeJogador);
struct player_data_ {
    int id;
    int idade;
    char *nomeJogador;
    char *nacionalidade;
    char *nomeClube;
};

#endif
