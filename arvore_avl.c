#include <stdlib.h>
#include <string.h>
#include "arvore_avl.h"

static int altura(NoAVL *no) { return no ? no->altura : 0; }
static int maior(int a, int b) { return a > b ? a : b; }

static void atualiza_altura(NoAVL *no) {
    no->altura = 1 + maior(altura(no->esq), altura(no->dir));
}

/* balanco = h(esq) - h(dir); fora de [-1,1] indica no desregulado. */
static int balanco(NoAVL *no) {
    return no ? altura(no->esq) - altura(no->dir) : 0;
}

static NoAVL *rotacao_direita(NoAVL *p, long *rot) {
    NoAVL *q = p->esq;
    p->esq = q->dir;
    q->dir = p;
    atualiza_altura(p);
    atualiza_altura(q);
    (*rot)++;
    return q;
}

static NoAVL *rotacao_esquerda(NoAVL *p, long *rot) {
    NoAVL *q = p->dir;
    p->dir = q->esq;
    q->esq = p;
    atualiza_altura(p);
    atualiza_altura(q);
    (*rot)++;
    return q;
}

static NoAVL *rebalancear(NoAVL *no, long *rot) {
    atualiza_altura(no);
    int b = balanco(no);
    if (b > 1) {                         /* pesado a esquerda */
        if (balanco(no->esq) < 0)        /* caso esquerda-direita */
            no->esq = rotacao_esquerda(no->esq, rot);
        return rotacao_direita(no, rot);
    }
    if (b < -1) {                        /* pesado a direita */
        if (balanco(no->dir) > 0)        /* caso direita-esquerda */
            no->dir = rotacao_direita(no->dir, rot);
        return rotacao_esquerda(no, rot);
    }
    return no;
}

static NoAVL *novo_no(const char *chave, int linha) {
    NoAVL *no = malloc(sizeof(NoAVL));
    no->chave = dup_str(chave);
    no->info.ocorrencias = 1;
    no->info.primeira_linha = linha;
    no->altura = 1;
    no->esq = no->dir = NULL;
    return no;
}

static NoAVL *inserir_rec(NoAVL *no, const char *chave, int linha,
                          long *rot, long *n) {
    if (!no) { (*n)++; return novo_no(chave, linha); }
    int cmp = strcmp(chave, no->chave);
    if (cmp < 0) no->esq = inserir_rec(no->esq, chave, linha, rot, n);
    else if (cmp > 0) no->dir = inserir_rec(no->dir, chave, linha, rot, n);
    else { no->info.ocorrencias++; return no; } /* chave ja existe */
    return rebalancear(no, rot);
}

static NoAVL *menor_no(NoAVL *no) {
    while (no->esq) no = no->esq;
    return no;
}

static NoAVL *remover_rec(NoAVL *no, const char *chave,
                          long *rot, long *n, int *removeu) {
    if (!no) return NULL;
    int cmp = strcmp(chave, no->chave);
    if (cmp < 0) no->esq = remover_rec(no->esq, chave, rot, n, removeu);
    else if (cmp > 0) no->dir = remover_rec(no->dir, chave, rot, n, removeu);
    else {
        *removeu = 1;
        if (!no->esq || !no->dir) {          /* 0 ou 1 filho */
            NoAVL *filho = no->esq ? no->esq : no->dir;
            free(no->chave);
            free(no);
            (*n)--;
            return filho;
        }
        /* 2 filhos: substitui pelo sucessor em-ordem */
        NoAVL *suc = menor_no(no->dir);
        free(no->chave);
        no->chave = dup_str(suc->chave);
        no->info = suc->info;
        int aux = 0;
        no->dir = remover_rec(no->dir, suc->chave, rot, n, &aux);
    }
    return rebalancear(no, rot);
}

ArvoreAVL *avl_criar(void) {
    ArvoreAVL *a = malloc(sizeof(ArvoreAVL));
    a->raiz = NULL;
    a->n = 0;
    a->rotacoes = 0;
    return a;
}

void avl_inserir(ArvoreAVL *a, const char *chave, int linha) {
    a->raiz = inserir_rec(a->raiz, chave, linha, &a->rotacoes, &a->n);
}

InfoSimbolo *avl_buscar(ArvoreAVL *a, const char *chave) {
    NoAVL *no = a->raiz;
    while (no) {
        int cmp = strcmp(chave, no->chave);
        if (cmp == 0) return &no->info;
        no = cmp < 0 ? no->esq : no->dir;
    }
    return NULL;
}

int avl_remover(ArvoreAVL *a, const char *chave) {
    int removeu = 0;
    a->raiz = remover_rec(a->raiz, chave, &a->rotacoes, &a->n, &removeu);
    return removeu;
}

void avl_metricas(ArvoreAVL *a, Metricas *out) {
    out->rotacoes = a->rotacoes;
    out->colisoes = 0;
    out->altura = altura(a->raiz);
    out->comprimento_max = 0;
    out->fator_carga = 0.0;
    /* Memoria: n nos (chave ~ 12 bytes + struct com 2 ponteiros + altura). */
    out->memoria_bytes = a->n * (long)(sizeof(NoAVL) + 12);
}

static void destruir_rec(NoAVL *no) {
    if (!no) return;
    destruir_rec(no->esq);
    destruir_rec(no->dir);
    free(no->chave);
    free(no);
}

void avl_destruir(ArvoreAVL *a) {
    destruir_rec(a->raiz);
    free(a);
}