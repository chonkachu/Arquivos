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
        FILE *csv = fopen();
    }
    else if (op == 2) {
        scanf("%s", bin_name);
        FILE *bin = fopen(bin_name, "rb");

        if (bin == NULL) {
            printf("Falha ao processar o arquivo\n");
        }

        select_from(bin);
    }
    else if (op == 3) {
        scanf("%s %d", bin_name, &num_queries);
        FILE *bin = fopen(bin_name, "rb");

        if (bin == NULL) {
            printf("Falha ao processar o arquivo\n");
        }

        int num_fields;
        char field_name[20];
        int id = -1, idade = -1;
        char nomeJogador[50], nacionalidade[50], nomeClube[50];

        while(num_queries--) {
            scanf("%d", &num_fields); 
            for (int i = 0; i < num_fields; i++) {
                scanf("%s", field_name);
                if (strcmp(field_name, "id") == 0) {
                    scanf("%d", &id);
                }
                else if (strcmp(field_name, "idade") == 0) {
                    scanf("%d", &idade);
                }
                else if (strcmp(field_name, "nacionalidade") == 0) {
                    scanf("%s", nacionalidade);
                }
                else if (strcmp(field_name, "nomeJogador") == 0) {
                    scanf("%s", nomeJogador);
                }
                else if (strcmp(field_name, "nomeClube") == 0) {
                    scanf("%s", nomeClube);
                }
            }

            nacionalidade[0] = '\0';
            nomeJogador[0] = '\0';
            nomeClube[0] = '\0';
            select_from_where(bin, id, idade, nacionalidade, nomeClube, nomeJogador);
        }
    }
}
