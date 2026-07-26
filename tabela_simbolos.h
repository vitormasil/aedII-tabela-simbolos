#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include <stdlib.h>
#include <string.h>

/* Copia portavel de string (substitui strdup, que e POSIX e nem sempre
 * esta disponivel no MinGW/Windows sob C padrao). */
static char *dup_str(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

/* Informacao associada a cada chave (o "valor" da tabela). */
typedef struct {
    int ocorrencias;   /* quantas vezes a chave foi inserida/vista */
    int primeira_linha;/* posicao da primeira ocorrencia */
} InfoSimbolo;

/* Metricas coletadas durante os experimentos. */
typedef struct {
    long rotacoes;      /* AVL: numero total de rotacoes (0 para hash) */
    long colisoes;      /* Hash: numero total de colisoes (0 para AVL) */
    int  altura;        /* AVL: altura final da arvore (0 para hash) */
    long comprimento_max;/* Hash: maior comprimento de lista encadeada */
    double fator_carga; /* Hash: alfa = n/m (0 para AVL) */
    long memoria_bytes; /* estimativa de memoria ocupada */
} Metricas;

#endif