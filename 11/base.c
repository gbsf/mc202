/*
  Autor:         Gabriel Souza Franco
  RA:            155477
  Disciplina:    MC202
  Turmas:        F
  
  Tarefa 11
  Segundo semestre de 2014

*/

#include <stdio.h>
#include <string.h>
#include "base.h"

/* DECLARAÇÕES DE TIPOS */
/* -------------------- */

typedef struct RegLista *Lista;
typedef struct RegLista {
  Aluno aluno;
  Lista prox;
} RegLista;

typedef struct RegBase *ImplBase;
typedef struct RegBase {
  int numregs;
  Lista tabela[MaxHash];
} RegBase;

/* FUNÇÃO DE ESPALHAMENTO */
/* ---------------------- */

int Espalha(int k) {
  return (k%MaxHash);
} /* Espalha */

/* FUNÇÕES AUXILIARES */
/* ------------------ */

/* Funções de manipulação de listas */

Boolean InsereLista(Lista *p, Aluno *a) {
/* Insere o elemento |*a| na posição correta na lista */

  /* Achar a posição correta */
  while (*p && (*p)->aluno.ra < a->ra)
    /* Copiar o endereço do campo, ao invés de atribuir no local
     * apontado por |p|: |*p = (*p)->prox| */
    p = &(*p)->prox;

  if (*p && (*p)->aluno.ra == a->ra)
    return false; /* Já existe */

  RegLista *elem = MALLOC(sizeof *elem);

  /* Inserir na posição encontrada */
  elem->aluno.ra = a->ra;
  elem->aluno.nome = a->nome;

  elem->prox = *p;
  *p = elem;

  return true;

}

Boolean RemoveLista(Lista *p, int ra) {
/* Remove o elemento |*a|, se existir */

  /* Achar a posição correta */
  while (*p && (*p)->aluno.ra < ra)
    p = &(*p)->prox;

  if (!(*p) || (*p)->aluno.ra != ra)
    return false; /* Não ocorre na lista */

  Lista tmp = *p;

  /* Desligar o elemento */
  *p = (*p)->prox;

  FREE(tmp->aluno.nome);
  FREE(tmp);

  return true;

}

Boolean ConsultaLista(Lista p, int ra, Aluno *a) {
/* Retorna |true| com os dados do aluno de RA |ra| na
 * variável |a|, se ele existir.
 * Caso contrário retorna |false| */

  /* Achar a posição correta */
  while (p && p->aluno.ra < ra)
    p = p->prox;

  if (!p || p->aluno.ra != ra)
    return false; /* Não ocorre na lista */

  *a = p->aluno;

  return true;

}

void LiberaLista(Lista p) {
/* Libera toda a memória ocupada pela lista */

  Lista temp;

  while (p) {
    /* Copiar o apontador de próximo antes de liberar o elemento */
    temp = p->prox;

    FREE(p->aluno.nome);
    FREE(p);

    p = temp;
  }

}

/* FUNÇÕES DA INTERFACE */
/* -------------------- */

Base CriaBase() {
/* Devolve apontador para uma base vazia com tabela inicializada */

  ImplBase base = MALLOC(sizeof *base);
  base->numregs = 0;
  /* |base->numregs| é um array estático, então
   * |sizeof base->numregs| retorna o tamanho do mesmo */
  memset(base->tabela, 0, sizeof base->tabela);

  return (Base) base;

}

Boolean InsereBase(Base p, Aluno *a) {
/* Insere o registro 'a' na base 'p' se não existe aluno
   com o mesmo valor de 'ra', e devolve 'true';  caso
   contrário devolve 'false' */

  ImplBase ib = (ImplBase) p;
  int posicao = Espalha(a->ra);

  if (InsereLista(&(ib->tabela[posicao]), a)) {
    ib->numregs++; /* Inserção com sucesso, aumentar número de nós */
    return true;
  }
  return false;

}
   
Boolean RemoveBase(Base p, int ra) {
/* Remove da base 'p' o aluno com 'ra' dado; devolve 'true'
   se ele existiu e 'false' caso contrário. */

  ImplBase ib = (ImplBase) p;
  int posicao = Espalha(ra);

  if (RemoveLista(&(ib->tabela[posicao]), ra)) {
    ib->numregs--; /* Remoção com sucesso, diminuir número de nós */
    return true;
  }
  return false;

}

Boolean ConsultaBase(Base p, int ra, Aluno *a) {
/* Devolve 'true' se existe um registro com 'ra' dado na
   base 'p';  caso contrário devolve 'false'.  'a' conterá 
   os dados do aluno, se encontrado. */

  ImplBase ib = (ImplBase) p;
  int posicao = Espalha(ra);

  return ConsultaLista(ib->tabela[posicao], ra, a);

}
   
int NumeroRegsBase(Base p) {
/* Devolve o número de registros (alunos) na base 'p'. */

  return ((ImplBase) p)->numregs;

}

void ImprimeBase(Base p) {
/* Imprime, os registros contidos na base 'p', um por linha.  A ordem
   de impressão segue a ordem das entradas da tabela, e para cada
   entrada, a ordem da lista ligada.  Cada linha começa com o índice
   correspondente na tabela de espalhamento. Deve ser usado o formato
   "(%3d) %06d %s\n". */

  ImplBase ib = (ImplBase) p;
  /* |sizeof ib->tabela| é o tamanho total da tabela.
   * |sizeof ib->tabela[0]| é o tamanho de um elemento.
   * Portanto |max| contém o número de elementos em |ib->tabela|.
   */
  const int max = sizeof ib->tabela / sizeof ib->tabela[0];
  Lista no_lista;

  for (int i = 0; i < max; i++) {
    no_lista = ib->tabela[i];
    while (no_lista) {
      printf("(%3d) %06d %s\n", i, no_lista->aluno.ra, no_lista->aluno.nome);
      no_lista = no_lista->prox;
    }
  }

}
   
void LiberaBase(Base p) {
/* Libera toda a memória utilizada pela base 'p', bem como
   todas as cadeias que guardam os nomes. */

  ImplBase ib = (ImplBase) p;
  /* Número de elementos em |ib->tabela|.
   * Nesse caso, sempre é igual a |MaxHash|, mas
   * dessa forma não é necessário ter esse
   * conhecimento à princípio */
  const int max = sizeof ib->tabela / sizeof ib->tabela[0];

  for (int i = 0; i < max; i++)
    LiberaLista(ib->tabela[i]);

  FREE(ib);

}

