/* Este m�dulo implementa opera��es sobre polin�mios em uma vari�vel.   */
/* Cada polin�mio � representado por uma lista ligada circular, com n�  */
/* cabe�a. Os n�s da lista representam os termos n�o nulos do polin�mio */
/* em ordem crescente dos seus expoentes. O n� cabe�a tem expoente '-1'.*/

#include "polinomios.h"
#include "balloc.h"
#include <stdio.h>

/*-----------------------  Fun��o auxiliar  ----------------------------*/

void InsereTermoAux(Polinomio p, int e, float c) {
/* Insere o termo '(e,c)' ap�s o n� apontado por 'p'.                   */

  Termo *novo = MALLOC(sizeof(Termo));
  novo->expo = e;
  novo->coef = c;
  novo->prox = p->prox;

  p->prox = novo;

} /* InsereTermoAux */



/*---------------- Implementa��o das fun��es da interface --------------*/


void ImprimePolinomio(Polinomio p) {
/* Imprime, em ordem crescente dos expoentes, os termos do              */
/* n�o nulos do 'p'. No caso do polin�mio identicamente nulo,           */
/* imprime as palavras "Polin�mio nulo".                                */

  if (p==NULL) {
    printf("Polin�mio inv�lido\n");
    return;
  }

  if (p->prox==p) {
    printf("Polin�mio nulo\n");
    return;
  }

  p = p->prox;
  while (p->expo!=-1) {
    printf("(%2d,%5.1f) ",p->expo,p->coef);
    p = p->prox;
  }
  printf("\n");

} /* ImprimePolinomio */


Polinomio CriaPolinomioNulo() {
/* Devolve um novo polin�mio identicamente nulo.                        */

  Polinomio polinomio = MALLOC(sizeof(Termo));
  polinomio->expo = -1;
  polinomio->coef = 0;
  polinomio->prox = polinomio;

  return polinomio;

} /* CriaPolinomioNulo */



void LiberaPolinomio(Polinomio p) {
/* Libera toda a mem�ria din�mica ocupada por um polin�mio.             */

  Termo *prox;

  /* Pular a cabe�a por enquanto */
  for (Termo *t = p->prox; t != p;) {
    prox = t->prox;
    FREE(t);
    t = prox;
  }

  /* Liberar a cabe�a */
  FREE(p);

}

Boolean PolinomioNulo(Polinomio p) {
/* Verifica se o polin�mio 'p' � identicamente nulo.                    */

  return p->prox == p;

} /* PolinomioNulo */


void InsereTermo(Polinomio p, int e, float c) {
/* Insere no polinomio 'p' o termo '(e,c)', de maneira a manter os      */
/* termos ordenados. Sup�e que n�o existe ainda em 'p' um termo com     */
/* expoente 'e', e que 'c' n�o � zero.                                  */

  while (p->prox->expo != -1 && p->prox->expo < e)
    p = p->prox;

  InsereTermoAux(p, e, c);

} /* InsereTermo */


Polinomio SomaPolinomios(Polinomio a, Polinomio b) {
/* Devolve a soma dos polin�mios 'a' e 'b'. N�o altera os polin�mios    */
/* dados. Termos nulos n�o s�o inseridos no resultados.                 */

  float soma;
  Polinomio resultado = CriaPolinomioNulo();
  Termo *fim = resultado;

  /* Ignorar as cabe�as */
  a = a->prox;
  b = b->prox;

  while (a->expo != -1 && b->expo != -1) {
    /* Se o expoente de |a| for menor que o de |b| (e vice-versa),
     * adicionar o elemento no resultado e avan�ar */
    if (a->expo < b->expo) {
      InsereTermoAux(fim, a->expo, a->coef);
      fim = fim->prox;
      a = a->prox;
    } else if (b->expo < a->expo) {
      InsereTermoAux(fim, b->expo, b->coef);
      fim = fim->prox;
      b = b->prox;
    } else {
      soma = a->coef + b->coef;
      
      /* Somente adicionar se o coeficiente resultante n�o for 0 */
      if (soma != 0) {
        InsereTermoAux(fim, a->expo, soma);
        fim = fim->prox;
      }
      a = a->prox;
      b = b->prox;
    }
  }

  /* Se os polin�mios n�o possuem a mesma ordem,
   * completar com os elementos restantes */
  while (a->expo != -1) {
      InsereTermoAux(fim, a->expo, a->coef);
      fim = fim->prox;
      a = a->prox;
  }

  while (b->expo != -1) {
      InsereTermoAux(fim, b->expo, b->coef);
      fim = fim->prox;
      b = b->prox;
  }

  return resultado;

} /* SomaPolinomios */


Polinomio SubPolinomios(Polinomio a, Polinomio b) {
/* Devolve a diferen�a entre o polin�mio 'a' e o polin�mio 'b'. N�o      */
/* altera os polin�mios dados. Termos nulos n�o s�o inseridos no         */
/* resultado.                                                            */

  float sub;
  Polinomio resultado = CriaPolinomioNulo();
  Termo *fim = resultado;

  /* Ignorar as cabe�as */
  a = a->prox;
  b = b->prox;

  while (a->expo != -1 && b->expo != -1) {
    /* Se o expoente de |a| for menor que o de |b| (e vice-versa),
     * adicionar o elemento no resultado e avan�ar */
    if (a->expo < b->expo) {
      InsereTermoAux(fim, a->expo, a->coef);
      fim = fim->prox;
      a = a->prox;
    } else if (b->expo < a->expo) {
      /* |a| � igual a zero => |0 - b| = |-b| */
      InsereTermoAux(fim, b->expo, -(b->coef));
      fim = fim->prox;
      b = b->prox;
    } else {
      sub = a->coef - b->coef;
      
      /* Somente adicionar se o coeficiente resultante n�o for 0 */
      if (sub != 0) {
        InsereTermoAux(fim, a->expo, sub);
        fim = fim->prox;
      }
      a = a->prox;
      b = b->prox;
    }
  }

  /* Se os polin�mios n�o possuem a mesma ordem,
   * completar com os elementos restantes */
  while (a->expo != -1) {
    InsereTermoAux(fim, a->expo, a->coef);
    fim = fim->prox;
    a = a->prox;
  }

  while (b->expo != -1) {
    InsereTermoAux(fim, b->expo, -(b->coef));
    fim = fim->prox;
    b = b->prox;
  }

  return resultado;

} /* SubPolinomios */


Polinomio MultTermo(Polinomio p, int e, float c) {
/* Devolve o polin�mio 'p' multiplicado pelo termo '(e,c)'. Sup�e       */
/* que 'c' n�o � nulo. N�o altera o polin�mio dado.                     */

  Polinomio resultado = CriaPolinomioNulo();
  Termo *fim = resultado;

  for (Termo *t = p->prox; t->expo != -1; t = t->prox) {
    InsereTermoAux(fim, t->expo + e, t->coef * c);
    fim = fim->prox;
  }

  return resultado;

} /* MultTermo */


Polinomio MultPolinomios(Polinomio a, Polinomio b) {
/* Devolve o produto dos polin�mios 'a' e 'b'. N�o altera os polin�mios */
/* dados.                                                               */

  Polinomio resultado = CriaPolinomioNulo();
  Polinomio aux1, aux2;

  for (Termo *t = a->prox; t->expo != -1; t = t->prox) {
    aux1 = MultTermo(b, t->expo, t->coef);
    aux2 = SomaPolinomios(resultado, aux1);
    LiberaPolinomio(aux1);
    LiberaPolinomio(resultado);
    resultado = aux2;
  }

  return resultado;

} /* MultPolinomios */
