#ifndef ARVORE_AVL_H
#define ARVORE_AVL_H

#include "tabela_simbolos.h"

typedef struct NoAVL {
    char *chave;
    InfoSimbolo info;
    int altura;              /* altura da subarvore enraizada neste no */
    struct NoAVL *esq, *dir;
} NoAVL;

typedef struct {
    NoAVL *raiz;
    long n;                  /* numero de chaves */
    long rotacoes;           /* contador de rotacoes */
} ArvoreAVL;

ArvoreAVL   *avl_criar(void);
void         avl_inserir(ArvoreAVL *a, const char *chave, int linha);
InfoSimbolo *avl_buscar(ArvoreAVL *a, const char *chave);
int          avl_remover(ArvoreAVL *a, const char *chave);
void         avl_metricas(ArvoreAVL *a, Metricas *out);
void         avl_destruir(ArvoreAVL *a);

#endif