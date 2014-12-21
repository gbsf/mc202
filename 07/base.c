/*
  Autor:         Gabriel Souza Franco
  RA:            155477
  Disciplina:    MC202
  Turma:         F
  
  Tarefa 07 
  Segundo semestre de 2012

*/

/*
 * Arquivo base.c: implementa��o das fun��es de manipula��o da base.
 */

#include <stdio.h>
#include "base.h"
#include "balloc.h"

typedef struct AuxNoArv {
  Aluno info;                   /* registros de informa��o */
  struct AuxNoArv *esq,*dir;   /* sub�rvores */
  } NoArv, * ImplBase;
  

Base CriaBase() {
/* Devolve apontador para uma base vazia */

  return NULL;
  
}

Boolean InsereBase(Base *p, Aluno a) {
/* Insere o registro 'a' na base 'p' se n�o existe aluno
   com o mesmo valor de 'ra', e devolve 'true';  caso
   contr�rio devolve 'false' */
  
  register ImplBase *b = (ImplBase*)p;

  if (*b == NULL) {
    /* Inserir aqui */
    *b = (NoArv*) MALLOC(sizeof(NoArv));
    (*b)->info = a;
    (*b)->esq = NULL;
    (*b)->dir = NULL;

    return true;
  }

  if (a.ra == (*b)->info.ra) {
    /* J� existe aluno com este RA */
    return false;
  }

  if (a.ra < (*b)->info.ra)
    return InsereBase((Base*) &((*b)->esq), a);
  else
    return InsereBase((Base*) &((*b)->dir), a);

} /* InsereBase */
   
Boolean ConsultaBase(Base *p, int ra, Aluno *a) {
/* Devolve 'true' se existe um registro com 'ra' dado na
   base 'p';  caso contr�rio devolve 'false'. 'a' conter� 
   os dados do aluno, se encontrado. */

  register ImplBase b = *(ImplBase*)p;

  if (b == NULL) {
    /* Chegamos a uma sub�rvore de uma folha.
     * Portanto o valor n�o est� na base */
    return false;
  }

  if (ra == b->info.ra) {
    *a = b->info;
    return true;
  }

  if (ConsultaBase((Base*) &(b->esq), ra, a))
    return true;
  if (ConsultaBase((Base*) &(b->dir), ra, a))
    return true;

  /* N�o est� nem na s.a. esquerda nem na direita */
  return false;
  
} /* ConsultaBase */

Boolean RemoveBase(Base *p, int ra) {
/* Devolve 'true' se um registro com 'ra' p�de ser removido da
   base 'p';  caso contr�rio devolve 'false'. */

  register ImplBase *b = (ImplBase*)p;

  if ((*b) == NULL) {
    /* N�o h� nada a remover aqui */
    return false;
  }

  if (ra < (*b)->info.ra)
    return RemoveBase((Base*) &(*b)->esq, ra);
  if (ra > (*b)->info.ra)
    return RemoveBase((Base*) &(*b)->dir, ra);

  /* |ra == (*b).info.ra| a partir desse ponto */

  /* Casos f�ceis */

  if ((*b)->esq == NULL) {
    register NoArv *no = *b;
    (*b) = (*b)->dir;
    FREE(no->info.nome);
    FREE(no);

    return true;
  }
  
  if ((*b)->dir == NULL) {
    register NoArv *no = *b;
    (*b) = (*b)->esq;
    FREE(no->info.nome);
    FREE(no);

    return true;
  }

  /* Caso dif�cil, requer substitui��o de n�s */

  register ImplBase *alvo = &((*b)->dir);

  while ((*alvo)->esq != NULL)
    alvo = &((*alvo)->esq);

  /* Substituir o valor de |*b| por |*alvo| */
  FREE((*b)->info.nome);
  (*b)->info = (*alvo)->info;

  /* Remover a posi��o |*alvo| */
  register NoArv *no = *alvo;
  (*alvo) = (*alvo)->dir;
  FREE(no);

  return true;

} /* RemoveBase */

int AlturaBaseRec(Base *p, int nivel) {
/* Fun��o auxiliar para calcular a altura da �rvore,
 * onde |*p| possui n�vel |nivel + 1| */

  register ImplBase b = *(ImplBase*)p;

  /* Se |b| � nulo, retornar o n�vel do pai */
  if (b == NULL)
    return nivel;
  
  /* Calcula a altura da �rvore considerando somente a 
   * sub�rvore esquerda */
  int e = AlturaBaseRec((Base*) &(b->esq), nivel + 1);
  /* Idem para a direita */
  int d = AlturaBaseRec((Base*) &(b->dir), nivel + 1);

  /* Retorna a maior altura encontrada */
  return e > d ? e : d;

}

int AlturaBase(Base *p) {
/* Devolve a altura da base 'p'. */
  
  return AlturaBaseRec(p, 0);

} /* AlturaBase */

void NumeroNosBaseRec(Base *p, int *contador) {
/* Incrementa |*contador| para cada n� visitado */
  
  register ImplBase b = *(ImplBase*)p;

  while (b != NULL) {
    NumeroNosBaseRec((Base*) &(b->esq), contador);
    (*contador)++;
    b = b->dir;
  }

}

int NumeroNosBase(Base *p) {
/* Devolve o n�mero de n�s da base 'p'. */

  int contador = 0;

  NumeroNosBaseRec(p, &contador);

  return contador;

} /* NumeroNosBase */


void PercorreBase(Base *p, void (*Visita)(Aluno*)) {
/* Executa um percurso inordem na base, invocando a fun��o Visita
   para todos os elementos. */

  register ImplBase b = *(ImplBase*)p;

  while (b != NULL) {
    PercorreBase((Base*) &(b->esq), Visita);
    Visita(&(b->info));
    b = b->dir;
  }

}   /* PercorreBase */

void LiberaBase(Base *p) {
/* Libera todos os n�s da base apontada por 'p', bem 
   como todas as cadeias que guardam os nomes. */

  register ImplBase b = *(ImplBase*)p;

  if (b != NULL) {
    LiberaBase((Base*) &(b->esq));
    LiberaBase((Base*) &(b->dir));

    FREE(b->info.nome);
    FREE(b);
  }

} /* LiberaBase */
