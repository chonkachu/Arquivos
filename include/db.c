#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "db.h"
#include "file_utils.h"

struct parametrosDeBusca_{            // estrututura que armazenos os parametros buscado em cada busca
    int id;            // armazena o id buscado
    int idade;            // armazena a idade buscada
    char nacionalidade[50];            // armazena a nacionalidade buscada
    char nome[50];            // armazena o nome buscado
    char clube[50];            // armazena o nome 
};

typedef struct pair_ {
    int32_t tamReg;
    int64_t byteOff;
} pair;

int comparaPair(const void* a, const void* b) {
    pair aa = *(pair*) a;
    pair bb = *(pair*) b;

    if (aa.tamReg > bb.tamReg)  return 1; 
    if (aa.tamReg < bb.tamReg) return -1;
    return 0;
}
int create_table(char* csv_name, char* bin_name){            // funçao que transforma o arquivo csv em binario (operação 1)
    FILE* csv_file = fopen(csv_name, "r");            // abre o arquivo csv
    for (char c = getc(csv_file); c != '\n'; c = getc(csv_file));
    file_object* fileObj = criarArquivoBin(bin_name, "wb+");            // cria o arquivo bin
    setHeaderStatus(fileObj, '0');
    setHeaderTopo(fileObj, -1);
    setHeaderProxByteOffset(fileObj, 0);
    setHeaderNroRegArq(fileObj, 0);
    setHeaderNroRegRem(fileObj, 0);
    writeRegistroCabecalho(fileObj);

    int cnt = 0;            // conta a quantidade de registros
    int rotation = 0;            // variavel auxiliar para sabermos qual campo estamos lendo agora ou se o jogar foi completamente lido
    int byteOff = 25;           // ponto inicial dos dados apos o cabeçalho
    data_registry* registro;            // montamos registro para escrever certinho no arquivo binario
    while(1){
        int reachedEOF = 0;            // variavel para saber se chegamos em EOF
        char* str = (char *) malloc(50*sizeof(char));             //string para ler o campo  
        if (rotation == 0)
            registro = criarRegistro();
        for(int i = 0;;i++){
            char a = getc(csv_file);            // lemos caracter por caracter
            
            if (a == EOF){
                reachedEOF=1;
                break;
            }
            if(a=='\r'){
                a=getc(csv_file);
            }
            if(a==',' || a=='\n'){              // momento em que o campo atual acaba de ser lido
                str[i]='\0';        
                break;
            }
            else{
                str[i]=a;
            }
        }

        int len = strlen(str);            //tamanho do campo para utilizar nos campos de tamanho variavel

        if(reachedEOF){
            free(str);
            liberarRegistro(&registro);
            break;
        }

        if(rotation==0){            // foi lido o campo de id
        
            if(str[0]=='\0'){
                setId(registro, -1);
                if(str!=NULL)
                    free(str);
            }
            else{
                setId(registro, atoi(str));
                free(str);
            }
        }
        else if(rotation==1){            // foi lido o campo de idade
            if(str[0]=='\0'){
                setIdade(registro, -1);
                if(str!=NULL)
                    free(str);
            }
            else{
                setIdade(registro, atoi(str));
                free(str);
            }
        }
        else if(rotation==2){            // foi lido o campo de nome do jogador
            setTamNomeJogador(registro, len);
            if(str[0]!='\0'){
                setNomeJogador(registro, str);  
            }
            else {
                free(str);
            }
        }
        else if(rotation==3){            // foi lido o campo de nacionalidade
            setTamNacionalidade(registro, len);
            if(str[0]!='\0'){
                setNacionalidade(registro, str);
            }
            else {
                free(str);
            }
        }
        else{            // foi lido o campo de nome do clube e tambem chega a hora de finalizar o registro e escrevelo no binario
            setTamNomeClube(registro, len);
            if(str[0]!='\0'){
                setNomeClube(registro, str);
            }
            else {
                free(str);
            }
            setProx(registro, -1);
            setRemovido(registro, '0');
        // variavel que armazena o tamanho do registro 33 fio + os campos de tamanho variavel
	    int tamReg = 33 + getTamNomeClube(registro) + getTamNacionalidade(registro) + getTamNomeJogador(registro);
	    byteOff += tamReg;
            setTamanhoRegistro(registro, tamReg);
            writeRegistroDados(fileObj, registro);
            
            liberarRegistro(&registro);
            //free(str);
            cnt++;
            rotation = 0;
            continue;
            
        }
        rotation++;
       
    }
    setHeaderStatus(fileObj, '1');
    setHeaderProxByteOffset(fileObj, byteOff);
    setHeaderNroRegArq(fileObj, cnt);
    writeRegistroCabecalho(fileObj);
    fecharArquivoBin(&fileObj);
    fclose(csv_file);

    binarioNaTela(bin_name);
    return 1;
}

int create_index(char* bin_name, char* index_bin_name){
    FILE *bin = fopen(bin_name, "rb");             // abre o arquivo binario

    data_index **arr = criarVetorIndice(30113);
    
    if(bin==NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    char stats;
    fread(&stats, 1, 1, bin);

    if(stats=='0'){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    fseek(bin, 24, SEEK_CUR);            
    int i=0;
    while(1){
        int64_t byteOff=ftell(bin);
        char status = getc(bin);        // necessario para verificar se chegamos em EOF    
        if (status == EOF)
            break;
        
        if(status == '1'){
            int tamReg=0;        // o registro esta logicamente removido portanto vamos pular o registro inteiro
            fread(&tamReg, 4, 1, bin);
            fseek(bin, tamReg-5, SEEK_CUR);
            continue;
        }

        int tamReg=0;        
        fread(&tamReg, 4, 1, bin);
        fseek(bin, 8, SEEK_CUR);

        int id;
        fread(&id, 4, 1, bin);

        

        setIndiceByteOff(arr[i], byteOff);
        setIndiceId(arr[i], id);

        fseek(bin, tamReg-17, SEEK_CUR);
        i++;
    }

    qsort(arr, 30113, sizeof(data_index*), comparaIndice);

    file_object_ind* fileObj = criarArquivoBinInd(index_bin_name);            // cria o arquivo bin
    setHeaderStatusInd(fileObj, '1');
    writeRegistroCabecalhoInd(fileObj);
    writeRegistroDadosInd(fileObj, arr, i);
    fecharArquivoBinInd(&fileObj);
    fclose(bin);
    binarioNaTela(index_bin_name);
    return 1;
}

void select_from(char* bin_name){  
    file_object *bin = criarArquivoBin(bin_name, "rb");          // função que imprime os registros do arquivo binario na forma pedida (operação 2)
    //FILE *bin = fopen(bin_name, "rb");             // abre o arquivo binario
    
    if(!verificaConsistencia(bin)) return;

    inicioRegistroDeDados(bin);

    int EXIST=0;            // variavel para auxiliar em caso de registro inexistente

    player_data *player = criarPlayer();

    while(1){
        int r=processaRegistro(bin, player);
        if(r==-1) break;
        
        if(r==1){
            imprimePlayerData(player);
        liberaPlayer(player);
        EXIST = 1;
        }
    }

    if(!EXIST){
        printf("Registro inexistente.\n\n");
    }

    free(player);
    fecharArquivoBin(&bin);
}

void select_from_where(char *bin_name, int num_queries){        // função que imprime os registros especificos de cada busca do arquivo binario na forma pedida (operação 3)
        player_data * parametros[1024];        // vetor que ira armazenar as especificaçoes de cada busca para lermos primeiro a entrada e somente depois imprimir


        for(int i=0;i<num_queries;i++){
            parametros[i] = lePlayerData();
        }           // Fim da computação da entrada

        // inicio da operação
        file_object *bin = criarArquivoBin(bin_name, "rb");
        if (!verificaConsistencia(bin)) return;

        for(int i=0;i<num_queries;i++){             // LOOP DAS BUSCAS
            int EXIST=0;            // variavel que auxilia em caso de registro inexistente
            inicioRegistroDeDados(bin); 

            printf("Busca %d\n\n", i+1);            

            player_data* player = criarPlayer();
        
            while(1){
                 int r=processaRegistro(bin, player);
                if(r==-1) break;
        
                if(r==1){ 
               if(comparaPlayer(player, parametros[i])){
                    EXIST=1;
                    imprimePlayerData(player);
                    //if(idbuscado!=-1) break; chris coda essa parte
                 }
                }
                liberaPlayer(player);       
            }
               

            if(!EXIST){
                printf("Registro inexistente.\n\n");
            }
           // free(player);
            fecharArquivoBin(&bin);
        }
}       

void delete_from_where(char *bin_name, char *index_bin_name, int n)
{
    parametrosDeBusca parametros[1024]; // vetor que ira armazenar as especificaçoes de cada busca para lermos primeiro a entrada e somente depois imprimir
    int num_fields;                     // quantidade de campos que sao requisitados na busca
    char field_name[20];                // para ler o nome do campo

    for (int i = 0; i < n; i++)
    {
        parametros[i].id = -1;
        parametros[i].idade = -1;
        parametros[i].nacionalidade[0] = '\0';
        parametros[i].clube[0] = '\0';
        parametros[i].nome[0] = '\0';

        scanf("%d", &num_fields);
        for (int j = 0; j < num_fields; j++)
        {
            scanf("%s", field_name);
            if (strcmp(field_name, "id") == 0)
            {
                scanf("%d", &parametros[i].id);
            }
            else if (strcmp(field_name, "idade") == 0)
            {
                scanf("%d", &parametros[i].idade);
            }
            else if (strcmp(field_name, "nacionalidade") == 0)
            {
                scan_quote_string(parametros[i].nacionalidade);
            }
            else if (strcmp(field_name, "nomeJogador") == 0)
            {
                scan_quote_string(parametros[i].nome);
            }
            else if (strcmp(field_name, "nomeClube") == 0)
            {
                scan_quote_string(parametros[i].clube);
            }
        }
    } // Fim da computação da entrada

    // inicio da operação
    pair arr[10002];
    int tamfila=0;
    int arquivosRemovidos=0;
    int tinhaArquivos = 0;

    FILE *bin = fopen(bin_name, "rb+"); // abre o arquivo binario

     if (bin == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            return;
        }

    char stats;
        fread(&stats, 1, 1, bin);

        if (stats == '0')
        {
            printf("Falha no processamento do arquivoi.\n");
            return;
        }

         fseek(bin, 0, SEEK_SET);
        char inconsistente='0';
        fwrite(&inconsistente, 1, 1, bin);
        

        int64_t Topo;
        fread(&Topo, 8, 1, bin);

        int64_t next = Topo;
        while (next != -1) {
            // Topo -> next -> next(next) -> deleted[0] -> deleted[1] ... deleted(n_queries)
            // sort(fila) por tamReg
            // do inicio do arquivo ao longo da fila, vamos ajeitar os next
            fseek(bin, next+1, SEEK_SET); // vou pro offset do tamReg do next, que eh next+1
            int tamReg = 0;
            fread(&tamReg, 4, 1, bin);
            arr[tamfila].tamReg = tamReg;
            arr[tamfila].byteOff = next;
            tamfila++;
            fread(&next, 8, 1, bin);
        }

    for (int i = 0; i < n; i++)
    { // LOOP DAS BUSCAS
    
        
        //FILE *bin = fopen(bin_name, "rb+"); // abre o arquivo binario

        // char stats;
        // fread(&stats, 1, 1, bin);

        // if (stats == '0')
        // {
        //     printf("Falha no processamento do arquivoi.\n");
        //     return;
        // }
        // como vamos escrever no arquivo mudamos seu estado para inconsistente
        fseek(bin, 0, SEEK_SET);
        char inconsistente='0';
        fwrite(&inconsistente, 1, 1, bin);
        

        fseek(bin, 16, SEEK_CUR);

        int nroRegArq;
        int nroRegRem;
        fread(&nroRegArq, 4, 1, bin);
        fread(&nroRegRem, 4, 1, bin);

        tinhaArquivos=nroRegArq;
        arquivosRemovidos=nroRegRem;

        int idBuscado = parametros[i].id;                         // o id buscado
        int idadeBuscada = parametros[i].idade;                   // a idade buscada
        char *nacionalidadeBuscada = parametros[i].nacionalidade; // a nacionalidade
        char *nomeBuscado = parametros[i].nome;                   // nome
        char *clubeBuscado = parametros[i].clube;                 // clube

        int64_t jump = -1;
        if (idBuscado != -1)
        {
            jump = indBB(idBuscado, index_bin_name, nroRegArq); // nao fiz a funçao nem defini ainda
            if(jump==-2){
                 
                return;
            }
        }
        
        // reinicialaiza o pontreiro do arquivo
        //fseek(bin, 25, SEEK_SET);

        if (jump != -1)
        {
            fseek(bin, jump, SEEK_SET);

            if (fitted(bin, idBuscado, idadeBuscada, nacionalidadeBuscada, nomeBuscado, clubeBuscado))
            {
                fseek(bin, jump, SEEK_SET);
                char removidological='1';
                fwrite(&removidological, 1, 1, bin);
                fread(&arr[tamfila].tamReg, 4, 1, bin);
                arr[tamfila].byteOff=jump;
                tamfila++;
               
            }
        }
        else
        {
            while (1)
            { // COMEÇAMOS A LER O ARQUIVO BINARIO
                int64_t byteOff = ftell(bin);
               
                int ans=fitted(bin, idBuscado, idadeBuscada, nacionalidadeBuscada, nomeBuscado, clubeBuscado);

                if(ans==-1) break;

                if (ans==1)
                {
                    fseek(bin, byteOff, SEEK_SET);
                    char removidological='1';
                    fwrite(&removidological, 1, 1, bin);
                    int tamReg = 0;
                    fread(&tamReg, 4, 1, bin);
                    arr[tamfila].tamReg = tamReg;
                    arr[tamfila].byteOff=byteOff;
                    fseek(bin, tamReg-5, SEEK_CUR);
                    tamfila++;
                    
                }
                
            }
        }
    }

    //printf("cheguei aqui\n");
    
    int novosRemovidos = tamfila - arquivosRemovidos;

    qsort(arr, tamfila, sizeof(pair), comparaPair);

    fseek(bin, 0, SEEK_SET);
    char consistente='1';
    fwrite(&consistente, 1, 1, bin); // consinstente 
    fseek(bin, 16, SEEK_CUR); // pulamos topop e byteoffset
    int auxiliar=tinhaArquivos-novosRemovidos;
    fwrite(&(auxiliar), 4, 1, bin);
    fwrite(&tamfila, 4, 1, bin);



    fseek(bin, 1, SEEK_SET); // vamos comecar do campo topo do cabecalho
    for(int i=0;i<tamfila;i++){
        int64_t nowByte=arr[i].byteOff;
        fwrite(&nowByte, 8, 1, bin);
        fseek(bin, nowByte+5, SEEK_SET); // dou fseek pro campo de offset do proximo elemento da fila e somo tamReg + status
    }
    int64_t menos1=-1;
    fwrite(&menos1, 8, 1, bin);
    fclose(bin);

    binarioNaTela(bin_name);
    create_index(bin_name, index_bin_name);
}

void insert_into(char* bin_name, char* index_bin_name, int n){
    parametrosDeBusca parametros[1024]; // vetor que ira armazenar as especificaçoes de cada busca para lermos primeiro a entrada e somente depois imprimir
    int tamRegistros[1024];

    for (int i = 0; i < n; i++) {
        parametros[i].id = -1;
        parametros[i].idade = -1;
        parametros[i].nacionalidade[0] = '\0';
        parametros[i].clube[0] = '\0';
        parametros[i].nome[0] = '\0';

        char temp_idade[50];
        scanf("%d", &parametros[i].id);
        scan_quote_string(temp_idade);
        if(temp_idade[0] != '\0')
            parametros[i].idade = atoi(temp_idade);
        scan_quote_string(parametros[i].nome);
        scan_quote_string(parametros[i].nacionalidade);
        scan_quote_string(parametros[i].clube);
        tamRegistros[i] = 33 + strlen(parametros[i].nome)
                             + strlen(parametros[i].nacionalidade)
                             + strlen(parametros[i].clube);
    }

    FILE *bin = fopen(bin_name, "rb+");
    char status;

    fread(&status, 1, 1, bin);
    if (status == '0') {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    status = '0'; // status inconsistente
    fseek(bin, 0, SEEK_SET);
    fwrite(&status, 1, 1, bin);

    int64_t topo = -1, byteOff = 0;
    int numRegistros = 0, numRemovidos = 0;
    fread(&topo, 8, 1, bin);
    int64_t fim = 0;
    fread(&fim, 8, 1, bin);
    printf("fim do arq: %ld\n", fim);
    fread(&numRegistros, 4, 1, bin);
    fread(&numRemovidos, 4, 1, bin);
    printf("num reg: %d\n", numRegistros);
    printf("num removido: %d\n", numRemovidos);

    if (topo == -1) {
        fseek(bin, 0, SEEK_END);
        printf(" fim do arq: %ld", ftell(bin));
        for (int i = 0; i < n; i++) {
            char removido = '0';
            int64_t prox = -1;
            fwrite(&removido, 1, 1, bin);
            fwrite(&tamRegistros[i], 4, 1, bin);
            fwrite(&prox, 8, 1, bin);
            fwrite(&parametros[i].id, 4, 1, bin);
            fwrite(&parametros[i].idade, 4, 1, bin);
            int tamNomeJog = strlen(parametros[i].nome);
            int tamNacionalidade = strlen(parametros[i].nacionalidade);
            int tamNomeClube = strlen(parametros[i].clube);

            printf("tamanho: %d\n", 33+tamNomeJog+tamNomeClube+tamNacionalidade);
            printf("%s, %s, %s", parametros[i].nome, parametros[i].nacionalidade, parametros[i].clube);
            fwrite(&tamNomeJog, 4, 1, bin);
            if (tamNomeJog) fwrite(parametros[i].nome, 1, tamNomeJog, bin);
            fwrite(&tamNacionalidade, 4, 1, bin);
            if (tamNacionalidade) fwrite(parametros[i].nacionalidade, 1, tamNacionalidade, bin);
            fwrite(&tamNomeClube, 4, 1, bin);
            if (tamNomeClube) fwrite(parametros[i].clube, 1, tamNomeClube, bin);
            numRegistros++;
        }
    }
    else {
        for (int i = 0; i < n; i++) {
            fseek(bin, 1, SEEK_SET);
            int64_t last = 1;
            fread(&byteOff, 8, 1, bin);
            printf("byteoff eh : %ld last eh : %ld\n", byteOff, last);
            int tamReg = 0;
            int flag = 0; // se conseguimos inserir num campo logicamente removido ou n
            while (byteOff != -1) {
                fseek(bin, byteOff+1, SEEK_SET);
                fread(&tamReg, 4, 1, bin);
                if (tamRegistros[i] <= tamReg) {
                    printf("achou");
                    fseek(bin, byteOff, SEEK_SET);
                    char removido = '0';
                    int64_t prox = -1, next = -1;
                    fwrite(&removido, 1, 1, bin);
                    printf("eu to aqui: %ld\n", ftell(bin));
                    printf("tamRegistros: %d\n", tamRegistros[i]);
                    fwrite(&tamReg, 4, 1, bin);
                    fread(&next, 8, 1, bin);
                    fseek(bin, ftell(bin)-8, SEEK_SET);
                    printf("next eh %ld", next);
                    fwrite(&prox, 8, 1, bin);
                    fwrite(&parametros[i].id, 4, 1, bin);
                    fwrite(&parametros[i].idade, 4, 1, bin);
                    int tamNomeJog = strlen(parametros[i].nome);
                    int tamNacionalidade = strlen(parametros[i].nacionalidade);
                    int tamNomeClube = strlen(parametros[i].clube);
                    printf("%s tam: %d %s tam: %d %s tam: %d\n", parametros[i].nome, tamNomeJog, parametros[i].nacionalidade, tamNacionalidade, parametros[i].clube, tamNomeClube);
                    fwrite(&tamNomeJog, 4, 1, bin);
                    if (tamNomeJog) fwrite(parametros[i].nome, 1, tamNomeJog, bin);
                    fwrite(&tamNacionalidade, 4, 1, bin);
                    if (tamNacionalidade) fwrite(parametros[i].nacionalidade, 1, tamNacionalidade, bin);
                    fwrite(&tamNomeClube, 4, 1, bin);
                    if (tamNomeClube) fwrite(parametros[i].clube, 1, tamNomeClube, bin);

                    for (int k = 0; k < tamReg - tamRegistros[i]; k++) {
                        char dollar = '$';
                        fwrite(&dollar, 1, 1, bin);
                    }
                    fseek(bin, last, SEEK_SET);
                    printf("vou mandar %ld pra %ld\n", last, next);
                    fwrite(&next, 8, 1, bin);

                    numRemovidos--;
                    flag = 1;
                    break;
                }
                else {
                    last = byteOff+5;
                    fread(&byteOff, 8, 1, bin);
                    printf("byteoff eh : %ld last eh : %ld\n", byteOff, last);
                }
            }
            if (!flag) {
                fseek(bin, 0, SEEK_END);
                char removido = '0';
                int64_t prox = -1;
                fwrite(&removido, 1, 1, bin);
                fwrite(&tamRegistros[i], 4, 1, bin);
                fwrite(&prox, 8, 1, bin);
                fwrite(&parametros[i].id, 4, 1, bin);
                fwrite(&parametros[i].idade, 4, 1, bin);
                int tamNomeJog = strlen(parametros[i].nome);
                int tamNacionalidade = strlen(parametros[i].nacionalidade);
                int tamNomeClube = strlen(parametros[i].clube);
                fwrite(&tamNomeJog, 4, 1, bin);
                if (tamNomeJog) fwrite(parametros[i].nome, 1, tamNomeJog, bin);
                fwrite(&tamNacionalidade, 4, 1, bin);
                if (tamNacionalidade) fwrite(parametros[i].nacionalidade, 1, tamNacionalidade, bin);
                fwrite(&tamNomeClube, 4, 1, bin);
                if (tamNomeClube) fwrite(parametros[i].clube, 1, tamNomeClube, bin);
            }
            numRegistros++;
        }
    }
    fseek(bin, 0, SEEK_END);
    int64_t fimDoArq = ftell(bin);
    printf("novo fim: %ld\n", fimDoArq);
    fseek(bin, 0, SEEK_SET);
    status = '1';
    fwrite(&status, 1, 1, bin);
    int64_t teste = 0;
    fread(&teste, 8, 1, bin);
    printf("topo: %ld\n", teste);
    //fseek(bin, 8, SEEK_CUR);
    fwrite(&fimDoArq, 8, 1, bin);
    fwrite(&numRegistros, 4, 1, bin);
    fwrite(&numRemovidos, 4, 1, bin);
    printf("num reg final: %d\n", numRegistros);
    printf("num removidos final: %d\n", numRemovidos);
    fclose(bin);
    binarioNaTela(bin_name);
    create_index(bin_name, index_bin_name);
}

