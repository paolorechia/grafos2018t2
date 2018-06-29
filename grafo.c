#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphviz/cgraph.h>
#include "grafo.h"

#define TAM_ROTULO 100

/*************************************************/
/* Estrutura de dados auxiliar - Fila            */
/*                                               */
/* FIFO implementada sobre                       */
/* uma lista duplamente encadeada                */
/*                                               */
/* Implementada com uma operação extra:          */
/*  pop_maxlabel (remove o elemento de maior     */
/*  rótulo ao invés do primeiro da fila)         */
/*                                               */
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

/*************************************************/
/* Atributos internos dos vertices               */
/*************************************************/

typedef struct vertice_s{
  Agrec_t h;
  char rotulo[TAM_ROTULO];
  int estado;
  int cor;
} atrb_t; // define atributos do vertice


//static tqueue * q_init(void);
//Funçao que inicializa a lista duplamente encadeada utilizada como fila
static tqueue * q_init(){
    tqueue * queue = malloc(sizeof(tqueue));
    queue->start = NULL;
    queue->end = NULL;
    queue->end = queue->start;
    queue->size = 0;
    return queue;
}

//Funçao usada para inserir um elemento na fila
static void q_push(tqueue * queue, void * key){
    if (queue->size == 0){
      queue->start = malloc(sizeof(tnode));
      queue->start->data = key;
      queue->end = queue->start;
      queue->end->prev = NULL;
    }
    else{
      queue->end->next = malloc(sizeof(tnode));
      queue->end->next->prev = queue->end;
      queue->end = queue->end->next;
      queue->end->data = key;
    }
    queue->end->next = NULL;
    queue->size++;
    return;
}


// Funcao que retorna o primeiro da fila
static void * q_pop(tqueue * queue){
    if (queue->size == 0){
      return -1;
    }
    void *key;
    if (queue->size == 1){
      key = queue->start->data;
      free(queue->start);
      queue->start = NULL;
      queue->end = NULL;
    }
    else{
      key = queue->start->data;
      queue->start = queue->start->next;
      free(queue->start->prev);
      queue->start->prev = NULL;
    }
    queue->size--;
    return key;
}

static void * q_pop_maxlabel(tqueue * queue){
//    printf ("Queue size: %d\n", queue->size);
    if (queue->size == 0 || queue->start == NULL){
      return -1;
    }
    void *key;
    tnode * max_node;
    if (queue->size == 1){
      max_node = queue->start;
      queue->start = NULL;
      queue->end = NULL;
    }
    else{
      int max_label_size = 0;
      int label_size = 0;
      atrb_t * atrib;
      tnode * node = queue->start;
//      printf("Start:  %s\n", agnameof(node->data));
//      tnode * node = queue->start->next;
      while (node){
        atrib = (atrb_t *) aggetrec((Agnode_t *)node->data, "atrb_t", FALSE);
        label_size = strlen(atrib->rotulo);
//        printf("Node:  %s, label: %d\n", agnameof(node->data), label_size);
        if (label_size >= max_label_size){
          max_node = node;
          max_label_size = label_size;
        }
        node = node->next;
      }
//      printf("Found max: %s, label: %d\n", agnameof(max_node->data), max_label_size);
      // sanity check, not supposed to enter here
      if (max_node->prev != NULL && max_node->next !=NULL){
        max_node->next->prev = max_node->prev;
        max_node->prev->next = max_node->next;
      }
      // only prev is NULL
      else if (max_node->prev == NULL && max_node->next!=NULL){
        max_node->next->prev = NULL;
      }
      // only next is NULL
      else if (max_node->prev != NULL && max_node->next ==NULL){
        max_node->prev->next=NULL;
      }
    }
    if (max_node == queue->start){
      if (max_node->next != NULL){
        queue->start=queue->start->next;
      }
    }
    if (max_node == queue->end){
      if (max_node->prev != NULL){
        queue->end=queue->end->prev;
      }
    }
    key = max_node->data;
    free(max_node);
    queue->size--;
    return key;
}

// Funçao auxiliar para ajudar a debugar o programa.
// Casta dados para um vertice do cgraph
static void q_print_v(tqueue * queue){
    tnode * node = queue->start;
    while (node){
        Agnode_t * v = node->data;
        printf("%s ", agnameof(v));
        node = node->next;
    }
    putchar('\n');
}
static int q_size(tqueue * queue){
    return queue->size;
}

// Desaloca fila
static void q_free(tqueue * queue){
    tnode * node = queue->start;
    tnode * prev = NULL; 
    while (node){
      node = node->next;
      if (node){
        prev = node->prev;
        free(prev);
      }
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
// (apontador para) estrutura de dados para representar um vertice

typedef struct vertice * agnode_t;

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
  Agraph_t * graph = (Agraph_t *) g;
  Agnode_t * v;
/*
  void aginit(graph, int kind, char *rec_name,
              int rec_size, int move_to_front);
*/
  int i = 0;
  for (v = agfstnode(graph); v; v = agnxtnode(graph,v)){
    i++;
  }
  return i;
}

// devolve o vértice de nome 'nome' em g
vertice vertice_de_nome(char *nome, grafo g){
  return (vertice *) agnode((agraph_t*)g, nome, FALSE);
}

//------------------------------------------------------------------------------
// lê um grafo no formato dot de input
// 
// devolve o grafo lido,
//         ou 
//         NULL, em caso de erro 

grafo le_grafo(FILE *input) {

    Agraph_t *g;
    if ((g = agread(input, NULL))){

        return (grafo)g;
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
    g = (Agraph_t *) g;
    if (g = agwrite(g, output)){
        return (grafo) g;
    }
    else return NULL;
}
//------------------------------------------------------------------------------
// devolve um número entre 0 e o número de vertices de g

// Nao utilizamos o ponteiro para o grafo
unsigned int cor(vertice v, grafo g){
  Agnode_t * u = (Agnode_t *) v;
  atrb_t *  atrb = (atrb_t *) aggetrec(u, "atrb_t", FALSE);
  return atrb->cor;
}

// retorna vertice w da aresta {u, w} em G
static Agnode_t * vizinho(Agraph_t * g, Agnode_t * u, Agedge_t * e){
    if (!strcmp(agnameof(u), agnameof(aghead(e))))
        return agnode(g, agnameof(agtail(e)), FALSE);
    else
        return agnode(g, agnameof(aghead(e)), FALSE);
}

static int tam_vizinhanca(Agraph_t * g, Agnode_t * u, Agedge_t * e){
  int tam = 0;
  for (e = agfstedge(g,u); e; e = agnxtedge(g,e,u)){
    tam++;
  }
  return tam;
}


//------------------------------------------------------------------------------
// preenche o vetor v (presumidamente um vetor com n_vertices(g)
// posições) com os vértices de g ordenados de acordo com uma busca em
// largura lexicográfica sobre g a partir de r e devolve v

vertice * busca_lexicografica(vertice r, grafo g, vertice *v){
  Agraph_t * graph = (Agraph_t *)g;
  Agnode_t * u; 
  Agnode_t * w; 
  Agnode_t * raiz = (agnode_t *) r; 
  Agedge_t * e; 
  atrb_t * atributos_u;
  atrb_t * atributos_w;
  tqueue * V = q_init();
  int num_vertices_g = n_vertices(g);
  int i = 0;
  Agedge_t ** ordem_lex = malloc(sizeof(agnode_t) * num_vertices_g);
  char tam_V[16];

  // Inicializa vertices, monta conjunto inicial com todos os vertices
  for(u = agfstnode(graph); u; u = agnxtnode(graph, u)){
    atributos_u = (atrb_t *) agbindrec(u, "atrb_t", sizeof(atrb_t), FALSE);
    atributos_u->estado = 0;
    strcpy(atributos_u->rotulo, "");
    q_push(V, (void *) u);
  }
  
  // Define a raiz
  u = agnode(graph, agnameof(raiz), FALSE);
  atributos_u = (atrb_t *) agbindrec(u, "atrb_t", sizeof(atrb_t), FALSE);
  sprintf(tam_V, "%d", V->size);
  strcpy(atributos_u->rotulo,tam_V);

  // Inicia a busca
  while ((u = (Agnode_t * ) q_pop_maxlabel(V)) != -1){
    atributos_u = aggetrec(u, "atrb_t", FALSE);
    printf("%s %d\n", agnameof(u), atributos_u->estado);
    if (atributos_u->estado != 2){
      // registra quantos vertices ainda estao em V
      sprintf(tam_V, "%d", V->size);
      // Para cada w E vizinhanca(u)
      for (e = agfstedge(graph,u); e; e = agnxtedge(graph,e,u)){
          w = vizinho(graph, u, e);
          atributos_w = (atrb_t *) agbindrec(w, "atrb_t", sizeof(atrb_t), FALSE);
          if (atributos_w->estado != 2){
            strcpy(atributos_w->rotulo, agnameof(u));
            strcat(atributos_w->rotulo, tam_V);
            // marca como visitado (irrelevante aparentemente)
            atributos_w->estado = 1;
          }
      }
    }
    // marca u como buscado
    atributos_u->estado = 2;
    ordem_lex[i]=u;
    i++;
  }
  // preenche o vetor 'v' com o reverso da ordem lexografica
  int j = num_vertices_g -1;
//  printf("Preenchendo v[i]\n");
  for (int i = 0; i < num_vertices_g; i++){
    u = ordem_lex[j];
    atributos_u = (atrb_t *) aggetrec(u, "atrb_t", FALSE);
//    printf("Vertice: %s (%s)\n", agnameof(u), atributos_u->rotulo);
    v[i] = u;
    j--;
  }
  q_free(V);
  return (vertice *) v;
}

//------------------------------------------------------------------------------
// colore os vértices de g de maneira "gulosa" segundo a ordem dos
// vértices em v e devolve o número de cores utilizado
//
// ao final da execução,
//     1. cor(v,g) > 0 para todo vértice de g
//     2. cor(u,g) != cor(v,g), para toda aresta {u,v} de g

unsigned int colore(grafo g, vertice *v){
  Agnode_t * u;
  Agnode_t * w;
  Agedge_t * e;
  Agraph_t * graph = (Agraph_t*) g;
  Agsym_t * color = agattr(g, AGNODE, "color", "#000000");
  atrb_t * atrb;
  char * cores[] = {"#FF0000", "#00FF00", "#0000FF"};
  int cor_max = 0;
  int tam = n_vertices(g);
  int n_vizinhos;
  int i = 0;
  int j = 0;
  int * cores_ocupadas;
  int cor_minima = 1;
  int encontrei_cor = 0;

  /* Pinta todos os vertices com a cor 0 */
  for (i = 0; i < tam; i++){
    atrb = aggetrec(v[i], "atrb_t", FALSE);
    atrb->cor = 0;
    printf("Zerando cor: %s\n", agnameof(v[i]));
  }

  // Coloracao gulosa */
  for (i = 0; i < tam; i++){
    n_vizinhos = tam_vizinhanca(graph, v[i], e);
    cores_ocupadas = malloc(sizeof(int) * n_vizinhos); 
    j = 0;
    for (e = agfstedge(graph,v[i]); e; e = agnxtedge(graph,e,v[i])){
        w = vizinho(graph, v[i], e);
        cores_ocupadas[j]=cor(w, graph);
        j++;
    }
    j = 0;
    cor_minima = 1;
    encontrei_cor = 0;
    while (!encontrei_cor){
      encontrei_cor = 1;
      for (j = 0; j < n_vizinhos; j++){
        if (cor_minima == cores_ocupadas[j]){
          cor_minima++;  
          encontrei_cor = 0;
        }
      }
    }
    if (cor_minima > cor_max){
      cor_max = cor_minima;
    }
    free(cores_ocupadas);
    atrb = aggetrec(v[i], "atrb_t", FALSE);
    atrb->cor = cor_minima;
  }
  return cor_max;

  // Transforma atributos internos do grafo em cores externas //

//    agxset(v[i], cor, cores[i]);
}

//------------------------------------------------------------------------------

