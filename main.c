/* 
1ª Entrega Projeto Sistemas Operativos 
93588 - Joao Lopes
93598 - Matias Sommer
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <pthread.h>
#include "fs.h"
#include "lib/hash.h"
                                                                    
#ifdef MUTEX                                                        //Macros dependentes da flag introduzida no MakeFile para evitar repeticao de codigo
#define INIT pthread_mutex_init(&fs->mutex, NULL)                   //Definicao da variavel INIT responsavel pela inicializacao do Mutex
#define INITVEC pthread_mutex_init(&fs->mutexVec, NULL)             //Definicao da variavel INITVEC responsavel pela inicializacao do Mutex para o vetor de comandos
#define LOCK pthread_mutex_lock(&fs->mutex)                         //Definicao da variavel LOCK responsavel pelo fecho do trinco Mutex
#define RDLOCK pthread_mutex_lock(&fs->mutex)                       //O RDLOCK esta definido tambem na ocurrencia da flag Mutex, porque em contrario nao estaria definida. 
                                                                    //Essencialmente, o RDLOCK e o LOCK com a flag Mutex sao identicos
#define LOCKVEC pthread_mutex_lock(&fs->mutexVec)                   //Definicao da variavel LOCKVEC responsavel pelo fecho do Mutex para o vetor de comandos
#define UNLOCK pthread_mutex_unlock(&fs->mutex)                     //Definicao da variavel UNLOCK para o desbloqueio do Mutex indicado anteriormente
#define UNLOCKVEC pthread_mutex_unlock(&fs->mutexVec)               //Definicao da varial UNLOCKVEC para o desbloqueio do Mutex para o vetor de comandos indicado tambem anteriormente

#elif RWLOCK                                                        //Macros dependentes da flag introduzida no MakeFile
#define INIT pthread_rwlock_init(&fs->lock, NULL)                   //Nestas Linhas de codigo e repetido essencialmente o mesmo que com a flag Mutex, mas adaptado para o locks Read e Write
#define INITVEC pthread_rwlock_init(&fs->lockVec, NULL)
#define LOCK pthread_rwlock_wrlock(&fs->lock)
#define RDLOCK pthread_rwlock_rdlock(&fs->lock)                     //Aqui o RDLOCK ja se diferencia da variavel LOCK, no sentido em que o LOCK fecha o trinco de Write e o RDLOCK o trinco de Read
#define LOCKVEC pthread_rwlock_wrlock(&fs->lockVec)
#define UNLOCK pthread_rwlock_unlock(&fs->lock)
#define UNLOCKVEC pthread_rwlock_unlock(&fs->lockVec)

#else                                                               //No caso de nao ser chamado nem a flag -DMUTEX, nem a -DRWLOCK sao chamadas estas linhas de codigo, que correm o programa sem qualquer protecao de trincos
#define INIT NULL
#define INITVEC NULL
#define LOCK NULL
#define RDLOCK NULL
#define LOCKVEC NULL
#define UNLOCK NULL
#define UNLOCKVEC NULL

#endif


#define MAX_COMMANDS 150000
#define MAX_INPUT_SIZE 100


int numberThreads = 0;
int numberBuckets = 0;
tecnicofs* fs;

FILE* f_in;
FILE* f_out;

struct timeval start, end;

char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];
int numberCommands = 0;
int headQueue = 0;
                                                                    //Nas seguintes linhas de codigo e verificada a chamada de cada macro para proteger o programa de qualquer erro que possa ocorrer
void init(){
    if (INIT != 0){                                                                                      
        fprintf(stderr, "Error: failed creating threads\n");
        exit(EXIT_FAILURE);                                         //O programa e terminado no caso de ocorrer qualquer erro na chamada da funcoes do pthread e dos trincos
    }
}

void initVec(){
    if (INITVEC != 0){
        fprintf(stderr, "Error: failed creating threads\n");
        exit(EXIT_FAILURE);
    }
}

void lock(){
    if (LOCK != 0){
        fprintf(stderr, "Error: failed locking threads\n");
        exit(EXIT_FAILURE);
    }
}

void rdLock(){
    if (RDLOCK != 0){
        fprintf(stderr, "Error: failed locking threads2\n");
        exit(EXIT_FAILURE);
    }
}

void lockVec(){
    if (LOCKVEC != 0){
        fprintf(stderr, "Error: failed locking threads3\n");
        exit(EXIT_FAILURE);
    }
}

void unlock(){
    if (UNLOCK != 0){
        fprintf(stderr, "Error: failed unlocking threads\n");
        exit(EXIT_FAILURE);
    }
}

void unlockVec(){
    if (UNLOCKVEC != 0){
        fprintf(stderr, "Error: failed unlocking threads\n");
        exit(EXIT_FAILURE);
    }
}

static void displayUsage (const char* appName){
    printf("Usage: %s\n", appName);
    exit(EXIT_FAILURE);
}

static void parseArgs (long argc, char* const argv[]){
    if (argc != 5) {
        fprintf(stderr, "Invalid format:\n");
        displayUsage(argv[0]);
    }
}

int insertCommand(char* data) {
    if(numberCommands != MAX_COMMANDS) {
        strcpy(inputCommands[numberCommands++], data);
        return 1;
    }
    return 0;
}

char* removeCommand() {
    if(numberCommands > 0){
        numberCommands--;
        return inputCommands[headQueue++];  
    }
    return NULL;
}

void errorParse(){
    fprintf(stderr, "Error: command invalid\n");
    //exit(EXIT_FAILURE);
}

void processInput(FILE* f_in){
    char line[MAX_INPUT_SIZE];

    while (fgets(line, sizeof(line)/sizeof(char), f_in)) {
        char token;
        char name[MAX_INPUT_SIZE];

        int numTokens = sscanf(line, "%c %s", &token, name);

        if (f_in == NULL)
            return;
        
        /* perform minimal validation */
        if (numTokens < 1) {
            continue;
        }
        switch (token) {
            case 'c':
            case 'l':
            case 'd':
                if(numTokens != 2)
                    errorParse();
                if(insertCommand(line))
                    break;
                return;
            case '#':
                break;
            default: { /* error */
                errorParse();
            }
        }
    }
}

void* applyCommands(void* f_out){
    while(numberCommands > 0){
        lockVec();                                                   //Funcao auxiliar para fecho do trinco dependente da flag (MUTEX ou RWLOCK)
        const char* command = removeCommand();
        
        if (command == NULL){
            continue;
        }

        char token;
        char name[MAX_INPUT_SIZE];
        int numTokens = sscanf(command, "%c %s", &token, name);
        
        if (numTokens != 2) {
            fprintf(stderr, "Error: invalid command in Queue\n");
            exit(EXIT_FAILURE);
        }

        int searchResult;
        int iNumber;
      
        if (f_out == NULL)
            return NULL;

        switch (token) {                                              //Em cada case de saida e chamada uma funcao auxiliar que abre o trinco tambem dependente da flag
            case 'c':
                iNumber = obtainNewInumber(fs);
                unlockVec();                                        
                create(fs, name, iNumber);
                break;
            case 'l': 
                unlockVec();    
                searchResult = lookup(fs, name);
                if(!searchResult)
                    printf("%s not found\n", name);
                else
                    printf("%s found with inumber %d\n", name, searchResult);       
                break;
            case 'd':
                unlockVec();
                delete(fs, name);
                break;
            default: { /* error */
                fprintf(stderr, "Error: command to apply\n");          //Na ocasiao de existir um erro, nao e necessario abrir o trinco e o programa termina
                exit(EXIT_FAILURE);
            }
        }
    }
    return NULL;                                                       //No caso de nao ocorrerem erros, o return final substitui o pthread_exit() e finaliza a funcao
}


int main(int argc, char* argv[]) {
    int i;
    numberThreads = atoi(argv[3]);                                     //argv[3] e o quarto input do terminal que neste caso e o numero de threads que queremos utilizar no nosso programa
    pthread_t tid[numberThreads];                                      //Criacao de um vetor de IDs de threads do numero indicado no terminal

    double start_t, end_t;                                             //Inicializa 2 variaveis para o calculo do tempo de execucao do programa

    f_in = fopen(argv[1], "r");                                        //argv[1] e argv[2] sao o ficheiro de input e output respetivamente.
    f_out = fopen(argv[2], "w");                                       //Abrimos entao esses dois ficheiros como Read e Write tambem respetivamente
    
    parseArgs(argc, argv);

    fs = new_tecnicofs();
    processInput(f_in);

    gettimeofday(&start, NULL);                                        //Guarda o tempo inicial na variavel start(criada na struct timeval)

    init();

    for (i = 0; i < numberThreads; i++){                               //Criacao do numero de threads indicado anteriormente
        if(pthread_create(&tid[i], 0, applyCommands, f_out)!=0){       //Verificacao da criacao de threads para o caso de ocorrer um erro
            fprintf(stderr, "Error: failed creating threads\n");
            exit(EXIT_FAILURE);                                        //Se ocorrer tal erro o programa termina em erro
        }
    }
    
    for (i = 0; i < numberThreads; i++){                               //Dar inicio a cada thread criada
        if(pthread_join(tid[i], NULL)!=0){                             //Mais verificacao para o caso da ocorrencia de algum erro
            fprintf(stderr, "Error: failed joining threads\n");
            exit(EXIT_FAILURE);
        }
    }
    
    gettimeofday(&end, NULL);                                          //Guarda o tempo final na variavel start(criada na struct timeval)

    start_t = start.tv_sec + start.tv_usec/1000000.0;                  //Transforma o tempo inicial em segundos
    end_t = end.tv_sec + end.tv_usec/1000000.0;                        //Transforma o tempo final em segundos

    printf("TecnicoFS completed in %.4f seconds.\n", end_t - start_t);

    print_tecnicofs_tree(f_out, fs);

    free_tecnicofs(fs);
    fclose(f_in);
    fclose(f_out);

    exit(EXIT_SUCCESS);
}
