# ⛓️ Replicação de Blockchain em C (Reupload)

Implementação educacional das principais estruturas e mecanismos de uma Blockchain, desenvolvida em **linguagem C** com uso da biblioteca **OpenSSL** para criptografia SHA-256.

Creditos a github.com/CarlosDiasS e github.com/Anisioengcomp pelo trabalho em conjunto.

---

## 📋 Sumário

- [Sobre o Projeto](#sobre-o-projeto)
- [Conceitos Implementados](#conceitos-implementados)
- [Estrutura dos Arquivos](#estrutura-dos-arquivos)
- [Estruturas de Dados](#estruturas-de-dados)
- [Como Compilar e Executar](#como-compilar-e-executar)
- [Exemplo de Execução](#exemplo-de-execução)
- [Dependências](#dependências)

---

## Sobre o Projeto

Este projeto foi desenvolvido como trabalho acadêmico da disciplina de **Algoritmos e Estruturas de Dados (AEDS)** e replica, em linguagem C, os mecanismos fundamentais que sustentam tecnologias como o Bitcoin.

O objetivo é demonstrar na prática como uma Blockchain garante **imutabilidade** e **integridade** dos dados, simulando inclusive um ataque de fraude e medindo o custo computacional necessário para reescrever a cadeia.

---

## Conceitos Implementados

### 🔗 Blockchain
Cadeia de blocos encadeados onde cada bloco armazena o hash do bloco anterior, tornando qualquer alteração detectável. Cada bloco contém:
- Índice e timestamp de criação
- Hash do bloco atual e do bloco anterior
- Nonce utilizado na mineração
- Raiz da Árvore de Merkle com as transações

### 🌳 Árvore de Merkle
Estrutura de árvore binária onde cada folha representa o hash de uma transação e cada nó pai é o hash da concatenação dos seus filhos. Permite verificar a presença de uma transação sem percorrer toda a cadeia.

### ⛏️ Proof of Work (PoW) Linear
Algoritmo de consenso que exige que o hash do bloco comece com um número determinado de zeros, controlado pelo parâmetro `dificuldade` (de 1 a 4). O nonce é incrementado até que essa condição seja satisfeita.

### ✅ Proof of Inclusion (PoI)
Prova criptográfica que verifica se uma transação específica está contida em um bloco, percorrendo o caminho da folha até a raiz da Árvore de Merkle e coletando os hashes irmãos ao longo do caminho.

### 🔍 Verificação de Integridade
Função que recomputa o hash de um bloco e o compara com o hash armazenado, detectando qualquer tentativa de adulteração dos dados.

### 💥 Simulação de Ataque
Simula um ataque real: altera uma transação em um bloco específico e recalcula todos os nonces subsequentes com dificuldade máxima (`MAX_SEVERIDADE = 4`), medindo o tempo gasto — demonstrando o custo computacional de fraudar uma blockchain.

---

## Estrutura dos Arquivos

```
Replicacao-BlockChain/
├── StructChain.h               # Definição das structs e protótipos de todas as funções
├── Blockchain.c                # Implementação principal: blocos, PoW, Merkle, PoI, ataque
├── GerarHash.c                 # Funções auxiliares de geração de hash com OpenSSL
├── MerkleTree.c                # Construção e manipulação da Árvore de Merkle
├── ProofOfWordAlg.c            # Algoritmo de Proof of Work
├── funçaoproofofinclusion.c    # Algoritmo de Proof of Inclusion
├── teste                       # Binário compilado de teste
└── Relatório_Trabalho_AEDS_1.pdf  # Relatório acadêmico do projeto
```

---

## Estruturas de Dados

### `MerkleNode` — Nó da Árvore de Merkle
```c
typedef struct MerkleNode {
    unsigned char hash[HASH_SIZE + 1]; // Hash SHA-256 do nó
    struct MerkleNode *left;           // Filho esquerdo
    struct MerkleNode *right;          // Filho direito
    time_t timestampTransacao;
} MerkleNode;
```

### `Chain` — Bloco da Blockchain
```c
typedef struct chain {
    int indice;                              // Posição na cadeia
    time_t timestamp;                        // Momento de criação
    unsigned char hashChainAtual[HASH_SIZE]; // Hash deste bloco
    unsigned char hashChainAnterior[HASH_SIZE]; // Hash do bloco anterior
    long nonceAtual;                         // Nonce minerado
    NodeList merkleTree;                     // Árvore de Merkle das transações
} Chain;
```

### Constantes
| Constante | Valor | Descrição |
|---|---|---|
| `HASH_SIZE` | 32 | Tamanho do hash em bytes (SHA-256) |
| `MAX_SEVERIDADE` | 4 | Dificuldade máxima do Proof of Work |
| `MAX_NUMEROPROOFS` | 100 | Número máximo de nós no caminho de prova |

---

## Como Compilar e Executar

### Pré-requisitos

- GCC (compilador C)
- Biblioteca OpenSSL instalada

**Linux/macOS:**
```bash
sudo apt install libssl-dev   # Ubuntu/Debian
brew install openssl          # macOS
```

**Windows:** Instale o [OpenSSL para Windows](https://slproweb.com/products/Win32OpenSSL.html) e use o MinGW ou WSL.

### Compilação

```bash
gcc Blockchain.c -o blockchain -lssl -lcrypto -lm
```

### Execução

```bash
./blockchain
```

---

## Exemplo de Execução

O `main()` em `Blockchain.c` demonstra o fluxo completo:

```
# Criação de 3 blocos com transações e dificuldade 4
hash minerado com sucesso em 38291 iterações.
hash minerado com sucesso em 52104 iterações.
hash minerado com sucesso em 19873 iterações.

# Verificação de transação via Proof of Inclusion
transação encontrada no bloco 0!

# Exibição dos blocos
Bloco de indice: 0
Timestamp de criacao: Tue Mar 31 10:00:00 2026
Hash do bloco: 0000a3f2...
Hash do bloco anterior: 0000000000000000...

# Simulação de ataque no bloco 1
Tempo gasto para reajustar todos os nonces: 4.73 segundos
transação alterada com sucesso!
```

---

## Dependências

| Biblioteca | Uso |
|---|---|
| `openssl/evp.h` | Geração de hash SHA-256 via EVP API |
| `openssl/sha.h` | Hash SHA-256 direto para a Árvore de Merkle |
| `stdlib.h` / `string.h` | Alocação de memória e manipulação de strings |
| `time.h` | Timestamps dos blocos e transações |
| `math.h` | Cálculos auxiliares |

---

*Projeto desenvolvido como trabalho acadêmico de AEDS — Algoritmos e Estruturas de Dados.*
