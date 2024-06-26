#ifndef DB_H
#define DB_H
#include "file_utils.h"

// definindo as funcionalidades do banco de dados

int create_table(char* csv_name, char* bin_name);
void create_index(char* bin_name, char* index_bin_name);
void select_from(char *bin_file);
void select_from_where(char *bin_name, int num_queries);
void delete_from_where(char* bin_name, char* index_bin_name, int n);
void insert_into(char* bin_name, char* index_bin_name, int n);
void create_index_btree(char * bin_name, char * index_bin_name);
void select_from_id(char * bin_name, char * index_bin_name, int num_queries);
void select_from_where_btree(char * bin_name, char * index_bin_name, int num_queries);
void insert_into_btree(char * bin_name, char * index_bin_name, int n);

#endif
