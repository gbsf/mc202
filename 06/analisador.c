/*
  Autor:         Gabriel Souza Franco
  RA:            155477
  Disciplina:    MC202
  Turma:         F
  
  Tarefa 06 
  Segundo semestre de 2014

*/

/* 
 * analisador.c: M�dulo de transforma��o de express�es em �rvores
 * bin�rias e seus percursos.
 */

#include "analisador.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "balloc.h"

/* Vari�veis globais a este m�dulo */

char *in;  /* Cadeia e �ndice para express�o infixa (entrada). */
int indIn;

Erro resCorreto = {EXPR_VALIDA,0}; /* resultado correto */

/* Macro para desprezar espa�os em branco na cadeia de entrada. */
/* Para evitar chamadas desnecess�rias a esta macro, adotou-se  */
/* seguinte conve��o. A macro � invocada uma vez imediatamente  */
/* antes da primeira chamada � fun��o Expressao e imediatamente */
/* ap�s todo o incremento de indIn.                             */
#define DESPREZA_ESPACOS() while (in[indIn] == ' ') indIn++


/* Prot�tipos das fun��es mutuamente recursivas. */
/* O resultado � devolvido atrav�s da vari�vel 'arv'. */
Erro Expressao(ArvBin *arv);
Erro Termo(ArvBin *arv);
Erro Fator(ArvBin *arv);
Erro Primario(ArvBin *arv);

/* Fun��o auxiliar -- declarada mais adiante */
Erro montaErro(int codigo, int pos);

/* Fun��o auxiliar de contru��o de n� */
NoArvBin* criaNo(char info, NoArvBin *esq, NoArvBin *dir) {
  
  NoArvBin *no = (NoArvBin*) MALLOC(sizeof(NoArvBin));
  no->info = info;
  no->esq = esq;
  no->dir = dir;

  return no;
}


/*************************************************************/
/*                 Fun��o   principal                        */
/*************************************************************/

Erro InArv(char *infixa, ArvBin *arv) {
/* Transforma a nota��o infixa em �rvore bin�ria.  Em caso de erro,
   devolve o c�digo e a posi��o na cadeia de entrada onde o erro foi
   encontrado.  */

  Erro err = resCorreto;

  in = infixa;
  indIn = 0;

  DESPREZA_ESPACOS();

  /* Se n�o h� nada na entrada, marcar erro */
  if (in[indIn] == '\0')
    err = montaErro(CADEIA_DE_BRANCOS, 0);
  /* Sen�o, computar a express�o */
  else
    err = Expressao(arv);

  /* Se a entrada n�o acabou, marcar erro */
  if (in[indIn] != '\0' && err.codigoErro == EXPR_VALIDA)
    err = montaErro(OPERADOR_ESPERADO, indIn);

  /* Verificar se o caracter que causou o erro � inv�lido */
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
/*           Fun��es de implementa��o do analisador          */
/*************************************************************/

Erro montaErro(int codigo, int posicao) {
/* Devolve estrutura com c�digo de erro e posi��o */

  return (Erro) { .posicao = posicao, .codigoErro = codigo };

} /* montaErro */


Erro Expressao(ArvBin *arv) {
/* Processa uma express�o da cadeia de entrada.  */

  Erro err = resCorreto;
  char op;

  /* Fim da entrada */
  if (in[indIn] == '\0' && in[indIn] == ')')
    return montaErro(OPERANDO_ESPERADO, indIn);

  /* Tratar mais e menos un�rio aqui, pois eles s� podem aparecer
   * no primeiro elemento de uma express�o */
  if (in[indIn] == '-') {
    indIn++;
    DESPREZA_ESPACOS();

    *arv = criaNo('~', NULL, NULL);
    err = Termo(&(*arv)->dir);

    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parent�tico tem preced�ncia */
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
      /* O erro parent�tico tem preced�ncia */
      if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
        return err;
      return montaErro(OPERANDO_ESPERADO, indIn);
    }
  } else {

    err = Termo(arv);

    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parent�tico tem preced�ncia */
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

      /* O operando esquerdo est� em |*arv|,
       * ent�o substituir seu conte�do por um n� que
       * tenha o valor antigo como filho esquerdo.*/
      *arv = criaNo(op, *arv, NULL);

      err = Termo(&(*arv)->dir);

      if (err.codigoErro != EXPR_VALIDA) {
        /* O erro parent�tico tem preced�ncia */
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

  /* Fim da express�o */
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

      /* O operando esquerdo est� em |*arv|,
       * ent�o substituir seu conte�do por um n� que
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

  /* Remover os espa�os do in�cio */
  DESPREZA_ESPACOS();

  err = Primario(arv);

  if (err.codigoErro != EXPR_VALIDA)
      return err;

  if (in[indIn] == '^') {
    indIn++;
    DESPREZA_ESPACOS();

    /* O operando esquerdo est� em |*arv|,
     * ent�o substituir seu conte�do por um n� que
     * tenha o valor antigo como filho esquerdo.*/
    *arv = criaNo('^', *arv, NULL);

    err = Fator(&(*arv)->dir);

    if (err.codigoErro != EXPR_VALIDA)
      return err;
  }

  return resCorreto;

} /* Fator */


Erro Primario(ArvBin *arv) {
/* Processa um  prim�rio da cadeia de entrada.  */

  Erro err = resCorreto;

  /* Se for par�ntese, iniciar express�o aninhada */
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
    /* Verificar se � um operando v�lido */
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

/* Fun��o auxiliar para implementar a recurs�o */
void ArvPreRec(ArvBin arv, char **pre) {

  while (arv != NULL) {
      (**pre) = arv->info;
      (*pre)++;

      /* Sub-�rvore esquerda */
      ArvPreRec(arv->esq, pre);
      /* Sub-�rvore direita, remo��o de recurs�o caudal */
      arv = arv->dir;
  }

}

void ArvPre(ArvBin arv, char *pre) {
 /* Produz a representa��o pr�-fixa a partir da �rvore. */ 
  
  ArvPreRec(arv, &pre);
  *pre = '\0';

}

/* Fun��o auxiliar para implementar a recurs�o */
void ArvPosRec(ArvBin arv, char **pos) {

  if (arv != NULL) {
      ArvPosRec(arv->esq, pos);
      ArvPosRec(arv->dir, pos);

      (**pos) = arv->info;
      (*pos)++;
  }

}

void ArvPos(ArvBin arv, char *pos) {
/* Produz a representa��o p�s-fixa a partir da �rvore. */  

  ArvPosRec(arv, &pos);
  *pos = '\0';

}


void LiberaArv(ArvBin arv) {
/* Libera o espa�o ocupado pela �rvore. */

  if (arv != NULL) {
      LiberaArv(arv->esq);
      LiberaArv(arv->dir);

      FREE(arv);
  }

}

