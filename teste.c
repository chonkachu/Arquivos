#include <stdint.h>
#include<stdio.h>

int main() {
    char str[50]; scanf("%s", str);
    FILE *f = fopen(str, "rb+");
    fseek(f, 1, SEEK_SET);
    int64_t t, Byteoff;
    fread(&t, 8, 1, f);
    fread(&Byteoff, 8, 1, f);
    printf("topo: %ld byteoff: %ld", t, Byteoff);
    fclose(f);
}
