/* Este módulo implementa operações sobre polinômios em uma variável.   */
/* Cada polinômio é representado por uma lista ligada circular, com nó  */
/* cabeça. Os nós da lista representam os termos não nulos do polinômio */
/* em ordem crescente dos seus expoentes. O nó cabeça tem expoente '-1'.*/

#include "polinomios.h"
#include "balloc.h"
#include <stdio.h>

/*-----------------------  Função auxiliar  ----------------------------*/

void InsereTermoAux(Polinomio p, int e, float c) {
/* Insere o termo '(e,c)' após o nó apontado por 'p'.                   */

  Termo *novo = MALLOC(sizeof(Termo));
  novo->expo = e;
  novo->coef = c;
  novo->prox = p->prox;

  p->prox = novo;

} /* InsereTermoAux */



/*---------------- Implementação das funções da interface --------------*/


void ImprimePolinomio(Polinomio p) {
/* Imprime, em ordem crescente dos expoentes, os termos do              */
/* não nulos do 'p'. No caso do polinômio identicamente nulo,           */
/* imprime as palavras "Polinômio nulo".                                */

  if (p==NULL) {
    printf("Polinômio inválido\n");
    return;
  }

  if (p->prox==p) {
    printf("Polinômio nulo\n");
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
/* Devolve um novo polinômio identicamente nulo.                        */

  Polinomio polinomio = MALLOC(sizeof(Termo));
  polinomio->expo = -1;
  polinomio->coef = 0;
  polinomio->prox = polinomio;

  return polinomio;

} /* CriaPolinomioNulo */



void LiberaPolinomio(Polinomio p) {
/* Libera toda a memória dinâmica ocupada por um polinômio.             */

  Termo *prox;

  /* Pular a cabeça por enquanto */
  for (Termo *t = p->prox; t != p;) {
    prox = t->prox;
    FREE(t);
    t = prox;
  }

  /* Liberar a cabeça */
  FREE(p);

}

Boolean PolinomioNulo(Polinomio p) {
/* Verifica se o polinômio 'p' é identicamente nulo.                    */

  return p->prox == p;

} /* PolinomioNulo */


void InsereTermo(Polinomio p, int e, float c) {
/* Insere no polinomio 'p' o termo '(e,c)', de maneira a manter os      */
/* termos ordenados. Supõe que não existe ainda em 'p' um termo com     */
/* expoente 'e', e que 'c' não é zero.                                  */

  while (p->prox->expo != -1 && p->prox->expo < e)
    p = p->prox;

  InsereTermoAux(p, e, c);

} /* InsereTermo */


Polinomio SomaPolinomios(Polinomio a, Polinomio b) {
/* Devolve a soma dos polinômios 'a' e 'b'. Não altera os polinômios    */
/* dados. Termos nulos não são inseridos no resultados.                 */

  float soma;
  Polinomio resultado = CriaPolinomioNulo();
  Termo *fim = resultado;

  /* Ignorar as cabeças */
  a = a->prox;
  b = b->prox;

  while (a->expo != -1 && b->expo != -1) {
    /* Se o expoente de |a| for menor que o de |b| (e vice-versa),
     * adicionar o elemento no resultado e avançar */
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
      
      /* Somente adicionar se o coeficiente resultante não for 0 */
      if (soma != 0) {
        InsereTermoAux(fim, a->expo, soma);
        fim = fim->prox;
      }
      a = a->prox;
      b = b->prox;
    }
  }

  /* Se os polinômios não possuem a mesma ordem,
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
/* Devolve a diferença entre o polinômio 'a' e o polinômio 'b'. Não      */
/* altera os polinômios dados. Termos nulos não são inseridos no         */
/* resultado.                                                            */

  float sub;
  Polinomio resultado = CriaPolinomioNulo();
  Termo *fim = resultado;

  /* Ignorar as cabeças */
  a = a->prox;
  b = b->prox;

  while (a->expo != -1 && b->expo != -1) {
    /* Se o expoente de |a| for menor que o de |b| (e vice-versa),
     * adicionar o elemento no resultado e avançar */
    if (a->expo < b->expo) {
      InsereTermoAux(fim, a->expo, a->coef);
      fim = fim->prox;
      a = a->prox;
    } else if (b->expo < a->expo) {
      /* |a| é igual a zero => |0 - b| = |-b| */
      InsereTermoAux(fim, b->expo, -(b->coef));
      fim = fim->prox;
      b = b->prox;
    } else {
      sub = a->coef - b->coef;
      
      /* Somente adicionar se o coeficiente resultante não for 0 */
      if (sub != 0) {
        InsereTermoAux(fim, a->expo, sub);
        fim = fim->prox;
      }
      a = a->prox;
      b = b->prox;
    }
  }

  /* Se os polinômios não possuem a mesma ordem,
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
/* Devolve o polinômio 'p' multiplicado pelo termo '(e,c)'. Supõe       */
/* que 'c' não é nulo. Não altera o polinômio dado.                     */

  Polinomio resultado = CriaPolinomioNulo();
  Termo *fim = resultado;

  for (Termo *t = p->prox; t->expo != -1; t = t->prox) {
    InsereTermoAux(fim, t->expo + e, t->coef * c);
    fim = fim->prox;
  }

  return resultado;

} /* MultTermo */


Polinomio MultPolinomios(Polinomio a, Polinomio b) {
/* Devolve o produto dos polinômios 'a' e 'b'. Não altera os polinômios */
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
