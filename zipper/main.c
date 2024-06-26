/*
    Thales Sena de Queiroz - 14608873
    Christian Bernard Simas Corrêa Gioia Ribeiro - 11795572
*/

#include <stdio.h>
#include <string.h>
#include "db.h"
#include "file_utils.h"

int main() {

    int op;         // numero da operação a ser realizada                                  
    int num_queries;            // quantidade de buscas que serao realizadas na operaçao 3
    scanf("%d", &op);
    char csv_name[50], bin_name[50];            // nome do arquivos csv e do arquivo binario

    if (op == 1) {
        scanf(" %s %s", csv_name, bin_name);
        create_table(csv_name, bin_name);            
        
    }
    else if (op == 2) {
        scanf("%s", bin_name);

        select_from(bin_name);            
    }
    else if (op == 3) {
        scanf("%s %d", bin_name, &num_queries);
        
        select_from_where(bin_name, num_queries);            
        
    }else if (op == 4) {
         scanf(" %s %s", csv_name, bin_name);
        create_index(csv_name, bin_name);            
        
    }else if (op == 5) {            
        scanf(" %s %s %d", csv_name, bin_name, &num_queries);
        delete_from_where(csv_name, bin_name, num_queries);
    }else if (op == 6) {    
        scanf(" %s %s %d", csv_name, bin_name, &num_queries);
        insert_into(csv_name, bin_name, num_queries);
    }else if (op == 7) {    
        scanf(" %s %s", csv_name, bin_name);
        create_index_btree(csv_name, bin_name); 
    }else if (op == 8) {    
        scanf(" %s %s %d", csv_name, bin_name, &num_queries);
        select_from_id(csv_name, bin_name, num_queries);
    }else if (op == 9) {    
        scanf(" %s %s %d", csv_name, bin_name, &num_queries);
        select_from_where_btree(csv_name, bin_name, num_queries);
    }else if (op == 10) {    
        scanf(" %s %s %d", csv_name, bin_name, &num_queries);
        insert_into_btree(csv_name, bin_name, num_queries);
    }
    
    

    return 0;
}
