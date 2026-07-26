#include <stdlib.h>
#include <string.h>
#include "tabela_hash.h"

/* Testa primalidade de forma simples (suficiente para dimensionar a tabela). */
static int eh_primo(long x) {
    if (x < 2) return 0;
    if (x % 2 == 0) return x == 2;
    for (long i = 3; i * i <= x; i += 2)
        if (x % i == 0) return 0;
    return 1;
}

/* Menor primo >= x. Primos reduzem aglomeracao no metodo da divisao. */
long proximo_primo(long x) {
    if (x < 2) return 2;
    while (!eh_primo(x)) x++;
    return x;
}

/* Hash polinomial (djb2) da chave textual -> inteiro nao negativo. */
static unsigned long hash_texto(const char *s) {
    unsigned long h = 5381;
    int c;
    while ((c = (unsigned char)*s++))
        h = ((h << 5) + h) + c;   /* h * 33 + c */
    return h;
}

/* Metodo da divisao: endereco-base = valor mod m. */
static long endereco(TabelaHash *t, const char *chave) {
    return (long)(hash_texto(chave) % (unsigned long)t->m);
}

TabelaHash *th_criar(long dimensao) {
    TabelaHash *t = malloc(sizeof(TabelaHash));
    t->m = proximo_primo(dimensao);
    t->n = 0;
    t->colisoes = 0;
    t->compartimentos = calloc((size_t)t->m, sizeof(NoHash *));
    return t;
}

void th_inserir(TabelaHash *t, const char *chave, int linha) {
    long i = endereco(t, chave);
    NoHash *p = t->compartimentos[i];
    /* Se a chave ja existe, apenas incrementa ocorrencias. */
    while (p) {
        if (strcmp(p->chave, chave) == 0) {
            p->info.ocorrencias++;
            return;
        }
        p = p->prox;
    }
    /* Chave nova: insere no inicio da lista. Se ja havia no, houve colisao. */
    if (t->compartimentos[i] != NULL) t->colisoes++;
    NoHash *novo = malloc(sizeof(NoHash));
    novo->chave = dup_str(chave);
    novo->info.ocorrencias = 1;
    novo->info.primeira_linha = linha;
    novo->prox = t->compartimentos[i];
    t->compartimentos[i] = novo;
    t->n++;
}

InfoSimbolo *th_buscar(TabelaHash *t, const char *chave) {
    long i = endereco(t, chave);
    NoHash *p = t->compartimentos[i];
    while (p) {
        if (strcmp(p->chave, chave) == 0) return &p->info;
        p = p->prox;
    }
    return NULL;
}

int th_remover(TabelaHash *t, const char *chave) {
    long i = endereco(t, chave);
    NoHash *p = t->compartimentos[i], *ant = NULL;
    while (p) {
        if (strcmp(p->chave, chave) == 0) {
            if (ant) ant->prox = p->prox;
            else t->compartimentos[i] = p->prox;
            free(p->chave);
            free(p);
            t->n--;
            return 1;
        }
        ant = p;
        p = p->prox;
    }
    return 0;
}

void th_metricas(TabelaHash *t, Metricas *out) {
    long maior = 0;
    for (long i = 0; i < t->m; i++) {
        long c = 0;
        for (NoHash *p = t->compartimentos[i]; p; p = p->prox) c++;
        if (c > maior) maior = c;
    }
    out->rotacoes = 0;
    out->colisoes = t->colisoes;
    out->altura = 0;
    out->comprimento_max = maior;
    out->fator_carga = (double)t->n / (double)t->m;
    /* Memoria: vetor de m ponteiros + n nos (chave ~ 12 bytes + struct). */
    out->memoria_bytes = t->m * (long)sizeof(NoHash *)
                       + t->n * (long)(sizeof(NoHash) + 12);
}

void th_destruir(TabelaHash *t) {
    for (long i = 0; i < t->m; i++) {
        NoHash *p = t->compartimentos[i];
        while (p) { NoHash *q = p->prox; free(p->chave); free(p); p = q; }
    }
    free(t->compartimentos);
    free(t);
}