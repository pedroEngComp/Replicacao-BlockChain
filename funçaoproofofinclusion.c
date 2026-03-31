#include "StructChain.h"

//ISSO É UM PONTO C APENAS PARA TESTE ISOLADO

//verificar se a intencao era passar o indice como referencia 

int poi(MerkleNode *no, const char *incluido, char proofhash[][HASH_SIZE * 2 + 1], int indice)
{
    // em proofhash, temos o tamanho definido pelo tamanho padrão do hash, 32bits vezes 2, para cada char ter espaço +1 para o caracter \0 no fim.

    // caso base, se encontra na raiz
    if (no->left == NULL && no->right == NULL)
    {
        // compara as 2 strings, caso iguais, o return pode vir com 1 ou 0, tem ou não tem incluido.
        return strcmp(no->hash, incluido) == 0;
    }

    // anda para a esquerda e chama a função com uma nova root
    if (no->left && poi(no->left, incluido, proofhash, indice))
    {
        // verifica existencia de alguem na direita da nova raiz
        if (no->right)
        {
            // copia o hash do no da direita para o indice trabalhado, depois incrementa pro proximo indice.
            strcpy(proofhash[indice], no->right->hash);
            (indice)++;
        }
        else
        {
            // coloca nada, não existe mais a direita, depois incrementa pro proximo indice.
            strcpy(proofhash[indice], "");
            (indice)++;
        }
        // encontrado na esquerda
        return 1;
    }

    // anda para a direita e chama a função com uma nova root{
    if (no->right && poi(no->right, incluido, proofhash, indice))
    {
        // verifica existencia de alguem na esquerda da nova raiz
        if (no->left)
        {
            // copia o hash do no da esquerda para o indice trabalhado, depois incrementa pro proximo indice.
            strcpy(proofhash[indice], no->left->hash);
            (indice)++;
        }
        else
        {
            // coloca nada, não existe mais a esquerda, depois incrementa pro proximo indice.
            strcpy(proofhash[indice], "");
            (indice)++;
        }
        // encontrado na direita
        return 1;
    }
    // caso não exista nó.
    return 0;
}

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
