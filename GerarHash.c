#include "StructChain.h"


//ISSO É UM PONTO C APENAS PARA TESTE ISOLADO DA FUNÇÃO GERARHASH, A FUNÇÃO ESTÁ NO .H 


/*
@brief faz o calculo do hash usando a biblioteca openSsl
@param Chain block

*/
void GerarHashNode(Chain *block, unsigned char *hashGerado)
{

    // instancia um novo contexto para o hash
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    // determina o alg sha256 para o hash
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);

    // usa os dados do input para gerar o hash
    EVP_DigestUpdate(ctx, block, sizeof(*block));

    unsigned int aux;

    // endereça o calculo do hash ao parametro de saida
    EVP_DigestFinal_ex(ctx, hashGerado, &aux);

    EVP_MD_CTX_free(ctx);
}
/*
@brief usa um laço para exibir o hash em forma hexadecimal
@param unsigned char *hash
 */
void exibirHash(unsigned char *hash)
{

    for (int i = 0; i < HASH_SIZE; i++)
    {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

int main()
{

    Chain *bloco = malloc(sizeof(Chain));
    bloco->indice = 10;
    bloco->nonceAtual = 2222;
    bloco->timestamp = 0;

    // memorySet
    memset(bloco->hashChainAnterior, 0, HASH_SIZE + 1);
    memset(bloco->hashChainAtual, 0, HASH_SIZE + 1);

    // testes que validam que quando todos os dados sao estaticos, o hash gerado é o mesmo

    unsigned char aux[HASH_SIZE];

    GerarHashNode(bloco, aux);
    exibirHash(aux);

    free(bloco);
    return 0;
}
