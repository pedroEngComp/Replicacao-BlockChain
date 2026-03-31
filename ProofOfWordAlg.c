#include "StructChain.h"


//ISSO É UM PONTO C APENAS PARA TESTE ISOLADO DA FUNÇÃO POW, A FUNÇÃO ESTÁ NO .H 


/*
@brief Com base na dificuldade, realiza a mineração do nounce especifico, a fim de alterar o hash
@param bloco: struct para o POW
@param dificuldade: de 1 a 4

*/
unsigned char ProofOfWork(Chain *bloco, int dificuldade)
{
    if(dificuldade < 1 || dificuldade > MAX_SEVERIDADE){
        printf("dificuldade invalida: %d\n", dificuldade);
        exit(1);
    }

    //tamanho definido
    char severidade[MAX_SEVERIDADE] = {0};

    //cria uma substring de acordo com a dificuldade
    for(int i=0;i<dificuldade;i++){
        severidade[i] = '0';
    }
    
    //finaliza a substring com o \0
    severidade[dificuldade] = '\0';

    //hash binario 
    unsigned char hash[HASH_SIZE];

    //em tamanho hexadecimal(64 bytes)
    char hexHash[HASH_SIZE*2 +1];

    int iteracoes = 0;

    while(1){
        iteracoes++;
        GerarHashNode(bloco,hash);
        
        HashParaHex(hash, hexHash, HASH_SIZE);

        exibirHash(hash);


        if (strncmp(hexHash, severidade, dificuldade) == 0)
{
            memcpy(bloco->hashChainAtual, hash, HASH_SIZE);
            printf("hash minerado com sucesso em %d iterações.\n", iteracoes);
            return 1;
        }

        bloco->nonceAtual++;
    }

}

int main()
{
    //TESTES 

    Chain *bloco = malloc(sizeof(Chain));
    bloco->indice = 10;
    bloco->nonceAtual = 2222;
    bloco->timestamp = 0;

    // memorySet
    memset(bloco->hashChainAnterior, 0, HASH_SIZE + 1);
    memset(bloco->hashChainAtual, 0, HASH_SIZE + 1);

    ProofOfWork(bloco,4);
    exibirHash(bloco->hashChainAtual);

    free(bloco);
    return 0;
}
