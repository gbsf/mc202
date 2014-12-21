/* 
 * huffman.c - Implementa��o do algoritmo de Huffman. 
 */

/* 
 * huffman.c - Implementa��o do algoritmo de Huffman. 
 *             Para fazer a implementa��o com bits verdadeiros,
 *             contr�rio, do arquivo "pseudo_bits.h".
 *
 *             Opcionalmente, poder� ser usada uma implementa��o
 *             independente de fila de prioridade (heap) da tarefa
 *             09. Neste caso, dever� ser submetido tamb�m o arquivo
 *             "heap.c". Caso contr�rio, dever� ser eliminada ou
 *             comentada a linha abaixo que inclui "heap.h".
 */

#include "heap.h"

/*#include "heap.h"*/  /* descomente se necess�rio */

/* Comente esta linha se quiser usar bits verdadeiros */
/*#define PSEUDO_BITS*/


#include <string.h>
#include <stdio.h>
#include "huffman.h"
#include "balloc.h"
#include "imprimearvore.h"

/* --------------------------------------------------------- */
/*         Tipos                                             */
/* --------------------------------------------------------- */


typedef struct RegArvHuf *ArvHuf;

typedef enum TipoFilho { 
  FilhoEsq = 0, FilhoDir = 1, Indeterminado } TipoFilho;

typedef struct RegArvHuf {
  int peso;                      /* peso da �rvore */
  char letra;                    /* somente no caso de folha */
  TipoFilho tipoFilho;           /* indica a rela��o com o pai */
  ArvHuf esq,dir,pai;            /* sub�rvores e pai */
} RegArvHuf;



/* --------------------------------------------------------- */
/*         Vari�veis globais                                 */
/* --------------------------------------------------------- */


ArvHuf Arvore;   /*  As fun��es abaixo construir�o e utilizar�o a
		     �rvore que ser� apontada por esta vari�vel */


ArvHuf Folhas[256];   /* Apontadores para as folhas da �rvore 'Arvore' */



/* --------------------------------------------------------- */
/* Fun��es auxiliares para este m�dulo                       */
/* --------------------------------------------------------- */

ArvHuf CriaFolha(char letra, int peso) {
/* Cria uma folha da �rvore; devolve NULL se n�o h� mais mem�ria. */

  ArvHuf p = MALLOC(sizeof(RegArvHuf));
  if (p!=NULL) {
    p->esq = p->dir = NULL;
    p->letra = letra;
    p->peso = peso;
    p->tipoFilho = Indeterminado;
  }
  return p;

} /* CriaFolha */

ArvHuf CombinaArvs(ArvHuf esq, ArvHuf dir) {
/* Combina duas sub�rvores numa �nica �rvore cujo peso � a soma dos
   pesos das sub�rvores; devolve NULL se n�o h� mais mem�ria. */
  
  ArvHuf p=MALLOC(sizeof(RegArvHuf));
  if (p!=NULL) {
    p->pai = NULL;
    p->esq = esq;
    p->dir = dir;
    p->peso = esq->peso+dir->peso;
    esq->pai = dir->pai = p;
    esq->tipoFilho = FilhoEsq;
    dir->tipoFilho = FilhoDir;
  }
  return p;

} /* CombinaArvs */

int ComparaPesos(void *a, void *b) {
/* Compara dois elementos de tipo |ArvHuf| e retorna:
 * <0 se a > b; 0 se a == b; >0 se a < b.
 * Obs: essa fun��o � tal que o menor valor 'ganha' e,
 * na implementa��o de Heap, o coloca antes, ou seja,
 * com maior prioridade */

  int pesoA = ((ArvHuf) a)->peso;
  int pesoB = ((ArvHuf) b)->peso;

  return pesoB - pesoA;

}

void LiberaArvoreAux(ArvHuf p){
/* Libera a mem�ria din�mica ocupada pela �rvore 'p'. */

  if (p!=NULL) {
    LiberaArvoreAux(p->esq);
    LiberaArvoreAux(p->dir);
    FREE(p);
  }
  
} /* LiberaArvoreAux */


/* --------------------------------------------------------- */
/* Fun��es da interface                                      */
/* --------------------------------------------------------- */
                      
Boolean ConstroiHuffman(char txt[], int n) {
/* A partir do texto 'texto' de 'n' caracteres, constr�i a �rvore de
   Huffman para compress�o deste texto e a guarda numa vari�vel local
   do m�dulo 'huffman.c'.  Numa outra vari�vel local guarda um vetor
   de apontadores para as folhas da �rvore. Devolve 'true' se a
   constru��o teve sucesso; 'false' caso contr�rio. */

  ArvHuf floresta[256];
  int freq[256], i, n_elems = 0;
  Heap heap;
  
  /* Inicializa vari�veis */
  /* Ambas s�o vetores est�ticos, portanto o |sizeof|
   * retorna o tamanho ocupado em mem�ria pelo vetor. */
  memset(freq, 0, sizeof freq);
  memset(Folhas, 0, sizeof Folhas);

  /* Conta a freq��ncia dos caracteres */
  for (i = 0; i < n; i++) {
    freq[(unsigned char) txt[i]]++;
  }

  /* Criar as folhas dos caracteres v�lidos e popula |floresta|
   * com os valores n�o-nulos  do vetor |Folhas| */
  for (i = 0; i < 256; i++)
    if (freq[i] > 0) {
      Folhas[i] = CriaFolha((char) i, freq[i]);
      if (!Folhas[i]) /* N�o h� mem�ria suficiente */
          return false;
      floresta[n_elems++] = Folhas[i];
    }

  heap = CriaInicializaHeap(n_elems, ComparaPesos, (void**) floresta);

  /* Enquanto houver mais de um elemento na floresta, combinar
   * as �rvores de menor peso. Como h� duas remo��es na heap
   * e uma inser��o, o n�mero de elementos |n_elems| diminui
   * de um a cada itera��o */
  while (n_elems-- > 1) {
    ArvHuf esq = (ArvHuf) RemoveHeap(heap);
    ArvHuf dir = (ArvHuf) RemoveHeap(heap);
    ArvHuf nova = CombinaArvs(esq, dir);
    if (!nova) /* N�o h� mem�ria suficiente */
        return false;
    InsereHeap(heap, nova);
  }

  /* |heap| deve possui apenas um elemento agora */
  Arvore = (ArvHuf) RemoveHeap(heap);

  LiberaHeap(heap);

  return true;
  
} /* ConstroiHuffman */

void LiberaHuffman() {
/* Libera a mem�ria din�mica ocupada pelas estruturas criadas por
   'ConstroiHuffman'. */

  LiberaArvoreAux(Arvore);
  /* Todas as folhas de |Folhas| estavam presentes
   * na �rvore, portanto j� foram liberadas.
   * Note que o conte�do de |Folhas| aponta para
   * mem�ria liberada, contudo esse vetor n�o � mais
   * acessado ap�s chamar essa fun��o */

} /* LiberaHuffman */


Boolean Comprime(char *txt, int n,
		 char *bits, int *numBits, int numBitsMax) {
/* Comprime os 'n' caracteres do texto 'txt' usando as estruturas j�
   constru�das, e deixa o resultado como uma seq��ncia de bits verdadeiros
   na �rea apontada por 'bits'. Devolve em 'numBits' o n�mero total de
   bits gerados. Em caso de sucesso devolve 'true'; se houver mais
   de 'numBitsMax' bits, devolve 'false'. */

  unsigned short acumulador = 0;
  short proximo_bit = 8;
  int bits_usados = 0;
  ArvHuf no_atual;
  *numBits = 0;

  while (n--) {
    /* Come�ar na folha equivalente � letra */
    no_atual = Folhas[(unsigned char) *(txt++)];
    bits_usados = 0;
    acumulador = 0;
    /* Enquanto n�o chegar � raiz */
    while (no_atual->pai) {
      if (no_atual->tipoFilho == FilhoEsq) {
        /* Colocar todos os bits uma posi��o para a esquerda,
         * de modo a "enfileirar" o novo bit no LSB */
        acumulador <<= 1;
        /* O novo bit j� � zero */
      } else if (no_atual->tipoFilho == FilhoDir) {
        acumulador <<= 1;
        /* Atribuir o novo bit para 1 */
        acumulador |= 1;
      }
      bits_usados++;
      /* Subir na �rvore */
      no_atual = no_atual->pai;
    }

    /* Temos a sequ�ncia (invertida!) dos bits correspondentes
     * ao caracter atual em |acumulador|. Passar agora os bits
     * na ordem correta para o vetor de sa�da. */

    /* Enquanto sobrarem bits no acumulador */ 
    while (bits_usados--) {
      if (++(*numBits) > numBitsMax)
        return false; /* N�mero de bits excede o m�ximo */

      /* "Desenfileirar" um bit do acumulador e colocar
       * na posi��o correta na sa�da */
      *bits |= (acumulador & 1) << --proximo_bit;
      acumulador >>= 1;

      /* Se j� usamos todos os bits desse byte, passar para o pr�ximo */
      if (proximo_bit == 0) {
        bits++;
        proximo_bit = 8;
      }
    }
  }

  return true;
  
} /* Comprime */

   
Boolean Descomprime(char *txt, int *n,
		    char *bits, int numBits, int tamMaxTxt) {
/* Descomprime a cadeia de bits verdadeiros na �rea
   apontada por 'bits', de comprimento 'numBits' seguindo a �rvore de
   Huffman j� constru�da.  Em caso de sucesso devolve 'true'; se
   aparecer uma codifica��o incompat�vel com a �rvore, ou se houver
   mais caracteres que 'tamMaxTxt' devolve 'false'.*/

  unsigned char byte = *bits;
  /* N�mero de bits restantes no byte */
  int bit_count = 8;
  /* N� atual da �rvore */
  ArvHuf no_atual = Arvore;
  *n = 0;

  /* Enquanto houver bits � processar */
  while (numBits--) {
    /* Pegar o bit mais significativo: */
    if (byte & 0x80) /* Se for o bit '1' */
      /* Descer � direita */
      no_atual = no_atual->dir;
    else
      /* Sen�o descer � esquerda */
      no_atual = no_atual->esq;

    if (!no_atual)
      /* Desceu por um caminho inv�lido, erro na codifica��o */
      return false;

    /* Se for um n� folha */
    if (!no_atual->esq && !no_atual->dir) {
      if (*n + 1 > tamMaxTxt)
        return false; /* Passou do tamanho m�ximo */

      /* Colocar a letra equivalente na sa�da e retornar ao
       * in�cio da �rvore */
      txt[(*n)++] = no_atual->letra;
      no_atual = Arvore;
    }

    /* Passar todos os bits para a esquerda,
     * descartando o MSB, que j� foi lido */
    byte <<= 1;
    if (--bit_count == 0) { /* Todos os bits j� foram lidos */
      /* Pegar o pr�ximo byte */
      byte = *(++bits);
      bit_count = 8;
    }
  }

  return true;
   
} /* Descomprime */

/* --------------------------------------------------------- */
/* Fun��es auxiliares para os testes pelo programa principal */
/* N�o modifique estas declara��es                           */
/* --------------------------------------------------------- */

#define DESLOCA 5

char buf[10];  /* O buffer de impress�o ser� fixo de at� 9 caracteres e \0 */

ArvHuf esq(ArvHuf p, int nivel) { return p->esq; }
ArvHuf dir(ArvHuf p, int nivel) { return p->dir; }

char *info(ArvHuf p, int nivel) { 
  if ((p->esq==NULL)&&(p->dir==NULL))  /* folha */
    sprintf(buf,"%4d:%c",p->peso,p->letra);
  else  /* n� interno */
    sprintf(buf,"%4d",p->peso);
  return buf;
}

void ImprimeHuffman() {
/* Imprime a �rvore de Huffman num formato leg�vel */

  ImprimeArvore(Arvore,(subarvfunc*)esq,(subarvfunc*)dir,(infofunc*)info,NULL,DESLOCA,"�rvore vazia");
  

} /* ImprimeArvore */

Boolean PseudoBits() {
/* Informa se foram usados pseudo-bits ou bits verdadeiros. */
#ifdef PSEUDO_BITS
  return true;
#else
  return false;
#endif
}

