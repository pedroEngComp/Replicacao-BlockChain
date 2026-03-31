#include "StructChain.h"


//ISSO É UM PONTO C APENAS PARA TESTE ISOLADO


//função para calcular o hash de uma string
void calculate_hash(const char* input, char* output) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)input, strlen(input), digest);

    //converte o hash para uma string hexadecimal
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", digest[i]);
    }
    output[HASH_SIZE] = '\0'; //finaliza a string
}

//criação de um nó folha
MerkleNode* create_leaf(const char* data) {
    MerkleNode* node = (MerkleNode*)malloc(sizeof(MerkleNode));
    if (!node) {
        fprintf(stderr, "Erro ao alocar memória para o nó folha.\n");
        exit(EXIT_FAILURE);
    }
    calculate_hash(data, node->hash);
    node->left = NULL;
    node->right = NULL;
    return node;
}

//criação de um nó pai com base em dois nós filhos
MerkleNode* create_parent(MerkleNode* left, MerkleNode* right) {
    MerkleNode* node = (MerkleNode*)malloc(sizeof(MerkleNode));
    if (!node) {
        fprintf(stderr, "Erro ao alocar memória para o nó pai.\n");
        exit(EXIT_FAILURE);
    }

    char combined[2 * HASH_SIZE + 1]; //combinação dos hashes dos filhos
    snprintf(combined, sizeof(combined), "%s%s", left->hash, right->hash);

    calculate_hash(combined, node->hash);
    node->left = left;
    node->right = right;
    return node;
}

//construção da Merkle Tree


NodeList build_merkle_tree(char** transactions, int num_transactions) {
    int num_nodes = num_transactions;
    MerkleNode** current_level = (MerkleNode**)malloc(num_nodes * sizeof(MerkleNode*));

    //cria nós folhas
    for (int i = 0; i < num_transactions; i++) {
        current_level[i] = create_leaf(transactions[i]);
    }

    //itera para construir a árvore
    while (num_nodes > 1) {
        int parent_count = (num_nodes + 1) / 2; //número de nós na próxima camada
        MerkleNode** parent_level = (MerkleNode**)malloc(parent_count * sizeof(MerkleNode*));

        for (int i = 0, j = 0; i < num_nodes; i += 2, j++) {
            MerkleNode* left = current_level[i];
            MerkleNode* right = (i + 1 < num_nodes) ? current_level[i + 1] : left; //duplicar último nó, se necessário
            parent_level[j] = create_parent(left, right);
        }

        free(current_level);
        current_level = parent_level;
        num_nodes = parent_count;
    }

    NodeList result = { .nodes = current_level, .size = 1 };
    return result;
}

//função para liberar memória da Merkle Tree
void free_merkle_tree(MerkleNode* root) {
    if (!root) return;
    free_merkle_tree(root->left);
    free_merkle_tree(root->right);
    free(root);
}

//exibição da Merkle Tree (em pré-ordem)
void print_merkle_tree(MerkleNode* root, int level) {
    if (!root) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf("%s\n", root->hash);
    print_merkle_tree(root->left, level + 1);
    print_merkle_tree(root->right, level + 1);
}

int main() {
    char* transactions[] = {"T1", "T2", "T3", "T4"};
    int num_transactions = sizeof(transactions) / sizeof(transactions[0]);

    NodeList result = build_merkle_tree(transactions, num_transactions);
    printf("Merkle Root: %s\n\n", result.nodes[0]->hash);

    printf("Merkle Tree:\n");
    print_merkle_tree(result.nodes[0], 0);

    free_merkle_tree(result.nodes[0]);
    free(result.nodes);
    return 0;
}
