#include "StructChain.h"

void exibirHash(unsigned char *hash)
{

    // em tamanho hexadecimal(64 bytes) + \0
    char auxHex[HASH_SIZE * 2 + 1];
    HashParaHex(hash, auxHex, HASH_SIZE);
    printf("%s", auxHex);
    printf("\n");
}

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

void HashParaHex(const unsigned char *hashBin, char *hashHex, size_t tamanhoHash)
{
    for (size_t i = 0; i < tamanhoHash; i++)
    {
        sprintf(hashHex + (i * 2), "%02x", hashBin[i]);
    }
    // finaliza a string
    hashHex[tamanhoHash * 2] = '\0';
}

unsigned char ProofOfWorkLinear(Chain *bloco, int dificuldade)
{
    if (dificuldade < 1 || dificuldade > MAX_SEVERIDADE)
    {
        printf("dificuldade invalida: %d\n", dificuldade);
        exit(1);
    }

    // tamanho definido
    char severidade[MAX_SEVERIDADE] = {0};

    // cria uma substring de acordo com a dificuldade
    for (int i = 0; i < dificuldade; i++)
    {
        severidade[i] = '0';
    }

    // finaliza a substring com o '\0'
    severidade[dificuldade] = '\0';

    // hash binario(32 bytes)
    unsigned char hash[HASH_SIZE];

    // em tamanho hexadecimal(64 bytes) + \0
    char hexHash[HASH_SIZE * 2 + 1];

    int iteracoes = 0;

    while (1)
    {
        iteracoes++;
        GerarHashNode(bloco, hash);

        HashParaHex(hash, hexHash, HASH_SIZE);

        // exibirHash(hash);

        if (strncmp(hexHash, severidade, dificuldade) == 0)
        {
            memcpy(bloco->hashChainAtual, hash, HASH_SIZE);
            printf("hash minerado com sucesso em %d iterações.\n", iteracoes);
            return 1;
        }

        // mineração linear
        bloco->nonceAtual++;
    }
}

void calculate_hash(const char *input, char *output)
{
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input, strlen(input), digest);

    // converte o hash para uma string hexadecimal
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(output + (i * 2), "%02x", digest[i]);
    }
    output[HASH_SIZE] = '\0'; // finaliza a string
}

MerkleNode *create_leaf(const char *data)
{
    MerkleNode *node = (MerkleNode *)malloc(sizeof(MerkleNode));
    if (!node)
    {
        fprintf(stderr, "Erro ao alocar memória para o nó folha.\n");
        exit(EXIT_FAILURE);
    }
    calculate_hash(data, node->hash);
    node->left = NULL;
    node->right = NULL;
    return node;
}

MerkleNode *create_parent(MerkleNode *left, MerkleNode *right)
{
    MerkleNode *node = (MerkleNode *)malloc(sizeof(MerkleNode));
    if (!node)
    {
        fprintf(stderr, "Erro ao alocar memória para o nó pai.\n");
        exit(EXIT_FAILURE);
    }

    char combined[2 * HASH_SIZE + 1]; // combinação dos hashes dos filhos
    snprintf(combined, sizeof(combined), "%s%s", left->hash, right->hash);

    calculate_hash(combined, node->hash);
    node->left = left;
    node->right = right;
    return node;
}

NodeList build_merkle_tree(char **transactions, int num_transactions)
{
    int num_nodes = num_transactions;
    MerkleNode **current_level = (MerkleNode **)malloc(num_nodes * sizeof(MerkleNode *));

    // cria nós folhas
    for (int i = 0; i < num_transactions; i++)
    {
        current_level[i] = create_leaf(transactions[i]);
    }

    // itera para construir a árvore
    while (num_nodes > 1)
    {
        int parent_count = (num_nodes + 1) / 2; // número de nós na próxima camada
        MerkleNode **parent_level = (MerkleNode **)malloc(parent_count * sizeof(MerkleNode *));

        for (int i = 0, j = 0; i < num_nodes; i += 2, j++)
        {
            MerkleNode *left = current_level[i];
            MerkleNode *right = (i + 1 < num_nodes) ? current_level[i + 1] : left; // duplicar último nó, se necessário
            parent_level[j] = create_parent(left, right);
        }

        free(current_level);
        current_level = parent_level;
        num_nodes = parent_count;
    }

    NodeList result = {.nodes = current_level, .size = 1};
    return result;
}

void free_merkle_tree(MerkleNode *root)
{
    if (!root)
        return;
    free_merkle_tree(root->left);
    free_merkle_tree(root->right);
    free(root);
}

void print_merkle_tree(MerkleNode *root, int level)
{
    if (!root)
        return;
    for (int i = 0; i < level; i++)
        printf(" ");
    printf("%s\n", root->hash);
    print_merkle_tree(root->left, level + 1);
    print_merkle_tree(root->right, level + 1);
}

int poi(MerkleNode *no, const char *incluido, char proofhash[][HASH_SIZE * 2 + 1], int *indice)
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
            strcpy(proofhash[*indice], no->right->hash);
            (*indice)++;
        }
        else
        {
            // coloca nada, não existe mais a direita, depois incrementa pro proximo indice.
            strcpy(proofhash[*indice], "");
            (*indice)++;
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
            strcpy(proofhash[*indice], no->left->hash);
            (indice)++;
        }
        else
        {
            // coloca nada, não existe mais a esquerda, depois incrementa pro proximo indice.
            strcpy(proofhash[*indice], "");
            (indice)++;
        }
        // encontrado na direita
        return 1;
    }
    // caso não exista nó.
    return 0;
}

Chain *novoBlockChain(int n)
{
    Chain *aux = malloc(sizeof(Chain) * n);

    if (!aux)
    {
        printf("Erro ao alocar memoria.");
        exit(1);
    }

    for (int i = 0; i < n; i++)
    {
        aux[i].timestamp = time(NULL);
        aux[i].indice = i;
        aux[i].nonceAtual = 0;
        aux[i].merkleTree.nodes = NULL;
        memset(aux[i].hashChainAtual, 0, HASH_SIZE);
        memset(aux[i].hashChainAnterior, 0, HASH_SIZE);
    }
    return aux;
}

void printBlocos(Chain *blockChain, int tam)
{
    // caso a blockChain exista e os nos nao sejam nulos
    if (blockChain)
    {
        for (int i = 0; i < tam; i++)
        {
            if (blockChain[i].merkleTree.nodes)
            {
                printf("Bloco de indice: %d\n", blockChain[i].indice);
                printf("Timestamp de criacao: %s\n", ctime(&blockChain[i].timestamp));
                printf("Hash do bloco: ");
                exibirHash(blockChain[i].hashChainAtual);
                printf("Hash do bloco anterior: ");
                exibirHash(blockChain[i].hashChainAnterior);
                printf("Hash das transacoes: \n");
                print_merkle_tree(*blockChain[i].merkleTree.nodes, 0);
                printf("\n");
            }
        }
    }
    else
    {
        printf("BlockChain vazia.");
        exit(1);
    }
}

int verificarIntegridade(Chain *blockChain, int indice, int tam)
{

    if (indice == 0)
        return 1;

    Chain aux = blockChain[indice];

    unsigned char reHash[HASH_SIZE];
    GerarHashNode(&aux, reHash);

    // compara o hash recalculado com o hash armazenado no bloco
    if (memcmp(aux.hashChainAtual, reHash, HASH_SIZE) != 0)
    {
        printf("Tentativa de fraude detectada no bloco %d\n", indice);
        return 0;
    }

    // compara o hash anterior do bloco subsequente ao bloco verificado com o seu hash
    if (indice < tam - 1)
    {
        if (memcmp(blockChain[indice + 1].hashChainAnterior, &aux.hashChainAtual, HASH_SIZE) != 0)
        {
            printf("Tentativa de fraude detectada entre os blocos %d e %d", indice, indice + 1);
        }
    }
    // integridade confirmada
    return 1;
}

Chain novoBloco(char **transacoes, int qtdTransacoes, int indice, int dificuldade, Chain *blockChain, int tam)
{
    Chain aux;
    aux.indice = indice;
    aux.timestamp = time(NULL);
    aux.merkleTree = build_merkle_tree(transacoes, qtdTransacoes);
    ProofOfWorkLinear(&aux, dificuldade);

    // se for o primeiro bloco, o hash anterior será nulo
    if (aux.indice == 0)
    {
        // defino o hash do bloco anterior como 0
        memset(aux.hashChainAnterior, 0, HASH_SIZE);
    }
    // se nao, sera o hash do bloco anterior
    else
    {
        // copia o hash do bloco anterior
        memcpy(aux.hashChainAnterior, blockChain[indice - 1].hashChainAtual, HASH_SIZE);
    }

    // verifica a autenticidade do novo bloco
    // if (!verificarIntegridade(blockChain, indice, tam))
    // {
    //     printf("O bloco não será adicionado.");
    //     free_merkle_tree(*aux.merkleTree.nodes);
    //     exit(1);
    // }
    return aux;
}

void existeTransacao(Chain *blockChain, char *transacao)
{

    char proofhash[100][HASH_SIZE * 2 + 1];
    char hashincluido[HASH_SIZE * 2 + 1];
    calculate_hash(transacao, hashincluido);
    int indicepow = 0;

    if (poi(blockChain[0].merkleTree.nodes[0], hashincluido, proofhash, &indicepow))
    {
        printf("transação encontrada no bloco %d!\n", blockChain[0].indice);
    }
    else
    {
        printf("transação não incluida no bloco %d!\n", blockChain[0].indice);
    }
}

void simularAtaque(Chain *blockchain, int numblocks, int indicealterado, char *transacao)
{
    if (indicealterado < 0 || indicealterado >= numblocks)
    {
        printf("indice invalido!");
        return;
    }
    if (blockchain[indicealterado].merkleTree.nodes != NULL)
    {
        // cast necessario por conta da chamada de blockchain sendo em **.
        calculate_hash(transacao, (char *)blockchain[indicealterado].merkleTree.nodes[0]->hash);

        clock_t inicio = clock();

        for (int i = indicealterado; i < numblocks; i++)
        {
            blockchain[i].nonceAtual = 0;                      // Reseta o nonce
            ProofOfWorkLinear(&blockchain[i], MAX_SEVERIDADE); // Recalcula o nonce
        }
        clock_t fim = clock();
        double tempo_gasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        printf("Tempo gasto para reajustar todos os nonces: %.2f segundos\n", tempo_gasto);
        printf("transação alterada com sucesso!");
    }
    else
    {
        printf("o bloco %d não possui transações para alterar", indicealterado);
        return;
    }
}

int main()
{
    Chain *blockChain = novoBlockChain(100);

    char *transacoes[] = {"t1", "t2", "t3"};

    blockChain[0] = novoBloco(transacoes, 3, 0, 4, blockChain, 100);

    existeTransacao(blockChain, "t1");

    char *transacoes2[] = {"t43", "t22", "t13"};

    blockChain[1] = novoBloco(transacoes, 3, 1, 4, blockChain, 100);

    char *transacoes3[] = {"t88", "t5", "t7"};

    blockChain[2] = novoBloco(transacoes3, 3, 2, 3, blockChain, 100);

    printBlocos(blockChain, 100);

    // simulacao do ataque
    simularAtaque(blockChain, 100, 1, "t4444");

    // verificando a blockChain apos o ataque
    printBlocos(blockChain, 100);

    return 0;
}