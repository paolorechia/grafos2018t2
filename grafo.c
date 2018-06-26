#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphviz/cgraph.h>
#include "grafo.h"


/*************************************************/
/* Estrutura de dados auxiliar - Fila            */
/*                                               */
/* FIFO implementada sobre                       */
/* uma lista duplamente encadeada                */
/*************************************************/
typedef struct cel_struct{
    void * data;              // ponteiro generico para dados
    struct cel_struct * prev; // ponteiro para anterior da fila
    struct cel_struct * next; // ponteiro para proximo da fila
} tnode;

typedef struct queue{
    tnode * start;            // ponteiro para primeiro elemento da fila
    tnode * end;              // ponteiro para ultimo elemento da fila
    int size;
} tqueue;

//Funçao que inicializa a lista duplamente encadeada utilizada como fila
static tqueue * q_init(){
    tqueue * queue = malloc(sizeof(tqueue));
    queue->start = malloc(sizeof(tnode));
    queue->start->prev = NULL;
    queue->start->next = NULL;
    queue->end = queue->start;
    queue->size = 0;
    return queue;
}

//Funçao usada para inserir um elemento na fila
static void q_push(tqueue * queue, void * key){
    queue->end->next = malloc(sizeof(tnode));
    queue->end->next->prev = queue->end;
    queue->end = queue->end->next;
    queue->end->data = key;
    queue->end->next = NULL;
    return;
}

// Funcao que retorna o primeiro da fila
static void * q_pop(tqueue * queue){
    void * key = queue->start->data;
    queue->start = queue->start->next;
    free(queue->start->prev);
    queue->start->prev = NULL;
    return;
}

//Funçao auxiliar para ajudar a debugar o programa.
static void q_print(tqueue * queue){
    tnode * node = queue->start;
    while (node){
        printf("%s ", node->data);
        node = node->next;
    }
    putchar('\n');
}

static int q_size(tqueue * queue){
    return queue->size;
}

// Desaloca fila
static int q_clear(tqueue * queue){
    tnode * node = queue->start;
    tnode * prev = NULL; 
    while (node){
      node = node->next;
      prev = node->prev;
      free(prev);
    }
    free(queue);
}
/*************************************************/
/* Fim da estrutura de dados fila                */
/*************************************************/

//------------------------------------------------------------------------------
// (apontador para) estrutura de dados para representar um grafo
// 
// o grafo pode ser direcionado ou não
// 
// o grafo tem um nome, que é uma "string"

typedef grafo * agraph_t;

//------------------------------------------------------------------------------
// desaloca toda a memória usada em *g
// 
// devolve 1 em caso de sucesso,
//         ou 
//         0, caso contrário

int destroi_grafo(grafo g) {
    return agclose((Agraph_t *)g);
}
//------------------------------------------------------------------------------
// devolve o número de vértices de g
int n_vertices(grafo g){
  return 0;
}

//------------------------------------------------------------------------------
// lê um grafo no formato dot de input
// 
// devolve o grafo lido,
//         ou 
//         NULL, em caso de erro 

grafo le_grafo(FILE *input) {

    Agraph_t *g;
    if (g = agread(input, NULL)){

        return g;
    }
    else return NULL;
}

//------------------------------------------------------------------------------
// escreve o grafo g em output usando o formato dot.
//
// devolve o grafo escrito,
//         ou 
//         NULL, em caso de erro 

grafo escreve_grafo(FILE *output, grafo g) {
    if (g = agwrite(g, output)){
        return g;
    }
    else return NULL;
}
//------------------------------------------------------------------------------
// devolve um número entre 0 e o número de vertices de g

unsigned int cor(vertice v, grafo g);


//------------------------------------------------------------------------------
// preenche o vetor v (presumidamente um vetor com n_vertices(g)
// posições) com os vértices de g ordenados de acordo com uma busca em
// largura lexicográfica sobre g a partir de r e devolve v

vertice *busca_lexicografica(vertice r, grafo g, vertice *v){


}

//------------------------------------------------------------------------------
// colore os vértices de g de maneira "gulosa" segundo a ordem dos
// vértices em v e devolve o número de cores utilizado
//
// ao final da execução,
//     1. cor(v,g) > 0 para todo vértice de g
//     2. cor(u,g) != cor(v,g), para toda aresta {u,v} de g

unsigned int colore(grafo g, vertice *v){


}

//------------------------------------------------------------------------------

