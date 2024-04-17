#ifndef DB_H
#define DB_H
#include "file_utils.h"
typedef struct player_data_ player_data;
int create_table(char* csv_name, char* bin_name);
void select_from(char *bin_file);
void select_from_where(FILE *bin_file, int id, int idade, 
                                char *nacionalidade, char *nomeClube, char *nomeJogador);

void imprimePlayerData(player_data *player);

#endif
