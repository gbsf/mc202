/* Programa: selas.c                                   */
/* Autor: Gabriel Souza Franco            RA155477     */
/* Disciplina: MC202                      Turma F      */
/* Data: 05/set/2014                                   */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool matriz_ler(size_t tam, int matriz[tam][tam]);

int main(void) {
    int ordem = 0;

    fscanf(stdin, "%d", &ordem);
    // Ignorar todos os caracteres at� o \n, e ent�o consum�-lo
    fscanf(stdin, "%*[^\n]%*c");

    if (ordem < 1 || ordem > 20) {
        fprintf(stdout, "Dados inv�lidos\n");
        return 0;
    }

    int matriz[ordem][ordem];
    int maior[ordem];
    int menor[ordem];

    if (!matriz_ler(ordem, matriz)) {
        fprintf(stdout, "Dados inv�lidos\n");
        return 0;
    }

    for (int i = 0; i < ordem; i++) {
        menor[i] = matriz[i][0];
        for (int j = 0; j < ordem; j++)
            if (matriz[i][j] < menor[i])
                menor[i] = matriz[i][j];

        maior[i] = matriz[0][i];
        for (int j = 0; j < ordem; j++)
            if (matriz[j][i] > maior[i])
                maior[i] = matriz[j][i];
    }

    bool vazio = true;

    fprintf(stdout, "Os pontos de sela da matriz s�o:\n\n");

    for (int i = 0; i < ordem; i++)
        for (int j = 0; j < ordem; j++)
            if (menor[i] == maior[j]) {
                fprintf(stdout, "%4d%4d%4d\n", i, j, matriz[i][j]);
                vazio = false;
            }

    if (vazio)
        fprintf(stdout, "    nenhum\n");

    return 0;
}

/**
 * L� a matriz de tamanho tam x tam,
 * retornando `true` se a opera��o ocorreu normalmente,
 * `false` caso contr�rio.
 *
 */
bool matriz_ler(size_t tam, int matriz[tam][tam]) {
    for (size_t i = 0; i < tam; i++) {
        for (size_t j = 0; j < tam; j++)
            if (1 != fscanf(stdin, "%d", &matriz[i][j]))
                return false;
        fscanf(stdin, "%*[^\n]%*c");
    }

    return true;
}
