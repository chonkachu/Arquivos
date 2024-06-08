#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "db.h"
#include "file_utils.h"

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
            parametros[i] = lerPlayerData(SELECT);
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
                    // if(idbuscado(parametros[i])!=-1){
                    //     liberaPlayer(player); 
                    //      break;
                    // } 
                 }
                }
                liberaPlayer(player);       
            }
               

            if(!EXIST){
                printf("Registro inexistente.\n\n");
            }
        }
        fecharArquivoBin(&bin);
}       

void delete_from_where(char *bin_name, char *index_bin_name, int n)
{
    player_data * parametros[1024];        // vetor que ira armazenar as especificaçoes de cada busca para lermos primeiro a entrada e somente depois imprimir

    for (int i = 0; i < n; i++)
    {
        parametros[i] = lerPlayerData(DELETE);
    } // Fim da computação da entrada

    // inicio da operação
    pair arr[10002];
    int tamfila=0;
    file_object *bin = criarArquivoBin(bin_name, "rb+");
    if (!verificaConsistencia(bin)) return;

    setHeaderStatus(bin, '0'); // escrevo que esta inconsistente

    // obtendo todos os registros que estão apagados
    int64_t next = getTopo(bin);
    while (next != -1) {
        // Topo -> next -> next(next) -> deleted[0] -> deleted[1] ... deleted(n_queries)
        // sort(fila) por tamReg
        // do inicio do arquivo ao longo da fila, vamos ajeitar os next
        fseek(getFile(bin), next+1, SEEK_SET); // vou pro offset do tamReg do next, que eh next+1
        int tamReg = 0;
        fread(&tamReg, 4, 1, getFile(bin));
        arr[tamfila].tamReg = tamReg;
        arr[tamfila].byteOff = next;
        tamfila++;
        fread(&next, 8, 1, getFile(bin));
    }

    int tinhaArquivos = getNroRegArq(bin);
    int  arquivosRemovidos= getNroRegRem(bin);

    for (int i = 0; i < n; i++)
    { // LOOP DAS BUSCAS

        inicioRegistroDeDados(bin);
        int64_t jump = -1;
        if (idbuscado(parametros[i]) != -1)
        {
            jump = indBB(idbuscado(parametros[i]), index_bin_name, tinhaArquivos); // nao fiz a funçao nem defini ainda
            if(jump==-2){
                return;
            }
        }
        if (jump != -1) {
            fseek(getFile(bin), jump, SEEK_SET);
            player_data * player=criarPlayer();
            processaRegistro(bin, player);
            if (comparaPlayer(player, parametros[i]))
            {
                //imprimePlayerData(player);
                fseek(getFile(bin), jump, SEEK_SET);
                char removidological='1';
                fwrite(&removidological, 1, 1, getFile(bin));
                fread(&arr[tamfila].tamReg, 4, 1, getFile(bin));
                arr[tamfila].byteOff=jump;
                tamfila++;
            }
        }
        else {
            while (1)
            { // COMEÇAMOS A LER O ARQUIVO BINARIO
                int64_t byteOff = ftell(getFile(bin));

                player_data * player = criarPlayer();
                int ans = processaRegistro(bin, player);

                if(ans==-1) break;

                if (ans==1 && comparaPlayer(player, parametros[i]))
                {
                    //imprimePlayerData(player);
                    fseek(getFile(bin), byteOff, SEEK_SET);
                    char removidological='1';
                    fwrite(&removidological, 1, 1, getFile(bin));
                    int tamReg = 0;
                    fread(&tamReg, 4, 1, getFile(bin));
                    arr[tamfila].tamReg = tamReg;
                    arr[tamfila].byteOff=byteOff;
                    fseek(getFile(bin), tamReg-5, SEEK_CUR);
                    tamfila++;
                }
            }
        }
    }

    int novosRemovidos = tamfila - arquivosRemovidos; // qtd de novos caras que foram removidos

    qsort(arr, tamfila, sizeof(pair), comparaPair);

    setHeaderStatus(bin, '1'); 
    int auxiliar = tinhaArquivos-novosRemovidos;
    setHeaderNroRegArq(bin, auxiliar);
    setHeaderNroRegRem(bin, tamfila);

    writeRegistroCabecalho(bin);

    fseek(getFile(bin), 1, SEEK_SET); // vamos comecar do campo topo do cabecalho
    for(int i = 0; i < tamfila; i++){
        int64_t nowByte = arr[i].byteOff;
        fwrite(&nowByte, 8, 1, getFile(bin));
        fseek(getFile(bin), nowByte+5, SEEK_SET); // dou fseek pro campo de offset do proximo elemento da fila e somo tamReg + status
    }

    int64_t menos1=-1;
    fwrite(&menos1, 8, 1, getFile(bin));
    fecharArquivoBin(&bin);

    binarioNaTela(bin_name);
    create_index(bin_name, index_bin_name);
}

void insert_into(char* bin_name, char* index_bin_name, int n){
    player_data* parametros[1024]; // vetor que ira armazenar as especificaçoes de cada busca para lermos primeiro a entrada e somente depois imprimir
    data_registry *registros[1024];
    int tamRegistros[1024];

    for (int i = 0; i < n; i++) {
        parametros[i] = lerPlayerData(INSERT);
        registros[i] = criarRegistroFromPlayer(parametros[i]);
        tamRegistros[i] = getTamRegistro(registros[i]);
    }

    file_object *bin = criarArquivoBin(bin_name, "rb+");
    if (!verificaConsistencia(bin))
        return;

    int64_t topo = getTopo(bin), byteOff = 0;
    int numRegistros = getNroRegArq(bin);
    int numRemovidos = getNroRegRem(bin);

    setHeaderStatus(bin, '0'); // vamos operar no arquivo agr

    if (topo == -1) {
        fimRegistroDeDados(bin);
        for (int i = 0; i < n; i++) {
            writeRegistroDados(bin, registros[i]);
            numRegistros++;
            liberarRegistro(&registros[i]);
        }
    }
    else {
        for (int i = 0; i < n; i++) {
            inicializaHeader(bin);
            byteOff = getTopo(bin);
            int64_t last = 1;
            int flag = 0; // se conseguimos inserir num campo logicamente removido ou n
            while (byteOff != -1) {
                gotoByteOffArquivoBin(bin, byteOff); // vamos para o proximo cara na lista de removidos
                data_registry *removido = processaRegistroRemovido(bin, byteOff);
                int tamReg = getTamRegistro(removido);
                if (tamRegistros[i] <= tamReg) {
                    setTamanhoRegistro(registros[i], tamReg);
                    writeRegistroDados(bin, registros[i]);

                    for (int k = 0; k < tamReg - tamRegistros[i]; k++) {
                        char dollar = '$';
                        fwrite(&dollar, 1, 1, getFile(bin));
                    }
                    gotoByteOffArquivoBin(bin, last);
                    int64_t next = getProx(removido);
                    fwrite(&next, 8, 1, getFile(bin));
                    if (last == 1) {
                        setHeaderTopo(bin, next);
                        writeRegistroCabecalho(bin);
                    }
                    numRemovidos--;
                    flag = 1;
                    break;
                }
                else {
                    last = byteOff+5; // vai pra exatamente o byteoffset do campo de prox do registro removido anterior
                    byteOff = getProx(removido);
                }
            }
            if (!flag) { // se nao achou lugar pra colocar, insere no final
                fimRegistroDeDados(bin);
                writeRegistroDados(bin, registros[i]);
            } 
            numRegistros++;
            liberarRegistro(&registros[i]);
        }
    }
    int64_t fimDoArq = tamanhoBin(bin);
    setHeaderProxByteOffset(bin, fimDoArq);
    setHeaderStatus(bin, '1');
    setHeaderNroRegArq(bin, numRegistros);
    setHeaderNroRegRem(bin, numRemovidos);
    writeRegistroCabecalho(bin);
    fecharArquivoBin(&bin);

    binarioNaTela(bin_name);
    create_index(bin_name, index_bin_name);
}
