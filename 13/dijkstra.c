/*
  Autor:         Gabriel Souza Franco
  RA:            155477
  Disciplina:    MC202
  Turmas:        F
  
  Tarefa: 13
  Segundo semestre de 2014

*/

/* A especificação das funções encontra-se no arquivo dijkstra.h */

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "dijkstra.h"
#include "balloc.h"

typedef enum {false, true} Boolean;

/* Aresta */
typedef struct aux {
  int dest;
  int peso;
  struct aux *prox;
} Aresta;

/* Implementação de grafo */
typedef struct {
  int tam;               /* número de vértices */
  Aresta * vertices[1];  /* tamanho final será 'tam' */
} RegGrafo, *ImplGrafo;


Grafo CriaGrafoVazio(int n) {

  ImplGrafo ig = (ImplGrafo) MALLOC(sizeof(RegGrafo) + (n-1)*sizeof(Aresta*));
  ig->tam = n;
  /* Limpar a memória do vetor de listas de arestas */
  memset(ig->vertices, 0, n * sizeof(Aresta*));

  return (Grafo) ig;

} /* CriaGrafoVazio */

Erro AcrescentaAresta(Grafo g, int u, int v, int p) {

  ImplGrafo ig = (ImplGrafo) g;
  Aresta *novo;
  Aresta **aresta;

  /* Checar se os vértices estão dentro dos limites */
  if ((u < 0 || u >= ig->tam) || (v < 0 || v >= ig->tam))
    return ERRADO;

  /* Checar se o peso é positivo */
  if (p < 0)
    return ERRADO;

  /* Pegar a primeira aresta desse vértice */
  aresta = &ig->vertices[u];

  /* Avançar na lista de arestas enquanto a atual tiver
   * vértice de destino menor que a aresta a ser criada */
  while (*aresta && (*aresta)->dest < v)
    aresta = &(*aresta)->prox;

  /* Checar se a aresta a ser criada não existe ainda */
  if (*aresta && (*aresta)->dest == v)
    return ERRADO;

  /* Criar a aresta */
  novo = (Aresta*) MALLOC(sizeof(Aresta));
  novo->dest = v;
  novo->peso = p;
  novo->prox = *aresta;

  /* Ligar a aresta */
  *aresta = novo;

  return CERTO;

} /* AcrescentaAresta */

void ImprimeGrafo(Grafo g) {

  ImplGrafo ig = g;
  int i, n;
  Aresta *v;

  n = ig->tam;
  printf("Grafo de %d vértices:\n",n);
  for (i=0; i<n; i++) {
    printf("%4d:",i);
    v = ig->vertices[i];
    while (v!=NULL) {
      printf("%4d (%d)",v->dest,v->peso);
      v = v->prox;
    }
    printf("\n");
  }

} /* ImprimeGrafo */

int menorIndice(int dist[], Boolean visitado[], int tam) {
/* Retorna o índice do elemento em |dist|, que ainda não
 * foi visitado, que possui o menor peso. Caso não
 * haja elemento válido, retorna -1 */

  int menor = 0;

  /* Passar para o primeiro elemento não visitado */
  while (menor < tam && visitado[menor])
    menor++;

  if (menor == tam)
    /* Não há elemento válido */
    return -1;

  /* Todos os elementos antes de |menor| já foram
   * visitados, então começar a comparar pelo próximo */
  for (int i = menor + 1; i < tam; i++)
    if (!visitado[i] && dist[menor] > dist[i])
      menor = i;

  return menor;

}

void Dijkstra(Grafo g, int r, int dist[]) {

  ImplGrafo ig = (ImplGrafo) g;
  const int tam = ig->tam;
  Boolean visitado[tam];
  int indice_atual;

  /* Inicializar a memória como false (0) */
  memset(visitado, 0, sizeof visitado);
  /* Inicializar os pesos mínimos como "infinito" (INT_MAX) */
  for (int i = 0; i < tam; i++)
    dist[i] = INT_MAX;
  /* O nó de origem tem peso 0 (ele para ele mesmo) */
  dist[r] = 0;

  /* Começar pela origem */
  indice_atual = r;

  do {
    /* Início da lista de arestas */
    Aresta *aresta = ig->vertices[indice_atual];

    for (; aresta != NULL; aresta = aresta->prox) {
      int peso = dist[indice_atual] + aresta->peso;
      /* Se o peso por esse caminho for menor que o
       * peso registrado, atualizar o mesmo */
      if (peso < dist[aresta->dest])
        dist[aresta->dest] = peso;
    }

    /* Marcar o vértice atual como processado */
    visitado[indice_atual] = true;

    /* Calcula o próximo elemento a ser visitado */
    indice_atual = menorIndice(dist, visitado, tam);
  } while (indice_atual != -1 && dist[indice_atual] != INT_MAX);
  /* Se |dist[indice_atual] == INT_MAX|, então todos os vértices
   * restantes são inacessíveis a partir da origem r */

  /* O vetor |dist| agora contém os pesos dos caminhos mínimos
   * à partir de r. Não há memória dinâmica a ser liberada */

} /* Dijkstra */


void LiberaGrafo(Grafo g) {

  ImplGrafo ig = (ImplGrafo) g;
  Aresta *atual, *prox;
  /* Guardar o tamanho para não ter de
   * acessar o ponteiro toda vez */
  const int tam = ig->tam;
  int i;

  /* Para cada um dos vértices */
  for (i = 0; i < tam; i++) {
    /* Passar pela lista de arestas, liberando-as */
    atual = ig->vertices[i];
    while (atual) {
      prox = atual->prox;
      FREE(atual);
      atual = prox;
    }
  }

  /* Liberar a estrutura do grafo */
  FREE(ig);

} /* LiberaGradfo */
