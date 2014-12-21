/*
  Autor:         Gabriel Souza Franco
  RA:            155477
  Disciplina:    MC202
  Turma:         F
  
  Tarefa 06 
  Segundo semestre de 2014

*/

/* 
 * analisador.c: Módulo de transformação de expressões em árvores
 * binárias e seus percursos.
 */

#include "analisador.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "balloc.h"

/* Variáveis globais a este módulo */

char *in;  /* Cadeia e índice para expressão infixa (entrada). */
int indIn;

Erro resCorreto = {EXPR_VALIDA,0}; /* resultado correto */

/* Macro para desprezar espaços em branco na cadeia de entrada. */
/* Para evitar chamadas desnecessárias a esta macro, adotou-se  */
/* seguinte conveção. A macro é invocada uma vez imediatamente  */
/* antes da primeira chamada à função Expressao e imediatamente */
/* após todo o incremento de indIn.                             */
#define DESPREZA_ESPACOS() while (in[indIn] == ' ') indIn++


/* Protótipos das funções mutuamente recursivas. */
/* O resultado é devolvido através da variável 'arv'. */
Erro Expressao(ArvBin *arv);
Erro Termo(ArvBin *arv);
Erro Fator(ArvBin *arv);
Erro Primario(ArvBin *arv);

/* Função auxiliar -- declarada mais adiante */
Erro montaErro(int codigo, int pos);

/* Função auxiliar de contrução de nó */
NoArvBin* criaNo(char info, NoArvBin *esq, NoArvBin *dir) {
  
  NoArvBin *no = (NoArvBin*) MALLOC(sizeof(NoArvBin));
  no->info = info;
  no->esq = esq;
  no->dir = dir;

  return no;
}


/*************************************************************/
/*                 Função   principal                        */
/*************************************************************/

Erro InArv(char *infixa, ArvBin *arv) {
/* Transforma a notação infixa em árvore binária.  Em caso de erro,
   devolve o código e a posição na cadeia de entrada onde o erro foi
   encontrado.  */

  Erro err = resCorreto;

  in = infixa;
  indIn = 0;

  DESPREZA_ESPACOS();

  /* Se não há nada na entrada, marcar erro */
  if (in[indIn] == '\0')
    err = montaErro(CADEIA_DE_BRANCOS, 0);
  /* Senão, computar a expressão */
  else
    err = Expressao(arv);

  /* Se a entrada não acabou, marcar erro */
  if (in[indIn] != '\0' && err.codigoErro == EXPR_VALIDA)
    err = montaErro(OPERADOR_ESPERADO, indIn);

  /* Verificar se o caracter que causou o erro é inválido */
  if (err.codigoErro != EXPR_VALIDA) {
    char c = in[indIn];
    if (!isalpha(c) && !isspace(c) &&
        c != '+' && c != '-' && c != '*' && c != '/' &&
        c != '(' && c != ')' && c != '^' && c != '\0')
      err = montaErro(CARACTERE_INVALIDO, indIn);
  }
  
  return err;

}

/*************************************************************/
/*           Funções de implementação do analisador          */
/*************************************************************/

Erro montaErro(int codigo, int posicao) {
/* Devolve estrutura com código de erro e posição */

  return (Erro) { .posicao = posicao, .codigoErro = codigo };

} /* montaErro */


Erro Expressao(ArvBin *arv) {
/* Processa uma expressão da cadeia de entrada.  */

  Erro err = resCorreto;
  char op;

  /* Fim da entrada */
  if (in[indIn] == '\0' && in[indIn] == ')')
    return montaErro(OPERANDO_ESPERADO, indIn);

  /* Tratar mais e menos unário aqui, pois eles só podem aparecer
   * no primeiro elemento de uma expressão */
  if (in[indIn] == '-') {
    indIn++;
    DESPREZA_ESPACOS();

    *arv = criaNo('~', NULL, NULL);
    err = Termo(&(*arv)->dir);

    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parentético tem precedência */
      if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
        return err;
      return montaErro(OPERANDO_ESPERADO, indIn);
    }
    
  } else if (in[indIn] == '+') {
    indIn++;
    DESPREZA_ESPACOS();

    *arv = criaNo('&', NULL, NULL);
    err = Termo(&(*arv)->dir);

    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parentético tem precedência */
      if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
        return err;
      return montaErro(OPERANDO_ESPERADO, indIn);
    }
  } else {

    err = Termo(arv);

    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parentético tem precedência */
      if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
        return err;
      return montaErro(OPERANDO_ESPERADO, indIn);
    }
  }

  do {
    op = in[indIn];

    if (op == '+' || op == '-') {
      /* Consumir o operador */
      indIn++;
      DESPREZA_ESPACOS();

      /* O operando esquerdo está em |*arv|,
       * então substituir seu conteúdo por um nó que
       * tenha o valor antigo como filho esquerdo.*/
      *arv = criaNo(op, *arv, NULL);

      err = Termo(&(*arv)->dir);

      if (err.codigoErro != EXPR_VALIDA) {
        /* O erro parentético tem precedência */
        if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
          return err;
        return montaErro(OPERANDO_ESPERADO, indIn);
      }
    } else
      break;
  } while (true);

  return resCorreto;
  
} /* Expressao */


Erro Termo(ArvBin *arv) {
/* Processa um termo da cadeia de entrada.  */
  
  Erro err;
  char op;

  /* Fim da expressão */
  if (in[indIn] == '\0' || in[indIn] == ')')
    return montaErro(OPERANDO_ESPERADO, indIn);

  err = Fator(arv);
  if (err.codigoErro != EXPR_VALIDA)
    return err;

  do {  
    op = in[indIn];

    if (op == '*' || op == '/') {
      /* Consumir o operador */
      indIn++;
      DESPREZA_ESPACOS();

      /* O operando esquerdo está em |*arv|,
       * então substituir seu conteúdo por um nó que
       * tenha o valor antigo como filho esquerdo.*/
      *arv = criaNo(op, *arv, NULL);
      err = Fator(&(*arv)->dir);

      if (err.codigoErro != EXPR_VALIDA)
        return err;
    } else
      break;
  } while (true);

  return resCorreto; 

} /* Termo */


Erro Fator(ArvBin *arv) {
/* Processa um fator da cadeia de entrada.  */

  Erro err = resCorreto;

  /* Remover os espaços do início */
  DESPREZA_ESPACOS();

  err = Primario(arv);

  if (err.codigoErro != EXPR_VALIDA)
      return err;

  if (in[indIn] == '^') {
    indIn++;
    DESPREZA_ESPACOS();

    /* O operando esquerdo está em |*arv|,
     * então substituir seu conteúdo por um nó que
     * tenha o valor antigo como filho esquerdo.*/
    *arv = criaNo('^', *arv, NULL);

    err = Fator(&(*arv)->dir);

    if (err.codigoErro != EXPR_VALIDA)
      return err;
  }

  return resCorreto;

} /* Fator */


Erro Primario(ArvBin *arv) {
/* Processa um  primário da cadeia de entrada.  */

  Erro err = resCorreto;

  /* Se for parêntese, iniciar expressão aninhada */
  if (in[indIn] == '(') {
    indIn++;
    DESPREZA_ESPACOS();

    err = Expressao(arv);

    if (err.codigoErro != EXPR_VALIDA)
      return err;
    if (in[indIn] == ')') {
      indIn++;
      DESPREZA_ESPACOS();
    }
    else
      return montaErro(FECHA_PARENTESE_ESPERADO, indIn);
  } else {
    char letra = in[indIn];
    /* Verificar se é um operando válido */
    if (isalpha(letra)) {
      *arv = criaNo(letra, NULL, NULL);
    } else
      return montaErro(CARACTERE_INVALIDO, indIn);
    indIn++;
    DESPREZA_ESPACOS();
  }

  return resCorreto;

} /* Primario */

/* Percursos */

/* Função auxiliar para implementar a recursão */
void ArvPreRec(ArvBin arv, char **pre) {

  while (arv != NULL) {
      (**pre) = arv->info;
      (*pre)++;

      /* Sub-árvore esquerda */
      ArvPreRec(arv->esq, pre);
      /* Sub-árvore direita, remoção de recursão caudal */
      arv = arv->dir;
  }

}

void ArvPre(ArvBin arv, char *pre) {
 /* Produz a representação pré-fixa a partir da árvore. */ 
  
  ArvPreRec(arv, &pre);
  *pre = '\0';

}

/* Função auxiliar para implementar a recursão */
void ArvPosRec(ArvBin arv, char **pos) {

  if (arv != NULL) {
      ArvPosRec(arv->esq, pos);
      ArvPosRec(arv->dir, pos);

      (**pos) = arv->info;
      (*pos)++;
  }

}

void ArvPos(ArvBin arv, char *pos) {
/* Produz a representação pós-fixa a partir da árvore. */  

  ArvPosRec(arv, &pos);
  *pos = '\0';

}


void LiberaArv(ArvBin arv) {
/* Libera o espaço ocupado pela árvore. */

  if (arv != NULL) {
      LiberaArv(arv->esq);
      LiberaArv(arv->dir);

      FREE(arv);
  }

}

