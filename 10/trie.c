/*
  Autor:         Gabriel Souza Franco
  RA:            155477
  Disciplina:    MC202
  Turmas:        F
  
  Tarefa 10
  Segundo semestre de 2014

*/
/* Arquivo trie.c:

   Implementa��o de �rvores digitais (ADs) atrav�s de um tipo
   abstrato de dados usando apontadores "void *".

   Todas as defini��es est�o no arquivo "trie.h".

*/

#include <stdio.h>
#include <string.h>

#include "trie.h"
#include "balloc.h"

typedef struct RegTrie * ImplTrie;

typedef struct RegTrie {
  Boolean fim;                 /* t�rmino de uma cadeia */
  ImplTrie subarv[TAM_ALFABETO]; /* vetor de sub�rvores */
} RegTrie;

/* Implementa��o b�sica de pilha para a fun��o |removeAD| */
typedef struct RegPilha *Pilha;
typedef struct RegPilha {
  ImplTrie *info;
  Pilha prox;
} RegPilha;

Pilha criaPilha();
void inserePilha(Pilha* p, ImplTrie *info);
ImplTrie *removePilha(Pilha *p);
Boolean pilhaVazia(Pilha p);
void liberaPilha(Pilha p);

ImplTrie criaInicializaNo() {
/* Devolve um n� com a marca de fim de cadeia falsa e todas as
   sub�rvores vazias; usada para criar a AD inicial e durante a
   inser��o.
*/
   
  ImplTrie p = MALLOC(sizeof *p);
  if (!p) {
    printf("Mem�ria esgotada\n");
    exit(0);
  }
  p->fim = false;
  /* Zera os apontadores */
  memset(p->subarv, 0, TAM_ALFABETO * sizeof p->subarv[0]);
  return p;

}

Boolean livre(ImplTrie t) {
/* Devolve verdadeiro se o n� 't' n�o � final e � uma folha; usada
   durante a remo��o.
*/
  
  for (int i = 0; i < TAM_ALFABETO; i++)
    if (t->subarv[i])
      return false;
  return !t->fim;

}

void percorreAux(ImplTrie t, funcVisita *v, char *buf, int m) {
/* Percorre a AD em ordem lexicogr�fica; constroi as cadeias em 'buf'
  e aplica a fun��o 'v' quando termina uma cadeia, colocando no fim o
  caractere '\0'; 'm' � o �ndice do pr�ximo caractere em 'buf'.
*/

  if (t->fim) {
    buf[m] = '\0';
    v(buf);
  }

  for (int i = 0; i < TAM_ALFABETO; i++)
    if (t->subarv[i]) {
      /* Transforma o �ndice em um caracter [a-z] */
      buf[m] = i + 'a';
      percorreAux(t->subarv[i], v, buf, m + 1);
    }

}

Trie CriaAD() {
  
  return criaInicializaNo();
  
}


Boolean consultaAD(Trie t, char *s) {

  ImplTrie it = (ImplTrie) t;
  ImplTrie prox;

  while (*s != '\0') {
    /* Acessa o apontador referente ao pr�ximo caracter */
    prox = it->subarv[*s - 'a'];
    if (prox) {
      it = prox;
      s++;
    } else {
      break;
    }
  }
  
  return *s == '\0' && it->fim;

}


Boolean insereAD(Trie t, char *s) {

  ImplTrie it = (ImplTrie) t;
  ImplTrie *prox;

  /* Se a cadeia j� se encontra na �rvore, retornar falha para a inser��o */
  if (consultaAD(t, s))
    return false;

  /* Enquanto n�o chega o fim da cadeia, descer na �rvore,
   * criando novos n�s se necess�rio */
  while (*s != '\0') {
    prox = &(it->subarv[*s - 'a']);
    if (!(*prox))
      *prox = criaInicializaNo();
    it = *prox;
    s++;
  }

  it->fim = true;
  
  return true;

}


int numCadeiasAD(Trie t) {

  ImplTrie it = (ImplTrie) t;
  int num = 0;
  
  /* Soma todas as cadeias que terminam em algum lugar das sub�rvores */
  for (int i = 0; i < TAM_ALFABETO; i++)
    if (it->subarv[i])
      num += numCadeiasAD((Trie) it->subarv[i]);

  /* Se esse n� termina uma cadeia, aumentar a contagem */
  if (it->fim)
    num++;

  return num;

}


Boolean removeAD(Trie t, char *s) {

  ImplTrie it = (ImplTrie) t;
  ImplTrie *prox;
  Pilha p = criaPilha();

  while (*s != '\0') {
    /* Acessa o apontador referente ao pr�ximo caracter */
    prox = &(it->subarv[*s - 'a']);
    if (*prox) {
      inserePilha(&p, prox);
      it = *prox;
      s++;
    } else {
      liberaPilha(p);
      return false;
    }
  }

  /* Se n�o chegou no fim da cadeia, a mesma n�o se encontra na �rvore */
  if (*s != '\0') {
    liberaPilha(p);
    return false;
  }

  /* Remover o marcador de fim de cadeia */
  it->fim = false;

  /* Retornar pelo caminho realizado na �rvore,
   * liberando os n�s desnecess�rios */
  while (!pilhaVazia(p)) {
    prox = removePilha(&p);

    if (livre(*prox)) {
      liberaAD(*prox);
      *prox = NULL;
    } else {
      break;
    }
  }

  liberaPilha(p);
  
  return true;

}


void percorreAD(Trie t, funcVisita *v) {
  
  char buf[TAM_MAX_CADEIA];
  
  percorreAux((ImplTrie)t,v,buf,0);
  
}

void liberaAD(Trie t) {

  ImplTrie it = (ImplTrie) t;

  for (int i = 0; i < TAM_ALFABETO; i++)
    if (it->subarv[i])
      liberaAD(it->subarv[i]);

  FREE(it);

}


/* 
  Fun��es usadas somente para verifica��o; conceitualmente
  n�o fazem parte da implementa��o.
*/

void numNosADAux(ImplTrie t, int *num) {

  (*num)++;

  for (int i = 0; i < TAM_ALFABETO; i++)
    if (t->subarv[i])
      numNosADAux(t->subarv[i], num);

}
   
int numNosAD(Trie t) {

  int num = 0;
  
  numNosADAux((ImplTrie) t, &num);

  return num;

}

int alturaADAux(ImplTrie t, int inicial) {

  int altura = inicial;
  int resultado;

  for (int i = 0; i < TAM_ALFABETO; i++)
    if (t->subarv[i]) {
      resultado = alturaADAux(t->subarv[i], inicial + 1);

      if (resultado > altura)
        altura = resultado;
    }
  

  return altura;

}

int alturaAD(Trie t) {

  return alturaADAux((ImplTrie) t, 1);

}

/* ================================================================ */
/*                Fun��es da implementa��o de pilha                 */

Pilha criaPilha() {

  return NULL;

}

void inserePilha(Pilha* p, ImplTrie *info) {

  RegPilha *reg = MALLOC(sizeof *reg);

  reg->info = info;
  reg->prox = *p;

  *p = reg;

}

ImplTrie *removePilha(Pilha *p) {

  RegPilha *reg = *p;
  ImplTrie *ret = reg->info;

  *p = reg->prox;
  FREE(reg);

  return ret;

}

Boolean pilhaVazia(Pilha p) {

  return p == NULL;

}

void liberaPilha(Pilha p) {

  if (p) {
    Pilha prox = p->prox;
    FREE(p);
    liberaPilha(prox);
  }

}

