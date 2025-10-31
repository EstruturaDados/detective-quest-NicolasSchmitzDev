/*
 * TRABALHO DE ESTRUTURA DE DADOS - DETECTIVE QUEST
 * Aluno: (Seu Nome de Calouro Aqui)
 * * Descrição: Este programa simula um jogo de detetive usando três
 * estruturas de dados principais:
 * 1. Uma Árvore Binária (para o mapa da mansão).
 * 2. Uma Árvore Binária de Busca (BST) (para armazenar pistas coletadas).
 * 3. Uma Tabela Hash (para ligar pistas a suspeitos).
 */

#include <stdio.h>
#include <stdlib.h> // Para malloc e free
#include <string.h> // Para strcmp e strcpy
#include <ctype.h>  // Para tolower

// --- CONSTANTES GLOBAIS ---
#define TAMANHO_HASH 10    // Tamanho da nossa tabela hash
#define TAMANHO_STRING 100 // Tamanho máximo para nomes e pistas

// --- ESTRUTURA 1: ÁRVORE DA MANSÃO (Árvore Binária Simples) ---

// Struct para um cômodo da mansão
typedef struct Sala
{
    char nome[TAMANHO_STRING];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

/**
 * @brief Aloca memória e cria um novo cômodo (nó) para a mansão.
 * * Esta função usa malloc para criar uma nova 'Sala', copia o nome
 * fornecido para a struct e define os filhos (esquerda/direita) como NULL.
 * * @param nome O nome do cômodo a ser criado.
 * @return Um ponteiro para a nova Sala criada.
 */
Sala *criarSala(const char *nome)
{
    // 1. Alocar memória
    Sala *novaSala = (Sala *)malloc(sizeof(Sala));

    // 2. Verificar se o malloc funcionou (boa prática de calouro!)
    if (novaSala == NULL)
    {
        printf("ERRO FATAL: Nao ha memoria suficiente! O jogo vai fechar.\n");
        exit(1); // Sai do programa se não houver memória
    }

    // 3. Configurar os dados
    strcpy(novaSala->nome, nome); // Copia o nome
    novaSala->esquerda = NULL;    // Ninguém à esquerda ainda
    novaSala->direita = NULL;     // Ninguém à direita ainda

    // 4. Retornar o ponteiro
    return novaSala;
}

// --- ESTRUTURA 2: PISTAS COLETADAS (Árvore Binária de Busca - BST) ---

// Struct para um nó da árvore de pistas
typedef struct NoPista
{
    char pista[TAMANHO_STRING];
    struct NoPista *esquerda;
    struct NoPista *direita;
} NoPista;

/**
 * @brief Insere uma nova pista na Árvore Binária de Busca (BST).
 * * A função insere a pista em ordem alfabética. Ela usa recursão
 * para encontrar o local certo. Se a pista já existir, ela não
 * é duplicada.
 * * @param raiz O nó raiz atual da BST de pistas.
 * @param pista O texto da pista a ser inserida.
 * @return O ponteiro para a raiz da árvore (pode ser a mesma ou uma nova).
 */
NoPista *adicionarPista(NoPista *raiz, const char *pista)
{
    // Caso Base: Se a árvore (ou sub-árvore) está vazia, crie o nó aqui
    if (raiz == NULL)
    {
        NoPista *novoNo = (NoPista *)malloc(sizeof(NoPista));
        if (novoNo == NULL)
        {
            printf("ERRO DE MEMORIA ao adicionar pista!\n");
            exit(1);
        }
        strcpy(novoNo->pista, pista);
        novoNo->esquerda = NULL;
        novoNo->direita = NULL;
        printf("   [Pista '%s' coletada e guardada na BST!]\n", pista);
        return novoNo; // Esta é a nova raiz
    }

    // Caso Recursivo: A árvore não está vazia, decidir para onde ir
    int comparacao = strcmp(pista, raiz->pista);

    if (comparacao < 0)
    {
        // A nova pista vem antes (alfabeticamente), vai para a esquerda
        raiz->esquerda = adicionarPista(raiz->esquerda, pista);
    }
    else if (comparacao > 0)
    {
        // A nova pista vem depois, vai para a direita
        raiz->direita = adicionarPista(raiz->direita, pista);
    }
    // else (comparacao == 0): A pista já existe, não faz nada.

    // Retorna a raiz (que não mudou)
    return raiz;
}

// Função auxiliar para mostrar as pistas em ordem (In-Order Traversal)
void mostrarPistas(NoPista *raiz)
{
    if (raiz == NULL)
    {
        return; // Parada da recursão
    }
    mostrarPistas(raiz->esquerda);      // Mostra tudo da esquerda primeiro
    printf("   - %s\n", raiz->pista); // Mostra o nó atual
    mostrarPistas(raiz->direita);     // Mostra tudo da direita por último
}

// --- ESTRUTURA 3: TABELA HASH (Pista -> Suspeito) ---

// Item da tabela hash (usando encadeamento para colisões)
typedef struct ItemHash
{
    char chavePista[TAMANHO_STRING];
    char valorSuspeito[TAMANHO_STRING];
    struct ItemHash *proximo; // Ponteiro para o próximo item em caso de colisão
} ItemHash;

// A própria Tabela Hash (um array de ponteiros para listas ligadas)
typedef struct TabelaHash
{
    ItemHash *baldes[TAMANHO_HASH];
} TabelaHash;

// Função Hash Simples (estilo calouro: soma os valores ASCII)
unsigned int funcaoHash(const char *chave)
{
    unsigned long hash = 0;
    int i = 0;
    while (chave[i] != '\0')
    {
        hash += (unsigned char)chave[i];
        i++;
    }
    return hash % TAMANHO_HASH; // Modulo pelo tamanho da tabela
}

// Inicializa a tabela hash (coloca tudo como NULL)
void inicializarTabela(TabelaHash *tabela)
{
    for (int i = 0; i < TAMANHO_HASH; i++)
    {
        tabela->baldes[i] = NULL;
    }
}

/**
 * @brief Insere uma associação (Pista, Suspeito) na tabela hash.
 * * Calcula o índice hash da pista (chave). Cria um novo ItemHash
 * e o insere no início da lista ligada (balde) correspondente
 * a esse índice.
 * * @param tabela Ponteiro para a TabelaHash.
 * @param pista A chave (string da pista).
 * @param suspeito O valor (string do suspeito).
 */
void inserirNaHash(TabelaHash *tabela, const char *pista, const char *suspeito)
{
    // 1. Calcular o índice usando a função hash
    unsigned int indice = funcaoHash(pista);

    // 2. Criar o novo item
    ItemHash *novoItem = (ItemHash *)malloc(sizeof(ItemHash));
    if (novoItem == NULL)
    {
        printf("ERRO DE MEMORIA ao criar item hash!\n");
        exit(1);
    }
    strcpy(novoItem->chavePista, pista);
    strcpy(novoItem->valorSuspeito, suspeito);

    // 3. Inserir na tabela (método de encadeamento simples)
    // O 'proximo' do novo item aponta para quem estava no balde antes
    novoItem->proximo = tabela->baldes[indice];
    // O balde agora aponta para o novo item (que virou o primeiro da lista)
    tabela->baldes[indice] = novoItem;
}

/**
 * @brief Encontra um suspeito na tabela hash usando a pista como chave.
 * * Calcula o índice hash da pista. Percorre a lista ligada (balde)
 * nesse índice procurando pela chave exata (pista).
 * * @param tabela Ponteiro para a TabelaHash.
 * @param pista A chave (pista) que queremos consultar.
 * @return O nome do suspeito (string) se encontrado, ou "Desconhecido" se não.
 */
const char *encontrarSuspeito(TabelaHash *tabela, const char *pista)
{
    // 1. Calcular o índice
    unsigned int indice = funcaoHash(pista);

    // 2. Procurar na lista (balde)
    ItemHash *atual = tabela->baldes[indice];
    while (atual != NULL)
    {
        // Compara a pista procurada com a chave do item atual
        if (strcmp(atual->chavePista, pista) == 0)
        {
            return atual->valorSuspeito; // Encontrou!
        }
        atual = atual->proximo; // Vai para o próximo da lista
    }

    // 3. Não encontrou
    return "Desconhecido";
}

// --- LÓGICA PRINCIPAL DO JOGO ---

// Função "hardcoded" que define qual pista está em qual sala
// (Como pedido na simplificação)
const char *buscarPistaDaSala(const char *nomeSala)
{
    if (strcmp(nomeSala, "Biblioteca") == 0)
    {
        return "Carta Rasgada";
    }
    if (strcmp(nomeSala, "Cozinha") == 0)
    {
        return "Faca com Manchas";
    }
    if (strcmp(nomeSala, "Sala de Jantar") == 0)
    {
        return "Copo Quebrado";
    }
    if (strcmp(nomeSala, "Escritorio") == 0)
    {
        return "Testamento Aberto";
    }
    if (strcmp(nomeSala, "Quarto de Hospedes") == 0)
    {
        return "Passaporte Falso";
    }
    // Outras salas (Hall, Despensa) não têm pistas
    return NULL;
}

/**
 * @brief Controla a navegação do jogador pela mansão.
 * * Esta é a função principal do loop do jogo. Ela mostra a sala atual,
 * verifica se há uma pista, coleta a pista (chamando adicionarPista)
 * e pergunta ao jogador para onde ir (esquerda, direita ou sair).
 * * @param salaAtual A sala onde o jogador começa (a raiz da árvore da mansão).
 * @param raizPistas Um ponteiro para o ponteiro da raiz da BST de pistas (para que
 * possamos modificá-la).
 * @param tabelaSuspeitos A tabela hash com as associações pista/suspeito.
 */
void explorarSalas(Sala *salaAtual, NoPista **raizPistas, TabelaHash *tabelaSuspeitos)
{
    char escolha = ' ';

    printf("\n--- EXPLORACAO DA MANSAO ---\n");
    printf("Comandos: (e)squerda, (d)ireita, (s)air da investigacao\n");

    while (escolha != 's')
    {
        printf("\n============================================\n");
        printf("Voce esta em: %s\n", salaAtual->nome);

        // 1. Verificar se há pista na sala
        const char *pistaEncontrada = buscarPistaDaSala(salaAtual->nome);
        if (pistaEncontrada != NULL)
        {
            printf("Voce encontrou uma pista: %s!\n", pistaEncontrada);
            // 2. Adicionar pista na BST
            //    (Passamos o endereço de 'raizPistas' para que a função
            //     'adicionarPista' possa mudar para onde 'raizPistas' aponta)
            *raizPistas = adicionarPista(*raizPistas, pistaEncontrada);
        }
        else
        {
            printf("Voce olha ao redor, mas nao encontra nada suspeito.\n");
        }

        // 3. Perguntar a ação do jogador
        printf("\nPara onde ir? (e, d, s): ");
        scanf(" %c", &escolha); // O espaço antes do %c é importante!
                              // Ele "come" o <Enter> da jogada anterior.

        escolha = tolower(escolha); // Converte para minúscula

        // 4. Mover o jogador
        if (escolha == 'e')
        {
            if (salaAtual->esquerda != NULL)
            {
                salaAtual = salaAtual->esquerda; // Move para a esquerda
            }
            else
            {
                printf("--> Nao ha passagem para a esquerda daqui.\n");
            }
        }
        else if (escolha == 'd')
        {
            if (salaAtual->direita != NULL)
            {
                salaAtual = salaAtual->direita; // Move para a direita
            }
            else
            {
                printf("--> Nao ha passagem para a direita daqui.\n");
            }
        }
        else if (escolha == 's')
        {
            printf("\nVoce decide que ja viu o suficiente e vai para o escritorio central...\n");
        }
        else
        {
            printf("--> Comando invalido, detetive!\n");
        }
    }
}

// Função auxiliar (recursiva) para contar quantas pistas COLETADAS apontam para o suspeito
int contarPistasParaSuspeito(NoPista *raizPistas, TabelaHash *tabela, const char *suspeitoAcusado)
{
    // Caso base: se não há mais pistas para checar
    if (raizPistas == NULL)
    {
        return 0;
    }

    int contagem = 0;
    // Verifica a pista atual
    const char *suspeitoDaPista = encontrarSuspeito(tabela, raizPistas->pista);
    if (strcmp(suspeitoDaPista, suspeitoAcusado) == 0)
    {
        contagem = 1; // Esta pista aponta para o acusado!
    }

    // Soma a contagem atual com a contagem da esquerda e da direita (recursão)
    return contagem +
           contarPistasParaSuspeito(raizPistas->esquerda, tabela, suspeitoAcusado) +
           contarPistasParaSuspeito(raizPistas->direita, tabela, suspeitoAcusado);
}

/**
 * @brief Conduz a fase final de julgamento.
 * * Esta função é chamada após o jogador sair da exploração.
 * Ela lista todas as pistas coletadas (usando a BST).
 * Pergunta ao jogador quem é o culpado.
 * Usa a Tabela Hash para verificar cada pista e contar quantas
 * apontam para o acusado.
 * Finalmente, exibe se a acusação foi um sucesso (>= 2 pistas).
 * * @param raizPistas A raiz da BST de pistas coletadas.
 * @param tabelaSuspeitos A tabela hash.
 */
void verificarSuspeitoFinal(NoPista *raizPistas, TabelaHash *tabelaSuspeitos)
{
    printf("\n\n--- FASE DE JULGAMENTO ---\n");

    // 1. Verificar se tem pistas
    if (raizPistas == NULL)
    {
        printf("Voce nao coletou NENHUMA pista.\n");
        printf("VEREDITO: INCONCLUSIVO. O caso foi arquivado por falta de provas.\n");
        return;
    }

    // 2. Listar as pistas coletadas (em ordem)
    printf("Voce revisa seu caderno de anotacoes. Pistas coletadas:\n");
    mostrarPistas(raizPistas);

    // 3. Perguntar ao jogador
    char acusado[TAMANHO_STRING];
    printf("\nDetetive, quem e o culpado? (Ex: Mordomo, Governanta, Sobrinho)\n");
    printf("Sua acusacao: ");
    
    // Limpa o buffer de entrada antes de ler a string
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    
    // Lê a linha inteira (permite nomes com espaço, tipo "Tia Zilda")
    // O calouro talvez usasse só scanf("%s", acusado), mas isso quebraria
    // com nomes compostos. fgets é melhor.
    fgets(acusado, TAMANHO_STRING, stdin);
    // Remove o '\n' que o fgets deixa no final
    acusado[strcspn(acusado, "\n")] = 0;


    printf("\nAnalisando as provas contra '%s'...\n", acusado);

    // 4. Contar as provas
    int numProvas = contarPistasParaSuspeito(raizPistas, tabelaSuspeitos, acusado);

    printf("Voce encontrou %d pista(s) que aponta(m) para '%s'.\n", numProvas, acusado);

    // 5. Dar o Veredito
    if (numProvas >= 2)
    {
        printf("\nVEREDITO: CULPADO!\n");
        printf("As provas sao contundentes. Voce resolveu o misterio! Parabens, detetive!\n");
    }
    else
    {
        printf("\nVEREDITO: INOCENTE!\n");
        printf("Voce acusou a pessoa errada ou nao tinha provas suficientes. O verdadeiro culpado escapou...\n");
    }
}

// --- FUNÇÃO PRINCIPAL ---

int main()
{
    printf("Bem-vindo ao Detective Quest: O Misterio da Mansao Binaria\n");
    printf("==========================================================\n");

    // --- SETUP FASE 1: Criar a Tabela Hash de Suspeitos ---
    // (Isto define quem é o culpado por cada pista)
    TabelaHash tabelaSuspeitos;
    inicializarTabela(&tabelaSuspeitos);

    // Inserindo as "respostas" do mistério na tabela
    inserirNaHash(&tabelaSuspeitos, "Carta Rasgada", "Sobrinho");
    inserirNaHash(&tabelaSuspeitos, "Faca com Manchas", "Mordomo");
    inserirNaHash(&tabelaSuspeitos, "Copo Quebrado", "Governanta");
    inserirNaHash(&tabelaSuspeitos, "Testamento Aberto", "Sobrinho");
    inserirNaHash(&tabelaSuspeitos, "Passaporte Falso", "Governanta");

    printf("O banco de dados de suspeitos foi carregado.\n");

    // --- SETUP FASE 2: Montar o Mapa da Mansão (Árvore Binária) ---
    // (Como pedido, mapa fixo montado na main)

    //       Hall
    //      /    \
    //  Jantar   Biblioteca
    //   /         /     \
    // Cozinha   Escritorio  Hospedes
    //   \
    //  Despensa

    Sala *hall = criarSala("Hall de Entrada");
    hall->esquerda = criarSala("Sala de Jantar");
    hall->direita = criarSala("Biblioteca");

    hall->esquerda->esquerda = criarSala("Cozinha");
    hall->esquerda->esquerda->direita = criarSala("Despensa"); // Sala sem pista

    hall->direita->esquerda = criarSala("Escritorio");
    hall->direita->direita = criarSala("Quarto de Hospedes");

    printf("O mapa da mansao foi carregado. A investigacao comeca agora.\n");

    // --- SETUP FASE 3: Inicializar a Árvore de Pistas Coletadas ---
    NoPista *pistasColetadas = NULL; // Começa vazia

    // --- INICIAR O JOGO ---
    // Passamos o endereço de 'pistasColetadas' (&pistasColetadas)
    explorarSalas(hall, &pistasColetadas, &tabelaSuspeitos);

    // --- INICIAR O JULGAMENTO ---
    verificarSuspeitoFinal(pistasColetadas, &tabelaSuspeitos);

    // --- LIMPEZA DE MEMÓRIA ---
    // (Um bom calouro tentaria limpar a memória, mas um calouro
    //  real provavelmente esqueceria ou faria errado. Vamos pular
    //  as funções complexas de 'free' recursivo para manter
    //  o foco no que foi pedido.)

    printf("\nFim de jogo.\n");
    return 0;
}
