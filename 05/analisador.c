/*
  Autor:         Gabriel Souza Franco
  RA:            155477
  Disciplina:    MC202
  Turma:         F
  
  Tarefa 05 
  Segundo semestre de 2014

*/

/* 
 * analisador.c: M�dulo de transforma��o de express�es na nota��o
 * infixa para p�s-fixa.
 */

#include "analisador.h"
#include <ctype.h>
#include <stdbool.h>

/* Vari�veis globais a este m�dulo */

char *in;  /* Cadeia e �ndice para express�o infixa (entrada). */
int indIn;

char *pos; /* Cadeia e �ndice para express�o p�sfixa (sa�da). */
int indPos;

Erro resCorreto = {EXPR_VALIDA,0}; /* resultado correto */


/* Prot�tipos das fun��es mutuamente recursivas  */
Erro Expressao();
Erro Termo();
Erro Fator();
Erro Primario();

/* Fun��o auxiliar */
Erro montaErro(int codigo, int pos);

/*************************************************************/
/*                 Fun��o   principal                        */
/*************************************************************/

Erro InPos(char *infixa, char *posfixa) {
/* Transforma uma express�o da nota��o infixa para a p�s-fixa. Em caso
   de erro, devolve o c�digo e a posi��o na cadeia de entrada onde o
   erro foi encontrado.  */

  Erro err = resCorreto;
  char c;

  in = infixa;
  indIn = 0;
  pos = posfixa;
  indPos = 0;

  /* Remover os espa�os do in�cio */
  while (isspace(in[indIn]))
    indIn++;

  /* Se n�o h� nada na entrada, marcar erro */
  if (in[indIn] == '\0')
    err = montaErro(CADEIA_DE_BRANCOS, 0);
  /* Sen�o, computar a express�o */
  else
    err = Expressao();

  /* Se a entrada n�o acabou, marcar erro */
  if (in[indIn] != '\0' && err.codigoErro == EXPR_VALIDA)
    err = montaErro(OPERADOR_ESPERADO, indIn);

  /* Verificar se o caracter que causou o erro � inv�lido */
  if (err.codigoErro != EXPR_VALIDA) {
    c = in[indIn];
    if (!isalpha(c) && !isspace(c) &&
        c != '+' && c != '-' && c != '*' && c != '/' &&
        c != '(' && c != ')' && c != '^' && c != '\0')
      err = montaErro(CARACTERE_INVALIDO, indIn);
  }

  posfixa[indPos] = '\0';
  return err;

}

/*************************************************************/
/*           Fun��es de implementa��o do analisador          */
/*************************************************************/

Erro montaErro(int codigo, int posicao) {
/* Devolve estrutura com c�digo de erro e posi��o */

  return (Erro) { .posicao = posicao, .codigoErro = codigo };

} /* montaErro */


Erro Expressao() {
/* Processa uma express�o da cadeia de entrada.  */

  Erro err = resCorreto;
  char op;

  /* Remover os espa�os do in�cio */
  while (isspace(in[indIn]))
    indIn++;

  /* Fim da entrada */
  if (in[indIn] == '\0' && in[indIn] == ')')
    return montaErro(OPERANDO_ESPERADO, indIn);

  /* Tratar mais e menos un�rio aqui, pois eles s� podem aparecer
   * no primeiro elemento de uma express�o */
  if (in[indIn] == '-') {
    indIn++;
    err = Termo();
    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parent�tico tem preced�ncia */
      if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
        return err;
      return montaErro(OPERANDO_ESPERADO, indIn);
    }
    pos[indPos++] = '~';
  } else if (in[indIn] == '+') {
    indIn++;
    err = Termo();
    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parent�tico tem preced�ncia */
      if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
        return err;
      return montaErro(OPERANDO_ESPERADO, indIn);
    }
    pos[indPos++] = '&';
  } else {
    err = Termo();
    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parent�tico tem preced�ncia */
      if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
        return err;
      return montaErro(OPERANDO_ESPERADO, indIn);
    }
  }

  do {
    /* Remover os espa�os at� o pr�ximo caracter */
    while (isspace(in[indIn]))
      indIn++;
    
    op = in[indIn];

    if (op == '+' || op == '-') {
      /* Consumir o operador */
      indIn++;

      err = Termo();
      if (err.codigoErro != EXPR_VALIDA) {
        /* O erro parent�tico tem preced�ncia */
        if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
          return err;
        return montaErro(OPERANDO_ESPERADO, indIn);
      }
      pos[indPos++] = op;
    } else
      break;
  } while (true);

  return resCorreto;

} /* Expressao */


Erro Termo() {
/* Processa um termo da cadeia de entrada.  */

  Erro err = resCorreto;
  char op;

  /* Remover os espa�os do in�cio */
  while (isspace(in[indIn]))
    indIn++;

  /* Fim da entrada */
  if (in[indIn] == '\0' || in[indIn] == ')')
    return montaErro(OPERANDO_ESPERADO, indIn);

  err = Fator();
  if (err.codigoErro != EXPR_VALIDA)
    return err;

  do {  
    /* Remover os espa�os at� o pr�ximo caracter */
    while (isspace(in[indIn]))
      indIn++;

    op = in[indIn];

    if (op == '*' || op == '/') {
      /* Consumir o operador */
      indIn++;

      err = Fator();
      if (err.codigoErro != EXPR_VALIDA)
        return err;
      pos[indPos++] = op;
    } else
      break;
  } while (true);

  return resCorreto; 

} /* Termo */


Erro Fator() {
/* Processa um fator da cadeia de entrada.  */

  Erro err = resCorreto;

  /* Remover os espa�os do in�cio */
  while (isspace(in[indIn]))
    indIn++;

  err = Primario();
  if (err.codigoErro != EXPR_VALIDA)
      return err;

  while (isspace(in[indIn]))
    indIn++;

  if (in[indIn] == '^') {
    indIn++;
    err = Fator();
    if (err.codigoErro != EXPR_VALIDA)
      return err;

    pos[indPos++] = '^';
  }

  return resCorreto;

} /* Fator */


Erro Primario() {
/* Processa um fator prim�rio da cadeia de entrada.  */

  Erro err = resCorreto;
  char letra;

  /* Remover os espa�os do in�cio */
  while (isspace(in[indIn]))
    indIn++;

  /* Se for par�ntese, iniciar express�o aninhada */
  if (in[indIn] == '(') {
    indIn++;
    err = Expressao();
    if (err.codigoErro != EXPR_VALIDA)
      return err;
    if (in[indIn] == ')')
      indIn++;
    else
      return montaErro(FECHA_PARENTESE_ESPERADO, indIn);
  } else {
    letra = in[indIn];
    /* Verificar se � um operando v�lido */
    if (isalpha(letra))
      pos[indPos++] = letra;
    else
      return montaErro(CARACTERE_INVALIDO, indIn);
    indIn++;
  }

  return resCorreto;

} /* Primario */
