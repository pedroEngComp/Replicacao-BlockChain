#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <openacc.h>
#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

#define HASH_SIZE 32         // 32 Bytes
#define MAX_SEVERIDADE 4     // Dificuldade
#define MAX_NUMEROPROOFS 100 // Qauntidade de nos no caminho de prova

typedef struct MerkleNode
{
    unsigned char hash[HASH_SIZE + 1]; // Hash do nó
    struct MerkleNode *left;           // Filho esquerdo
    struct MerkleNode *right;          // Filho direito
    time_t timestampTransacao;
} MerkleNode;

typedef struct
{
    MerkleNode **nodes;
    int size;
} NodeList;

typedef struct chain
{
    int indice;
    time_t timestamp;
    unsigned char hashChainAtual[HASH_SIZE];
    unsigned char hashChainAnterior[HASH_SIZE];
    long nonceAtual;
    NodeList merkleTree;

} Chain;

/*
@brief faz o calculo do hash usando a biblioteca openSsl
@param block: Struct para gerar o hash
@param hashGerado: Output do hash

*/
void GerarHashNode(Chain *block, unsigned char *hashGerado);

/*
@brief usa um laço para exibir o hash em forma hexadecimal
@param hash: hash a ser exibido
 */
void exibirHash(unsigned char *hash);

/*
@brief Converte um hash binário em uma string hexadecimaL
@param hashBin: Ponteiro para o hash binário gerado
@param hashHex: Ponteiro para o buffer onde o hash hexadecimal será armazenado
*/
void HashParaHex(const unsigned char *hashBin, char *hashHex, size_t tamanhoHash);

/*
@brief Com base na dificuldade, realiza a mineração do nounce especifico, a fim de alterar o hash
@param bloco: struct para o POW
@param dificuldade: de 1 a 4
*/
unsigned char ProofOfWorkLinear(Chain *bloco, int dificuldade);

/*
@brief Verifica se uma transação está incluída na árvore de Merkle e constrói o caminho de prova (proof of inclusion)
@param no: Nó da árvore de Merkle a ser verificado
@param incluido: transação que se deseja verificar se está incluída na árvore
@param proofhash: array de strings que armazenará os hashes do caminho de prova
@param indice: índice atual no array de proofhash
@return retorna 1 se a transação estiver incluída na árvore, caso contrário retorna 0
*/
int poi(MerkleNode *no, const char *incluido, char proofhash[][HASH_SIZE * 2 + 1], int *indice);

/*
@brief Cria um nó folha da árvore de Merkle com base nos dados fornecidos
@param data: dados da transação que serão armazenados no nó folha
@return Retorna um ponteiro para o nó folha criado
*/
MerkleNode *create_leaf(const char *data);

/*
@brief Cria um nó pai na árvore de Merkle com base nos nós filhos fornecidos
@param left: nó filho esquerdo
@param right: nó filho direito
@return Retorna um ponteiro para o nó pai criado
*/
MerkleNode *create_parent(MerkleNode *left, MerkleNode *right);

/*
@brief Constrói uma árvore de Merkle a partir de uma lista de transações
@param transactions: array de strings contendo as transações
@param num_transactions: número de transações no array
@return Retorna uma estrutura NodeList contendo os nós da árvore de Merkle
*/
NodeList build_merkle_tree(char **transactions, int num_transactions);

/*
@brief Libera a memória alocada para a árvore de Merkle
@param root: ponteiro para a raiz da árvore de Merkle
*/
void free_merkle_tree(MerkleNode *root);

/*
@brief Exibe a árvore de Merkle de forma hierárquica
@param root: ponteiro para a raiz da árvore de Merkle
@param level: nível atual na árvore (usado para indentação na exibição)
*/
void print_merkle_tree(MerkleNode *root, int level);

/*
@brief Instancia uma nova BlockChain, iniciando os campos necessarios de cada bloco
@param n: Quantidade de blocos
*/
Chain *novoBlockChain(int n);

/*
@brief Calcula o hash de um char e o retorna por referência
@param input: string de entrada
@param output: string de saida
*/
void calculate_hash(const char *input, char *output);

/*
@brief Exibe os dados de cada bloco da blockChain
@param blockChain: BlockChain com os blocos
@param tam: Quantidade de blocos da BlockChain
*/
void printBlocos(Chain *blockChain, int tam);

/*
@brief instancia um novo bloco de acordo com os parâmetros
@param transacoes vetor de strings
@param qtdTransacoes total de transações
@param indice posicao na BlockChain
@param dificuldade 0 - 4
@param blockChain BlockChain que ira receber o bloco
@param tam: Quantidade de blocos da BlockChain
*/
Chain novoBloco(char **transacoes, int qtdTransacoes, int indice, int dificuldade, Chain *blockChain, int tam);

/*
@brief Verifica se uma transacao esta presente em um bloco
@param blockChain: BlockChain com os blocos
@param transacao: transacao para pesquisa
*/
void existeTransacao(Chain *blockChain, char *transacao);

/*
@brief Simula um ataque a uma BlockChain
@param blockChain: BlockChain com os blocos
@param numblocks: quantidade de blocos
@param indicealterado: indice que ira receber o ataque
@param transacao: transacao fraudulenta
*/
void simularAtaque(Chain *blockchain, int numblocks, int indicealterado, char *transacao);