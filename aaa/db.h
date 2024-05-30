#ifndef DB_H
#define DB_H
#include "file_utils.h"
typedef struct player_data_ player_data;
typedef struct parametrosDeBusca_ parametrosDeBusca;
int create_table(char* csv_name, char* bin_name);
void select_from(char *bin_file);
void select_from_where(char *bin_name, int num_queries);

void imprimePlayerData(player_data *player);

#endif