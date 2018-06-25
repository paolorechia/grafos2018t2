#include <stdlib.h>
#include <stdio.h>
#include "grafo.h"

//------------------------------------------------------------------------------

int main(void) {

  grafo g = le_grafo(stdin);

  if ( !g )

    return 1;


  int n = n_vertices(g);

  vertice *v = busca_lexicografica(vertice_de_nome((char *)"A", g), 
                                   g, 
                                   malloc((long unsigned int)n*sizeof(vertice)));

  printf("Foram usadas %d cores: ", colore(g, v));

  int penultimo = n - 1;

  for (int i = 0; i < penultimo; i++)

    printf("%d, ", cor(v[i], g));

  printf("%d\n\n", cor(v[penultimo], g));

  escreve_grafo(stdout, g);

  free(v);

  return ! destroi_grafo(g);
}
