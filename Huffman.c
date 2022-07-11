/*
Nome: Patrick Utzig Haselof
Matrícula: 1910985
Nome: Thiago Luis Becker da Rocha
Matrícula: 2110449
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ASCII 256

typedef struct node
{
    char caracter;
    struct node *esq;
    struct node *dir;
    struct node *prox;
    int freq;

} No;

typedef struct list
{
    No *ini;
    int tam;
} List;

// TAD

int sizeTotalFile(char *path);                                                  // retorna o tamanho do arquivo
void getFileContent(char *text, char *path);                                    // uma string do arquivo inteiro
void counter(char textArq[], int table[]);                                      // conta a frequencia de caracteres
void insert(List *lista, No *no);                                               // insere um novo node ordenado na lista
void fillList(int *table, List *lista);                                         // preenche a lista com os nos
void printlist(List *lista);                                                    // imprime a lista
No *huffmanTree(List *lista);                                                   // cria a arvore de huffman
int treeheight(No *raiz);                                                       // retorna a altura da arvore
void createdictionary(char **dicionario, No *raiz, char *caminho, int colunas); // gera o dicionario
void printdictionary(char **dicionario);                                        // imprime o dicionario
char *codify(char **dicionario, char *texto);                                   // retorna uma string do conteudo codificado seguindo o dicionario
void zipHuffman(char *str, char *path);                                         // compacta o arquivo a partir da string codificada
void unzipHuffman(No *raiz, char *path);                                        // descompacta o arquivo a partir da arvore de prefixos

int main(void)
{
    List lista; // lista de nos
    No *arvore; // arvore de huffman
    int colunas, tam, i, *table; 
    char *codified, *texto, **dicionario; 

    tam = sizeTotalFile("Trem_de_Alagoas.txt"); // pega o tamanho do arquivo

    texto = malloc((tam + 1) * sizeof(char)); // aloca memoria para a string do arquivo
    getFileContent(texto, "Trem_de_Alagoas.txt"); // pega o conteudo do arquivo

    table = malloc(sizeof(int) * ASCII); // aloca memoria para a tabela de frequencia
    counter(texto, table); // conta a frequencia de caracteres

    lista.ini = NULL; //inicializa a lista
    lista.tam = 0; //inicializa o tamanho da lista

    fillList(table, &lista); // preenche a lista com os nos

    printf("Lista:\n");
    printlist(&lista);
    printf("\n");

    arvore = huffmanTree(&lista); // cria a arvore de huffman

    colunas = treeheight(arvore) + 1; // pega a quantidade de colunas da arvore
    dicionario = malloc(sizeof(char *) * ASCII); // aloca memoria para o dicionario de prefixos

    for (i = 0; i < ASCII; i++)
        dicionario[i] = malloc(colunas * sizeof(char)); // aloca memoria para cada linha do dicionario

    createdictionary(dicionario, arvore, "", colunas); // gera o dicionario

    printf("Dicionario:\n");
    printdictionary(dicionario);
    printf("\n");

    codified = codify(dicionario, texto); // codifica o texto em uma string

    zipHuffman(codified, "Trem_de_Alagoas_compactado.bin"); // compacta o arquivo

    printf("texto traduzido com o dicionario:\n"
           "%s"
           "\n\n",
           codified);
    printf("Tamanho do arquivo: %d Bytes\n", tam);
    printf("tamanho do codificado: %ld Bytes\n\n", strlen(codified) / 8);

    float taxa = ((float)strlen(codified) / 8) / tam;
    printf("taxa de compactacao: %.2f%%\n", (taxa - 1) * -100);

    unzipHuffman(arvore, "Trem_de_Alagoas_compactado.bin"); // descompacta o arquivo
    printf("Arquivo descompactato e .txt gerado\n");

    for (i = 0; i < ASCII; i++)
    {
        free(dicionario[i]);
    }

    free(dicionario);
    free(codified);
    free(texto);
    free(table);

    return 0;
}

int sizeTotalFile(char *path)
{
    FILE *arquivo = fopen(path, "r");
    int size = 0;
    char c;

    if (arquivo)
    {
        while (fscanf(arquivo, "%c", &c) != EOF)
            size++;
    }
    fclose(arquivo);
    return size;
}

void getFileContent(char *text, char *path)
{
    FILE *arquivo = fopen(path, "r");
    int i = 0;
    char c;

    while (fscanf(arquivo, "%c", &c) != EOF)
        text[i++] = c;
    fclose(arquivo);
}

void counter(char *textArq, int *table)
{
    for (int i = 0; i < ASCII; i++)
    {
        table[i] = 0;
    }
    for (int i = 0; i < strlen(textArq); i++)
    {
        table[textArq[i]]++;
    }
}

void insert(List *lista, No *no) 
{
    No *aux = lista->ini;
    No *ant = NULL;
    while (aux != NULL && aux->freq < no->freq)
    {
        ant = aux;
        aux = aux->prox;
    }
    if (ant == NULL)
    {
        no->prox = lista->ini;
        lista->ini = no;
    }
    else
    {
        ant->prox = no;
        no->prox = aux;
    }
    lista->tam++;
}

void fillList(int *table, List *lista)
{
    for (int i = 0; i < ASCII; i++)
    {
        if (table[i] != 0)
        {
            No *no = (No *)malloc(sizeof(No));
            no->caracter = i;
            no->freq = table[i];
            no->esq = NULL;
            no->dir = NULL;
            no->prox = NULL;
            insert(lista, no);
        }
    }
}

void printlist(List *lista)
{
    int i = 0;

    No *aux1 = lista->ini;
    while (aux1 != NULL)
    {
        aux1 = aux1->prox;

        i++;
    }

    int v[i];
    char c[i];

    int j = 0;

    No *aux2 = lista->ini;
    while (aux2 != NULL)
    {
        c[j] = aux2->caracter;
        v[j] = aux2->freq;
        aux2 = aux2->prox;

        j++;
    }

    for (int k = i - 1; k > 0; k--)
    {
        printf("[%c] = %d\n", c[k], v[k]);
    }
}

No *huffmanTree(List *lista)
{
    No *no1, *no2, *no3; // nós auxiliares

    while (lista->tam > 1) // enquanto a lista tiver mais de um no
    {
        no1 = lista->ini; // pega o primeiro no da lista
        lista->ini = lista->ini->prox; // atualiza o inicio da lista
        lista->tam--; // atualiza o tamanho da lista
        no2 = lista->ini; // pega o segundo no da lista
        lista->ini = lista->ini->prox; // atualiza o inicio da lista
        lista->tam--; // atualiza o tamanho da lista
        no3 = (No *)malloc(sizeof(No)); // aloca memoria para o novo no
        no3->freq = no1->freq + no2->freq; // atualiza a frequencia do novo no
        no3->esq = no1; // atualiza o filho esquerdo do novo no
        no3->dir = no2; // atualiza o filho direito do novo no
        insert(lista, no3); // insere o novo no na lista
    }
    return lista->ini; // retorna o no raiz da arvore
}

int treeheight(No *raiz)
{
    if (raiz == NULL)
    {
        return 0;
    }
    int h_esq = treeheight(raiz->esq);
    int h_dir = treeheight(raiz->dir);
    if (h_dir > h_esq)
    {
        return h_esq + 1;
    }
    else
    {
        return h_dir + 1;
    }
}

void createdictionary(char **dicionario, No *raiz, char *caminho, int colunas)
{
    char esquerda[colunas], direita[colunas]; // strings auxiliares

    if (raiz->esq == NULL && raiz->dir == NULL) // se o no for folha
    {
        strcpy(dicionario[raiz->caracter], caminho); // copia o caminho para o dicionario
    }
    if (raiz->esq != NULL) // se o no tiver filho esquerdo
    {
        strcpy(esquerda, caminho); // copia o caminho para a variavel esquerda
        strcat(esquerda, "0"); // concatena o 0 ao caminho
        createdictionary(dicionario, raiz->esq, esquerda, colunas); // chama a funcao recursivamente
    }
    if (raiz->dir != NULL) // se o no tiver filho direito
    {
        strcpy(direita, caminho); // copia o caminho para a variavel direita
        strcat(direita, "1"); // concatena o 1 ao caminho
        createdictionary(dicionario, raiz->dir, direita, colunas); // chama a funcao recursivamente
    }
}

void printdictionary(char **dicionario)
{
    int i = 0;
    while (i < ASCII)
    {
        if (dicionario[i][0] != '\0')
        {
            printf("[%c] = %s\n", i, dicionario[i]);
        }
        i++;
    }
}

char *codify(char **dicionario, char *texto)
{
    int i = 0, size;
    char *codified;
    for (i = 0; i < strlen(texto); i++) // percorre o texto
    {
        size += strlen(dicionario[texto[i]]); // atualiza o tamanho da string codificada
    }
    codified = (char *)malloc(sizeof(char) * size); // aloca memoria para a string codificada

    for (i = 0; i < strlen(texto); i++) 
    {
        strcat(codified, dicionario[texto[i]]); // concatena a string codificada
    }
    return codified; // retorna a string codificada
}

void zipHuffman(char str[], char *path)
{
    FILE *arq = fopen(path, "wb");
    int i = 0;
    unsigned char aux, byte = 0;
    while (str[i] != '\0') // percorre o texto
    {
        aux = str[i]; 
        if (aux == '0') 
        {
            byte = byte << 1; // shifta o byte para a esquerda
        }
        else
        {
            byte = byte << 1; // shifta o byte para a esquerda
            byte = byte | 1; // seta o bit
        }
        i++; 
        if (i % 8 == 0) // se o byte estiver completo
        {
            fwrite(&byte, sizeof(char), 1, arq); // escreve o byte no arquivo
            byte = 0; // zera o byte
        }
    }
    fclose(arq);
}

void unzipHuffman(No *root, char *path)
{
    FILE *arquivo = fopen(path, "rb");
    FILE *arquivoOrigem = fopen("Trem_de_AlagoasDescomp.txt", "w");
    No *aux = root;
    unsigned char byte;
    int i;

    while (fread(&byte, sizeof(char), 1, arquivo)) // le o arquivo byte a byte
    {
        i = 7; 
        while (i > -1) // percorre o byte
        {
            if (byte & (1 << i)) // se o bit for 1
            {
                aux = aux->dir; // anda para o filho direito
            }
            else
            {
                aux = aux->esq; // anda para o filho esquerdo
            }
            if (aux->esq == NULL && aux->dir == NULL) //se for folha
            {
                fputc(aux->caracter, arquivoOrigem); // escreve o caracter no arquivo de origem
                aux = root; // volta para a raiz
            }
            i--; 
        }
    }
    fclose(arquivo);
    fclose(arquivoOrigem);
}
