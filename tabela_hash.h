#ifndef TABELA_HASH_H
#define TABELA_HASH_H
 
#include "tabela_simbolos.h"
 
typedef struct NoHash {
    char *chave;
    InfoSimbolo info;
    struct NoHash *prox;   /* encadeamento exterior */
} NoHash;
 
typedef struct {
    NoHash **compartimentos; /* vetor de m listas encadeadas */
    long m;                  /* dimensao da tabela (numero primo) */
    long n;                  /* numero de chaves armazenadas */
    long colisoes;           /* contador de colisoes */
} TabelaHash;
 
TabelaHash *th_criar(long dimensao);
void        th_inserir(TabelaHash *t, const char *chave, int linha);
InfoSimbolo *th_buscar(TabelaHash *t, const char *chave);
int         th_remover(TabelaHash *t, const char *chave);
void        th_metricas(TabelaHash *t, Metricas *out);
void        th_destruir(TabelaHash *t);
long        proximo_primo(long x);
 
#endif