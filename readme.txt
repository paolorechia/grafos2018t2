------------------------------------------------------------
UFPR  - DINF
CI065 - ALGORITMOS E TEORIA DOS GRAFOS - 2018-1
------------------------------------------------------------

Prof. Renato Carmo
Alunos: Paolo Andreas Stall Rechia - GRR 20135196
        Luiz ...                   - GRR 


------------------------------------------------------------

Tópicos:

1. Busca lexográfica
1.1 Pseudo-código
1.2 Detalhes da implementação
1.3 Eficiência da implementação da busca lexográfica.

------------------------------------------------------------


1. Busca lexográfica

1.1 Pseudo-código

  lex(G, r, vet):

    V <- fila modificada vazia
    Para cada v E V(G):
      v.cor = 0
      v.visitado= 0
      v.rotulo= "" 
      V.push(v)

    r.rotulo = |V(G)|
    Enquanto (V != vazia)
      u = pop_maxlabel(V)
      Se (u.visitado != 2)
        Para cada w E vizinhanca(u)
        Se (w.visitado != 2)
          w.rotulo = u.rotulo + |V(G)|
          w.visitado = 1
      u.visitado = 2


1.2 Detalhes da implementação

    Implementamos uma busca lexográfica sobre um Grafo
  utilizando uma fila modificada.
    A única modificação sobre essa fila é uma operação nova,
  'pop_maxlabel', que em vez de retirar o elemento mais
  antigo da fila, retira aquele de maior rótulo (string de 
  maior comprimento).
    Na prática, poderíamos ter implementado diretamente a
  lista duplamente encadeada porém isso só foi constatado
  após a fila estar 100% implementada.

    O tamanho do maior rótulo possível foi definido em '1000'
  em macro, ou seja, o algoritmo não funcionaria para grafos
  grandes.
  
1.3 Eficiência da implementação da busca lexográfica.


  O algoritmo consiste em basicamente:
    i) inserir todos os elementos na fila (n * O(1)).
    ii) retirar todos os elementos da fila (n * custo(pop_maxlabel))
    iii) concatenar rotulos de u em w quando w não foi visitado (n*custo_concat)

    A operação 'pop_maxlabel'  é cara, e exige uma busca
  sobre todos os elementos da fila O(n). Como a operação é
  chamada exatamente n vezes, temos um custo O(n^2).

    A concatenação tem um custo considerável, porque os rótulos crescem
  rapidamente e no pior caso também custa O(n) quando é o último rótulo.

    Assim, o algoritmo tem um custo total aproximado entre O(n^2) a O(n^3):
      n*custo(pop_maxlabel)*custo_concat = n *O(n) * O(n) = O(n^3)

  O pop_maxlabel é o principal gargalo de eficiência.

  De fato, encontramos vários artigos que descrevem uma versao
  desse algoritmo de tempo linear (ou quase linear: nlgn), utilizando
  'partition refinement'. Substitui-se a fila por uma estrutura de conjunto, 
  implementada com varias listas duplamente encadeadas, uma delas com
  o conjunto inteiro e particiona-se o conjunto em varias listas (subconjuntos).
  
  Os detalhes exatos não foram compreendidos, por isso optamos pela implementação
  menos eficiente.
