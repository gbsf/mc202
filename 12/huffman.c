/* 
 * huffman.c - Implementação do algoritmo de Huffman. 
 */

/* 
 * huffman.c - Implementação do algoritmo de Huffman. 
 *             Para fazer a implementação com bits verdadeiros,
 *             contrário, do arquivo "pseudo_bits.h".
 *
 *             Opcionalmente, poderá ser usada uma implementação
 *             independente de fila de prioridade (heap) da tarefa
 *             09. Neste caso, deverá ser submetido também o arquivo
 *             "heap.c". Caso contrário, deverá ser eliminada ou
 *             comentada a linha abaixo que inclui "heap.h".
 */

#include "heap.h"

/*#include "heap.h"*/  /* descomente se necessário */

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
  int peso;                      /* peso da árvore */
  char letra;                    /* somente no caso de folha */
  TipoFilho tipoFilho;           /* indica a relação com o pai */
  ArvHuf esq,dir,pai;            /* subárvores e pai */
} RegArvHuf;



/* --------------------------------------------------------- */
/*         Variáveis globais                                 */
/* --------------------------------------------------------- */


ArvHuf Arvore;   /*  As funções abaixo construirão e utilizarão a
		     árvore que será apontada por esta variável */


ArvHuf Folhas[256];   /* Apontadores para as folhas da árvore 'Arvore' */



/* --------------------------------------------------------- */
/* Funções auxiliares para este módulo                       */
/* --------------------------------------------------------- */

ArvHuf CriaFolha(char letra, int peso) {
/* Cria uma folha da árvore; devolve NULL se não há mais memória. */

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
/* Combina duas subárvores numa única árvore cujo peso é a soma dos
   pesos das subárvores; devolve NULL se não há mais memória. */
  
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
 * Obs: essa função é tal que o menor valor 'ganha' e,
 * na implementação de Heap, o coloca antes, ou seja,
 * com maior prioridade */

  int pesoA = ((ArvHuf) a)->peso;
  int pesoB = ((ArvHuf) b)->peso;

  return pesoB - pesoA;

}

void LiberaArvoreAux(ArvHuf p){
/* Libera a memória dinâmica ocupada pela árvore 'p'. */

  if (p!=NULL) {
    LiberaArvoreAux(p->esq);
    LiberaArvoreAux(p->dir);
    FREE(p);
  }
  
} /* LiberaArvoreAux */


/* --------------------------------------------------------- */
/* Funções da interface                                      */
/* --------------------------------------------------------- */
                      
Boolean ConstroiHuffman(char txt[], int n) {
/* A partir do texto 'texto' de 'n' caracteres, constrói a árvore de
   Huffman para compressão deste texto e a guarda numa variável local
   do módulo 'huffman.c'.  Numa outra variável local guarda um vetor
   de apontadores para as folhas da árvore. Devolve 'true' se a
   construção teve sucesso; 'false' caso contrário. */

  ArvHuf floresta[256];
  int freq[256], i, n_elems = 0;
  Heap heap;
  
  /* Inicializa variáveis */
  /* Ambas são vetores estáticos, portanto o |sizeof|
   * retorna o tamanho ocupado em memória pelo vetor. */
  memset(freq, 0, sizeof freq);
  memset(Folhas, 0, sizeof Folhas);

  /* Conta a freqüência dos caracteres */
  for (i = 0; i < n; i++) {
    freq[(unsigned char) txt[i]]++;
  }

  /* Criar as folhas dos caracteres válidos e popula |floresta|
   * com os valores não-nulos  do vetor |Folhas| */
  for (i = 0; i < 256; i++)
    if (freq[i] > 0) {
      Folhas[i] = CriaFolha((char) i, freq[i]);
      if (!Folhas[i]) /* Não há memória suficiente */
          return false;
      floresta[n_elems++] = Folhas[i];
    }

  heap = CriaInicializaHeap(n_elems, ComparaPesos, (void**) floresta);

  /* Enquanto houver mais de um elemento na floresta, combinar
   * as árvores de menor peso. Como há duas remoções na heap
   * e uma inserção, o número de elementos |n_elems| diminui
   * de um a cada iteração */
  while (n_elems-- > 1) {
    ArvHuf esq = (ArvHuf) RemoveHeap(heap);
    ArvHuf dir = (ArvHuf) RemoveHeap(heap);
    ArvHuf nova = CombinaArvs(esq, dir);
    if (!nova) /* Não há memória suficiente */
        return false;
    InsereHeap(heap, nova);
  }

  /* |heap| deve possui apenas um elemento agora */
  Arvore = (ArvHuf) RemoveHeap(heap);

  LiberaHeap(heap);

  return true;
  
} /* ConstroiHuffman */

void LiberaHuffman() {
/* Libera a memória dinâmica ocupada pelas estruturas criadas por
   'ConstroiHuffman'. */

  LiberaArvoreAux(Arvore);
  /* Todas as folhas de |Folhas| estavam presentes
   * na árvore, portanto já foram liberadas.
   * Note que o conteúdo de |Folhas| aponta para
   * memória liberada, contudo esse vetor não é mais
   * acessado após chamar essa função */

} /* LiberaHuffman */


Boolean Comprime(char *txt, int n,
		 char *bits, int *numBits, int numBitsMax) {
/* Comprime os 'n' caracteres do texto 'txt' usando as estruturas já
   construídas, e deixa o resultado como uma seqüência de bits verdadeiros
   na área apontada por 'bits'. Devolve em 'numBits' o número total de
   bits gerados. Em caso de sucesso devolve 'true'; se houver mais
   de 'numBitsMax' bits, devolve 'false'. */

  unsigned short acumulador = 0;
  short proximo_bit = 8;
  int bits_usados = 0;
  ArvHuf no_atual;
  *numBits = 0;

  while (n--) {
    /* Começar na folha equivalente à letra */
    no_atual = Folhas[(unsigned char) *(txt++)];
    bits_usados = 0;
    acumulador = 0;
    /* Enquanto não chegar à raiz */
    while (no_atual->pai) {
      if (no_atual->tipoFilho == FilhoEsq) {
        /* Colocar todos os bits uma posição para a esquerda,
         * de modo a "enfileirar" o novo bit no LSB */
        acumulador <<= 1;
        /* O novo bit já é zero */
      } else if (no_atual->tipoFilho == FilhoDir) {
        acumulador <<= 1;
        /* Atribuir o novo bit para 1 */
        acumulador |= 1;
      }
      bits_usados++;
      /* Subir na árvore */
      no_atual = no_atual->pai;
    }

    /* Temos a sequência (invertida!) dos bits correspondentes
     * ao caracter atual em |acumulador|. Passar agora os bits
     * na ordem correta para o vetor de saída. */

    /* Enquanto sobrarem bits no acumulador */ 
    while (bits_usados--) {
      if (++(*numBits) > numBitsMax)
        return false; /* Número de bits excede o máximo */

      /* "Desenfileirar" um bit do acumulador e colocar
       * na posição correta na saída */
      *bits |= (acumulador & 1) << --proximo_bit;
      acumulador >>= 1;

      /* Se já usamos todos os bits desse byte, passar para o próximo */
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
/* Descomprime a cadeia de bits verdadeiros na área
   apontada por 'bits', de comprimento 'numBits' seguindo a árvore de
   Huffman já construída.  Em caso de sucesso devolve 'true'; se
   aparecer uma codificação incompatível com a árvore, ou se houver
   mais caracteres que 'tamMaxTxt' devolve 'false'.*/

  unsigned char byte = *bits;
  /* Número de bits restantes no byte */
  int bit_count = 8;
  /* Nó atual da árvore */
  ArvHuf no_atual = Arvore;
  *n = 0;

  /* Enquanto houver bits à processar */
  while (numBits--) {
    /* Pegar o bit mais significativo: */
    if (byte & 0x80) /* Se for o bit '1' */
      /* Descer à direita */
      no_atual = no_atual->dir;
    else
      /* Senão descer à esquerda */
      no_atual = no_atual->esq;

    if (!no_atual)
      /* Desceu por um caminho inválido, erro na codificação */
      return false;

    /* Se for um nó folha */
    if (!no_atual->esq && !no_atual->dir) {
      if (*n + 1 > tamMaxTxt)
        return false; /* Passou do tamanho máximo */

      /* Colocar a letra equivalente na saída e retornar ao
       * início da árvore */
      txt[(*n)++] = no_atual->letra;
      no_atual = Arvore;
    }

    /* Passar todos os bits para a esquerda,
     * descartando o MSB, que já foi lido */
    byte <<= 1;
    if (--bit_count == 0) { /* Todos os bits já foram lidos */
      /* Pegar o próximo byte */
      byte = *(++bits);
      bit_count = 8;
    }
  }

  return true;
   
} /* Descomprime */

/* --------------------------------------------------------- */
/* Funções auxiliares para os testes pelo programa principal */
/* Não modifique estas declarações                           */
/* --------------------------------------------------------- */

#define DESLOCA 5

char buf[10];  /* O buffer de impressão será fixo de até 9 caracteres e \0 */

ArvHuf esq(ArvHuf p, int nivel) { return p->esq; }
ArvHuf dir(ArvHuf p, int nivel) { return p->dir; }

char *info(ArvHuf p, int nivel) { 
  if ((p->esq==NULL)&&(p->dir==NULL))  /* folha */
    sprintf(buf,"%4d:%c",p->peso,p->letra);
  else  /* nó interno */
    sprintf(buf,"%4d",p->peso);
  return buf;
}

void ImprimeHuffman() {
/* Imprime a árvore de Huffman num formato legível */

  ImprimeArvore(Arvore,(subarvfunc*)esq,(subarvfunc*)dir,(infofunc*)info,NULL,DESLOCA,"Árvore vazia");
  

} /* ImprimeArvore */

Boolean PseudoBits() {
/* Informa se foram usados pseudo-bits ou bits verdadeiros. */
#ifdef PSEUDO_BITS
  return true;
#else
  return false;
#endif
}

