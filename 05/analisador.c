/*
  Autor:         Gabriel Souza Franco
  RA:            155477
  Disciplina:    MC202
  Turma:         F
  
  Tarefa 05 
  Segundo semestre de 2014

*/

/* 
 * analisador.c: Módulo de transformação de expressões na notação
 * infixa para pós-fixa.
 */

#include "analisador.h"
#include <ctype.h>
#include <stdbool.h>

/* Variáveis globais a este módulo */

char *in;  /* Cadeia e índice para expressão infixa (entrada). */
int indIn;

char *pos; /* Cadeia e índice para expressão pósfixa (saída). */
int indPos;

Erro resCorreto = {EXPR_VALIDA,0}; /* resultado correto */


/* Protótipos das funções mutuamente recursivas  */
Erro Expressao();
Erro Termo();
Erro Fator();
Erro Primario();

/* Função auxiliar */
Erro montaErro(int codigo, int pos);

/*************************************************************/
/*                 Função   principal                        */
/*************************************************************/

Erro InPos(char *infixa, char *posfixa) {
/* Transforma uma expressão da notação infixa para a pós-fixa. Em caso
   de erro, devolve o código e a posição na cadeia de entrada onde o
   erro foi encontrado.  */

  Erro err = resCorreto;
  char c;

  in = infixa;
  indIn = 0;
  pos = posfixa;
  indPos = 0;

  /* Remover os espaços do início */
  while (isspace(in[indIn]))
    indIn++;

  /* Se não há nada na entrada, marcar erro */
  if (in[indIn] == '\0')
    err = montaErro(CADEIA_DE_BRANCOS, 0);
  /* Senão, computar a expressão */
  else
    err = Expressao();

  /* Se a entrada não acabou, marcar erro */
  if (in[indIn] != '\0' && err.codigoErro == EXPR_VALIDA)
    err = montaErro(OPERADOR_ESPERADO, indIn);

  /* Verificar se o caracter que causou o erro é inválido */
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
/*           Funções de implementação do analisador          */
/*************************************************************/

Erro montaErro(int codigo, int posicao) {
/* Devolve estrutura com código de erro e posição */

  return (Erro) { .posicao = posicao, .codigoErro = codigo };

} /* montaErro */


Erro Expressao() {
/* Processa uma expressão da cadeia de entrada.  */

  Erro err = resCorreto;
  char op;

  /* Remover os espaços do início */
  while (isspace(in[indIn]))
    indIn++;

  /* Fim da entrada */
  if (in[indIn] == '\0' && in[indIn] == ')')
    return montaErro(OPERANDO_ESPERADO, indIn);

  /* Tratar mais e menos unário aqui, pois eles só podem aparecer
   * no primeiro elemento de uma expressão */
  if (in[indIn] == '-') {
    indIn++;
    err = Termo();
    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parentético tem precedência */
      if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
        return err;
      return montaErro(OPERANDO_ESPERADO, indIn);
    }
    pos[indPos++] = '~';
  } else if (in[indIn] == '+') {
    indIn++;
    err = Termo();
    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parentético tem precedência */
      if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
        return err;
      return montaErro(OPERANDO_ESPERADO, indIn);
    }
    pos[indPos++] = '&';
  } else {
    err = Termo();
    if (err.codigoErro != EXPR_VALIDA) {
      /* O erro parentético tem precedência */
      if (err.codigoErro == FECHA_PARENTESE_ESPERADO)
        return err;
      return montaErro(OPERANDO_ESPERADO, indIn);
    }
  }

  do {
    /* Remover os espaços até o próximo caracter */
    while (isspace(in[indIn]))
      indIn++;
    
    op = in[indIn];

    if (op == '+' || op == '-') {
      /* Consumir o operador */
      indIn++;

      err = Termo();
      if (err.codigoErro != EXPR_VALIDA) {
        /* O erro parentético tem precedência */
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

  /* Remover os espaços do início */
  while (isspace(in[indIn]))
    indIn++;

  /* Fim da entrada */
  if (in[indIn] == '\0' || in[indIn] == ')')
    return montaErro(OPERANDO_ESPERADO, indIn);

  err = Fator();
  if (err.codigoErro != EXPR_VALIDA)
    return err;

  do {  
    /* Remover os espaços até o próximo caracter */
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

  /* Remover os espaços do início */
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
/* Processa um fator primário da cadeia de entrada.  */

  Erro err = resCorreto;
  char letra;

  /* Remover os espaços do início */
  while (isspace(in[indIn]))
    indIn++;

  /* Se for parêntese, iniciar expressão aninhada */
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
    /* Verificar se é um operando válido */
    if (isalpha(letra))
      pos[indPos++] = letra;
    else
      return montaErro(CARACTERE_INVALIDO, indIn);
    indIn++;
  }

  return resCorreto;

} /* Primario */
