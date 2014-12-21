/*
  Autor:         Gabriel Souza Franco
  RA:            155477
  Disciplina:    MC202
  Turma:         F
  
  Tarefa 08 
  Segundo semestre de 2014

*/

/*
 * Arquivo base.c: implementação das funções de manipulação da base.
 */

#include <stdio.h>
#include "base.h"
#include "balloc.h"

#include "imprimearvore.h"

/* Remove avisos de compilação referentes à variáveis não utilizadas */
#ifdef __GNUC__
# define UNUSED(x) x##_UNUSED __attribute__((unused))
#else
# define UNUSED(x) x
#endif

typedef struct AuxNoArv {
  Aluno info;                  /* registros de informação */
  struct AuxNoArv *esq,*dir;   /* subárvores */
  short int bal;               /* fator de balanceamento */
} NoArv, * ImplBase;

/* ============================================================ */

/* Função ImprimeBase para produzir a representação gráfica */
/* Pode ajudar na depuração */

char cadeiainfo[7] = "0000000"; /* conterá os dígitos do RA */
char *vazia = "Árvore vazia\n";

ImplBase esq(ImplBase a, int UNUSED(d)) {
  /* Supõe a!=NULL */
  return a->esq;
}

ImplBase dir(ImplBase a, int UNUSED(d)) {
  /* Supõe a!=NULL */
  return a->dir;
}

char *info(ImplBase a, int UNUSED(d)) {
  /* Supõe a!=NULL */
  sprintf(cadeiainfo,"%6d", (a->info).ra);
  /* cadeiainfo[0] = (a->info)->ra;*/
  return cadeiainfo;
}

void ImprimeBase(Base *p) {
  /* Imprime a base sob a forma de uma árvore 'deitada'. */
  ImplBase q = (ImplBase)(*p);
  printf("\n");
  ImprimeArvore(q,(subarvfunc*)esq,(subarvfunc*)dir,(infofunc*)info,NULL,1,vazia);
  printf("\n");
}

/* Funções auxiliares de rotação de nós */

static void RotLL(ImplBase *b) {

  register ImplBase aux;

  aux = (*b)->esq->dir;
  (*b)->esq->dir = *b;
  *b = (*b)->esq;
  (*b)->dir->esq = aux;

}

static void RotLR(ImplBase *b) {

  register ImplBase aux;

  aux = (*b)->esq->dir;
  (*b)->esq->dir = aux->esq;
  aux->esq = (*b)->esq;
  (*b)->esq = aux->dir;
  aux->dir = (*b);
  
  *b = aux;

}

static void RotRR(ImplBase *b) {

  register ImplBase aux;

  aux = (*b)->dir->esq;
  (*b)->dir->esq = *b;
  *b = (*b)->dir;
  (*b)->esq->dir = aux;

}

static void RotRL(ImplBase *b) {

  register ImplBase aux;

  aux = (*b)->dir->esq;
  (*b)->dir->esq = aux->dir;
  aux->dir = (*b)->dir;
  (*b)->dir = aux->esq;
  aux->esq = (*b);
  
  *b = aux;

}

static int AchaSubstituto(ImplBase *b) {
/* Substitui o valor de |*b| pelo valor do nó mais
 * à esquerda da sub-árvore direita. Retorna o RA
 * desse nó, para continuar a remoção. */

  ImplBase subst = (*b)->dir;

  while (subst->esq != NULL)
    subst = subst->esq;

  FREE((*b)->info.nome);

  (*b)->info = subst->info;
  /* Remover o ponteiro do nome na folha, a fim
   * de evitar que o mesmo seja liberado */
  subst->info.nome = NULL;

  return subst->info.ra;

}

/* Declarações das funções auxiliares de ajuste */

static void AjusteInsercaoEsquerda(ImplBase *b, Boolean *alt);
static void AjusteInsercaoDireita(ImplBase *b, Boolean *alt);

static void AjusteRemocaoEsquerda(ImplBase *b, Boolean *alt);
static void AjusteRemocaoDireita(ImplBase *b, Boolean *alt);

static void AjusteBalanceamentoInsercao(ImplBase *b, int bal);
static void AjusteBalanceamentoRemocao(ImplBase *b, int bal);

/* ============================================================ */

Base CriaBase() {
/* Devolve apontador para uma base vazia */
    
  return NULL;

}

Base SubEsq(Base *p) {
/* Devolve a subárvore esquerda da base 'p' se não for vazia; caso
   contrário devolve NULL. */

  return (Base) (*(ImplBase*)p)->esq;

}

Base SubDir(Base *p) {
/* Devolve a subárvore direita da base 'p' se não for vazia; caso
   contrário devolve NULL. */

  return (Base) (*(ImplBase*)p)->dir;

}

int FatorBal(Base *p) {
/* Devolve o fator de balanceamento da raiz da base: altura da
   subárvore direita menos a altura da subárvore esquerda; devolve
   zero se a árvore é vazia. */

  return (*(ImplBase*)p)->bal;

}

static Boolean InsereBaseRec(ImplBase *b, Aluno a, Boolean *alt) {
/* Função recursiva que realiza a inserção na árvore */

  if (*b == NULL) {
    /* |*b| é o ponto de inserção */
    *b = (ImplBase) MALLOC(sizeof **b);
    (*b)->info = a;
    (*b)->esq = NULL;
    (*b)->dir = NULL;
    (*b)->bal = 0;

    *alt = true;
    return true;
  }

  if (a.ra == (*b)->info.ra) {
    /* Aluno já existente */
    *alt = false;
    return false;
  }

  if (a.ra < (*b)->info.ra) {
    /* Inserção à esquerda */
    Boolean res = InsereBaseRec(&(*b)->esq, a, alt);

    if (!res)
      return false;

    if (*alt)
      AjusteInsercaoEsquerda(b, alt);

    return true;
  }

  if (a.ra > (*b)->info.ra) {
    /* Inserção à direita */
    Boolean res = InsereBaseRec(&(*b)->dir, a, alt);

    if (!res)
      return false;

    if (*alt)
      AjusteInsercaoDireita(b, alt);

    return true;
 }

  return false;

}

Boolean InsereBase(Base *p, Aluno a) {
/* Insere o registro 'a' na base 'p' se não existe aluno
   com o mesmo valor de 'ra', e devolve 'true';  caso
   contrário devolve 'false' */

  Boolean alt;

  return InsereBaseRec((ImplBase*)p, a, &alt);

}

static Boolean RemoveBaseRec(ImplBase *b, int ra, Boolean *alt) {
/* Função recursiva que realiza a remoção na árvore */

  Boolean res;

  if (*b == NULL) {
    /* Aluno não encontrado, remoção não efetuada */
    *alt = false;
    return false;
  }

  if (ra < (*b)->info.ra) {
    res = RemoveBaseRec(&(*b)->esq, ra, alt);

    /* Se não houve remoção, não é necessário fazer ajustes na árvore */
    if (!res)
        return false;

    if (*alt)
      AjusteRemocaoEsquerda(b, alt);

    /* Remoção com sucesso */
    return true;

  } else if (ra > (*b)->info.ra) {
    /* Idem para remoção do lado direito */
    res = RemoveBaseRec(&(*b)->dir, ra, alt);

    if (!res)
        return false;

    if (*alt)
      AjusteRemocaoDireita(b, alt);

    return true;
  }

  /* ra == (*b)->info.ra */

  /* Casos fáceis */

  if ((*b)->esq == NULL) {
    ImplBase dir = (*b)->dir;
    FREE((*b)->info.nome);
    FREE(*b);
    *b = dir;

    *alt = true;
    return true;
  }

  if ((*b)->dir == NULL) {
    ImplBase esq = (*b)->esq;
    FREE((*b)->info.nome);
    FREE(*b);
    *b = esq;

    *alt = true;
    return true;
  }

  /* Ambos os filhos estão presentes,
   * substituir por remoção do menor nó à direita */
  ra = AchaSubstituto(b);
  
  /* Continuar a descer na árvore */
  res = RemoveBaseRec(&(*b)->dir, ra, alt);
  
  if (!res)
    return false;

  if (*alt)
    AjusteRemocaoDireita(b, alt);

  return true;

}

Boolean RemoveBase(Base *p, int ra) {
/* Devolve 'true' se um registro com 'ra' pôde ser removido da
   base 'p';  caso contrário devolve 'false'. */

  Boolean alt;

  return RemoveBaseRec((ImplBase*)p, ra, &alt);

}

Boolean ConsultaBase(Base *p, int ra, Aluno *a) {
/* Devolve 'true' se existe um registro com 'ra' dado na
   base 'p';  caso contrário devolve 'false'. 'a' conterá 
   os dados do aluno, se encontrado. */

  ImplBase b = *(ImplBase*)p;

  if (b == NULL) {
    /* Chegamos a uma subárvore de uma folha.
     * Portanto o valor não está na base */
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

  /* Não está nem na sub-árvore esquerda nem na direita */
  return false;

}
   
static int AlturaBaseRec(ImplBase b, int nivel) {
/* Função auxiliar para calcular a altura da árvore,
 * onde |*p| possui nível |nivel + 1| */

  int e, d;

  /* Se |b| é nulo, retornar o nível do pai */
  if (b == NULL)
    return nivel;
  
  /* Calcula a altura da árvore considerando somente a 
   * subárvore esquerda */
  e = AlturaBaseRec(b->esq, nivel + 1);
  /* Idem para a direita */
  d = AlturaBaseRec(b->dir, nivel + 1);

  /* Retorna a maior altura encontrada */
  return e > d ? e : d;

}

int AlturaBase(Base *p) {
/* Devolve a altura da base 'p'. */
  
  return AlturaBaseRec(*(ImplBase*)p, 0);

}

static void NumeroNosBaseRec(ImplBase b, int *contador) {
/* Incrementa |*contador| para cada nó visitado */
  
  while (b != NULL) {
    NumeroNosBaseRec(b->esq, contador);
    (*contador)++;
    /* Remoção de recursão caudal */
    b = b->dir;
  }

}

int NumeroNosBase(Base *p) {
/* Devolve o número de nós da base 'p'. */

  int contador = 0;

  NumeroNosBaseRec(*(ImplBase*)p, &contador);

  return contador;

}

void PercorreBase(Base *p, TipoVisita Visita) {
/* Executa um percurso inordem na base, invocando a função Visita
   para todos os elementos. */

  ImplBase b = *(ImplBase*)p;

  while (b != NULL) {
    PercorreBase((Base*) &(b->esq), Visita);
    Visita(&(b->info));
    /* Remoção de recursão caudal */
    b = b->dir;
  }

}

void LiberaBase(Base *p) {
/* Libera todos os nós da base apontada por 'p', bem 
   como todas as cadeias que guardam os nomes. */

  ImplBase b = *(ImplBase*)p;

  if (b != NULL) {
    LiberaBase((Base*) &(b->esq));
    LiberaBase((Base*) &(b->dir));

    FREE(b->info.nome);
    FREE(b);
  }
}

/* Funções auxiliares de ajuste */

static void AjusteInsercaoEsquerda(ImplBase *b, Boolean *alt) {
/* Ajusta a árvore para manter as propriedades AVL,
 * quando houve inserção do lado esquerdo. */

  switch((*b)->bal) {
    case -1:
      /* Necessário realizar ajuste da árvore, dependente
       * do fator de balanceamento do filho esquerdo. */
      switch ((*b)->esq->bal) {
        case -1: {
          RotLL(b);
          (*b)->dir->bal = 0;
          break;
        }
        case +1: {
          int bal_LR = (*b)->esq->dir->bal;
          RotLR(b);
          AjusteBalanceamentoInsercao(b, bal_LR);
          break;
        }
      }
      (*b)->bal = 0;
      *alt = false;
      break;
    case  0:
      (*b)->bal = -1;
      *alt = true;
      break;
    case +1:
      (*b)->bal =  0;
      *alt = false;
      break;
  }

}

static void AjusteInsercaoDireita(ImplBase *b, Boolean *alt) {
/* Ajusta a árvore para manter as propriedades AVL,
 * quando houve inserção do lado direito. */

  switch((*b)->bal) {
    case -1:
      (*b)->bal =  0;
      *alt = false;
      break;
    case  0:
      (*b)->bal = +1;
      *alt = true;
      break;
    case +1:
      /* Necessário realizar ajuste da árvore, dependente
       * do fator de balanceamento do filho direito. */
      switch ((*b)->dir->bal) {
        case +1: {
          RotRR(b);
          (*b)->esq->bal = 0;
          break;
        }
        case -1: {
          int bal_RL = (*b)->dir->esq->bal;
          RotRL(b);
          AjusteBalanceamentoInsercao(b, bal_RL);
          break;
        }
      }
      (*b)->bal = 0;
      *alt = false;
      break;
  }

}

static void AjusteRemocaoEsquerda(ImplBase *b, Boolean *alt) {
/* Ajusta a árvore para manter as propriedades AVL,
 * quando houve remoção do lado esquerdo. */

  switch ((*b)->bal) {
    case -1:
      (*b)->bal =  0;
      *alt = true;
      break;
    case  0:
      (*b)->bal = +1;
      *alt = false;
      break;
    case +1:
      /* Necessário realizar ajuste da árvore, dependente
       * do fator de balanceamento do filho direito. */
      switch ((*b)->dir->bal) {
        case -1: {
          /* Rotação RL */
          int bal_RL = (*b)->dir->esq->bal;
          RotRL(b);
          AjusteBalanceamentoRemocao(b, bal_RL);
          (*b)->bal = 0;
          *alt = true;
          break;
        }
        case  0: {
          /* Rotação RR */
          RotRR(b);
          (*b)->bal = -1;
          *alt = false;
          break;
        }
        case +1: {
          /* Rotação RR */
          RotRR(b);
          (*b)->bal = 0;
          (*b)->esq->bal = 0;
          *alt = true;
          break;
        }
      }
      break;
  }

}

static void AjusteRemocaoDireita(ImplBase *b, Boolean *alt) {
/* Ajusta a árvore para manter as propriedades AVL,
 * quando houve remoção do lado direito. */

  switch ((*b)->bal) {
    case -1:
      /* Necessário realizar ajuste da árvore, dependente
       * do fator de balanceamento do filho esquerdo. */
      switch ((*b)->esq->bal) {
        case -1: {
          /* Rotação LL */
          RotLL(b);
          (*b)->bal = 0;
          (*b)->dir->bal = 0;
          *alt = true;
          break;
        }
        case  0: {
          /* Rotação LL */
          RotLL(b);
          (*b)->bal = +1;
          *alt = false;
          break;
        }
        case +1: {
          /* Rotação LR */
          int bal_LR = (*b)->esq->dir->bal;
          RotLR(b);
          AjusteBalanceamentoRemocao(b, bal_LR);
          (*b)->bal = 0;
          *alt = true;
          break;
        }
      }
      break;
    case  0:
      (*b)->bal = -1;
      *alt = false;
      break;
    case +1:
      (*b)->bal =  0;
      *alt = true;
      break;
  }

}

static void AjusteBalanceamentoInsercao(ImplBase *b, int bal) {

  switch (bal) {
    case -1:
      (*b)->esq->bal =  0;
      (*b)->dir->bal = +1;
      break;
    case 0:
      (*b)->esq->bal =  0;
      (*b)->dir->bal =  0;
      break;
    case +1:
      (*b)->esq->bal = -1;
      (*b)->dir->bal =  0;
      break;
  }

}

static void AjusteBalanceamentoRemocao(ImplBase *b, int bal) {

  switch (bal) {
    case -1:
      (*b)->esq->bal =  0;
      (*b)->dir->bal = +1;
      break;
    case  0:
      (*b)->esq->bal =  0;
      (*b)->dir->bal =  0;
      break;
    case +1:
      (*b)->esq->bal = -1;
      (*b)->dir->bal =  0;
      break;
  }

}

