#include <stdio.h>
#include <string.h>
#include "file_utils.h"
#include "db.h"

int main() {

    int op;
    int num_queries;
    scanf("%d", &op);
    char csv_name[50], bin_name[50];

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
        
    }

    return 0;
}
