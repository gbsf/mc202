/*
  Implementação das funções de manipulação de filas de prioridade
  (FP). Vide o arquivo heap.h com as especificações.
 */  

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include "balloc.h"

typedef struct {
  int tamMax;
  int tam;
  funcCompara *comp;/* função de comparação */
  void *vetor[1];   /* tamanho alocado será 'tamMax' */
} RegHeap, *ImplHeap;


/* Funções auxiliares para manipulação de FPs. */
void Sobe(ImplHeap h, int m) {

  /* Posição do pai de |m| */
  int j = (m - 1) / 2;
  /* Guardar o elemento que está subindo */
  void *elemento = h->vetor[m];

  /* Enquanto |m| está dentro do vetor e
   * |j| é mais prioritário que o elemento */
  while (m > 0 && h->comp(h->vetor[j], elemento) < 0) {
    /* Copiar |vetor[j]| para |vetor[m]| e recalcular o pai de |j| */
    h->vetor[m] = h->vetor[j];
    m = j;
    j = (j - 1) / 2;
  }

  /* |m| está na posição correta, copiar o elemento */
  h->vetor[m] = elemento;

} /* Sobe */

void Desce(ImplHeap h, int m) {

  /* |k| é o índice do filho esquerdo da posição |m| */
  int k = 2*m + 1;
  /* Guardar o valor que se está descendo */
  void *elemento = h->vetor[m];

  /* Checar se o valor é válido */
  while (k < h->tam) {
    /* Se o filho direito for mais prioritário que
     * o esquerdo, usar o direito */
    if (k < h->tam - 1 && h->comp(h->vetor[k], h->vetor[k+1]) < 0)
      k++;

    /* Se |k| for menos prioritário que o elemento, copiar |vetor[k]|
     * para |vetor[m]| e recalcular a posição dos filhos de |k| */
    if (h->comp(h->vetor[k], elemento) > 0) {
      h->vetor[m] = h->vetor[k];
      m = k;
      k = 2*k + 1;
    } else {
      break;
    }
  }

  /* |m| está na posição correta, copiar o valor salvo */
  h->vetor[m] = elemento;
 
} /* Desce */

Heap CriaHeapAux(int n, funcCompara *comp, void *elems[]) {
  /* Cria um heap vazio; se 'elems' não é nulo, preenche com os
     valores do vetor 'elems' e transforma num heap.
  */
  ImplHeap ih = (ImplHeap) MALLOC(sizeof(RegHeap)+(n-1)*sizeof(void *));

  ih->tamMax = n;
  /* Guardar a função de comparação */
  ih->comp = comp;

  if (!elems) {
    /* Não há elementos iniciais */
    ih->tam = 0;
  } else {
    int i;

    /* Copiar os elementos do vetor para a heap */
    memcpy(ih->vetor, elems, n * sizeof(void*));
    ih->tam = n;

    /* Transformar numa heap */
    for (i = (ih->tam - 2) / 2; i >= 0; i--)
      Desce(ih, i);
  }
  
  return ih;
  
} /* CriaHeapAux */

Heap CriaHeap(int n, funcCompara *comp) {

  return CriaHeapAux(n,comp,NULL);

} /* CriaHeap */

Heap CriaInicializaHeap(int n, funcCompara *comp, void *elems[]) {

  return CriaHeapAux(n,comp,elems);

} /* CriaInicializaHeap */

int TamanhoHeap(Heap h) {

  return ((ImplHeap) h)->tam;
  
} /* TamanhoHeap */


void InsereHeap(Heap h, void *e) {

  ImplHeap ih = (ImplHeap) h;
  if (ih->tam==ih->tamMax) {
    printf("Estouro da FP\n");
    exit(0);
  }

  /* Adicionar o elemento no final do vetor
   * e realizar a operação de subida nele */
  ih->vetor[ih->tam] = e;
  ih->tam++;
  Sobe(ih, ih->tam - 1);
 
} /* insereHeap */


void * RemoveHeap(Heap h) {

  ImplHeap ih = h;
  void *ret = NULL;
  if (ih->tam==0) {
    printf("FP vazia\n");
    exit(0);
  }

  /* Remover o primeiro elemento da heap,
   * substituindo-o pelo último elemento,
   * e fazendo este descer até seu lugar
   * correto no heap (potencialmente
   * diferente do lugar inicial */
  ret = ih->vetor[0];
  ih->tam--;
  ih->vetor[0] = ih->vetor[ih->tam];
  Desce(ih, 0);

  return ret;
  
} /* removeHeap */

void LiberaHeap(Heap h) {

  FREE(h);

} /* liberaHeap */


void * Elemento(Heap h, int k) {

  ImplHeap ih = (ImplHeap) h;
  
  if ((k<0) || (k>=ih->tam))
    return NULL;
  return ih->vetor[k];

} /* Elemento */
