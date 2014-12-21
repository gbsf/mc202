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

  int j = (m - 1) / 2;
  void *elem = h->vetor[m];

  while (m > 0 && h->comp(h->vetor[j], elem) < 0) {
    h->vetor[m] = h->vetor[j];
    m = j;
    j = (j - 1) / 2;
  }

  h->vetor[m] = elem;

} /* Sobe */

void Desce(ImplHeap h, int m) {

  int k = 2*m + 1;
  void *elem = h->vetor[m];

  while (k < h->tam) {
    if (k < h->tam - 1 && h->comp(h->vetor[k], h->vetor[k+1]) < 0)
      k++;

    if (h->comp(h->vetor[k], elem) > 0) {
      h->vetor[m] = h->vetor[k];
      m = k;
      k = 2*k + 1;
    } else {
      break;
    }
  }

  h->vetor[m] = elem;
 
} /* Desce */

Heap CriaHeapAux(int n, funcCompara *comp, void *elems[]) {
  /* Cria um heap vazio; se 'elems' não é nulo, preenche com os
     valores do vetor 'elems' e transforma num heap.
  */
  ImplHeap ih = (ImplHeap) MALLOC(sizeof(RegHeap)+(n-1)*sizeof(void *));

  ih->tamMax = n;
  ih->comp = comp;

  if (!elems) {
    ih->tam = 0;
  } else {
    int i;

    memcpy(ih->vetor, elems, n);
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
