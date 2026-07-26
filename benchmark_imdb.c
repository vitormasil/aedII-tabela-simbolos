/* Compara Tabela Hash x Arvore AVL como tabela de simbolos, usando dados
 * REAIS do catalogo de filmes do IMDb (arquivo title.basics.tsv).
 *
 * Estudo de caso: catalogo de filmes. A chave e o titulo do filme
 * (primaryTitle) e o valor e a informacao associada (numero de
 * ocorrencias do titulo e a "linha"/posicao da primeira ocorrencia).
 * Titulos repetidos (p.ex. remakes com o mesmo nome) sao contabilizados
 * como ocorrencias adicionais da mesma chave.
 *
 * Uso:
 *   ./benchmark_imdb <arquivo_de_titulos> [n1 n2 n3 ...]
 * Le uma chave por linha. Se tamanhos forem informados, roda para cada um
 * (usando os n primeiros titulos apos embaralhamento); caso contrario usa
 * um conjunto padrao de tamanhos.
 *
 * Saida: CSV no stdout.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tabela_hash.h"
#include "arvore_avl.h"

#define MAX_LINHA 2048

/* Tempo em milissegundos. Usa clock() da biblioteca padrao <time.h>,
 * portavel entre Windows (MinGW), Linux e macOS. Mede tempo de CPU, o
 * que e adequado para comparar o custo das operacoes das estruturas. */
static double agora_ms(void) {
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
}

/* Le ate 'limite' titulos (uma chave por linha) do arquivo. Devolve o
 * numero lido em *out_n. */
static char **ler_titulos(const char *caminho, long limite, long *out_n) {
    FILE *f = fopen(caminho, "r");
    if (!f) { perror("fopen"); exit(1); }
    char **v = malloc((size_t)limite * sizeof(char *));
    char buf[MAX_LINHA];
    long i = 0;
    while (i < limite && fgets(buf, sizeof(buf), f)) {
        size_t len = strlen(buf);
        if (len && buf[len - 1] == '\n') buf[--len] = '\0';
        if (len == 0) continue;
        v[i++] = dup_str(buf);
    }
    fclose(f);
    *out_n = i;
    return v;
}

/* Embaralha in-place (Fisher-Yates, semente fixa) para ordem aleatoria. */
static void embaralhar(char **v, long n, unsigned seed) {
    srand(seed);
    for (long i = n - 1; i > 0; i--) {
        long j = (long)((double)rand() / ((double)RAND_MAX + 1) * (i + 1));
        char *t = v[i]; v[i] = v[j]; v[j] = t;
    }
}

/* Gera n chaves garantidamente ausentes (prefixo improvavel). */
static char **gerar_ausentes(long n) {
    char **v = malloc((size_t)n * sizeof(char *));
    for (long i = 0; i < n; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "@@ausente#%ld@@", i);
        v[i] = dup_str(buf);
    }
    return v;
}

static void liberar(char **v, long n) {
    for (long i = 0; i < n; i++) free(v[i]);
    free(v);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "uso: %s <arquivo_titulos> [n1 n2 ...]\n", argv[0]);
        return 1;
    }
    const char *arquivo = argv[1];

    long tamanhos_padrao[] = {1000, 5000, 10000, 50000, 100000, 500000, 645000};
    int qt;
    long *tamanhos;
    if (argc > 2) {
        qt = argc - 2;
        tamanhos = malloc(qt * sizeof(long));
        for (int i = 0; i < qt; i++) tamanhos[i] = atol(argv[i + 2]);
    } else {
        qt = (int)(sizeof(tamanhos_padrao) / sizeof(tamanhos_padrao[0]));
        tamanhos = tamanhos_padrao;
    }

    /* Descobre o maior tamanho pedido e carrega esse tanto de titulos. */
    long maxn = 0;
    for (int i = 0; i < qt; i++) if (tamanhos[i] > maxn) maxn = tamanhos[i];

    long total;
    char **todos = ler_titulos(arquivo, maxn, &total);
    fprintf(stderr, "Titulos carregados: %ld\n", total);
    embaralhar(todos, total, 12345u);

    printf("estrutura,n,t_insercao_ms,t_busca_ok_ms,t_busca_nok_ms,"
           "t_remocao_ms,rotacoes,colisoes,altura,comp_max,fator_carga,"
           "memoria_kb\n");

    for (int k = 0; k < qt; k++) {
        long n = tamanhos[k];
        if (n > total) n = total;
        char **chaves = todos;              /* usa os n primeiros */
        char **ausentes = gerar_ausentes(n);
        double t0, t1;
        Metricas mt;

        /* ---------------- ARVORE AVL ---------------- */
        ArvoreAVL *a = avl_criar();
        t0 = agora_ms();
        for (long i = 0; i < n; i++) avl_inserir(a, chaves[i], (int)i);
        t1 = agora_ms();
        double t_ins_avl = t1 - t0;

        t0 = agora_ms();
        for (long i = 0; i < n; i++) (void)avl_buscar(a, chaves[i]);
        t1 = agora_ms();
        double t_bok_avl = t1 - t0;

        t0 = agora_ms();
        for (long i = 0; i < n; i++) (void)avl_buscar(a, ausentes[i]);
        t1 = agora_ms();
        double t_bnok_avl = t1 - t0;

        avl_metricas(a, &mt);
        long rot = mt.rotacoes; int alt = mt.altura; long mem_avl = mt.memoria_bytes;

        t0 = agora_ms();
        for (long i = 0; i < n; i++) (void)avl_remover(a, chaves[i]);
        t1 = agora_ms();
        double t_rem_avl = t1 - t0;
        avl_destruir(a);

        printf("AVL,%ld,%.3f,%.3f,%.3f,%.3f,%ld,%d,%d,%ld,%.4f,%.1f\n",
               n, t_ins_avl, t_bok_avl, t_bnok_avl, t_rem_avl,
               rot, 0, alt, 0L, 0.0, mem_avl / 1024.0);

        /* ---------------- TABELA HASH ---------------- */
        TabelaHash *t = th_criar(n);
        t0 = agora_ms();
        for (long i = 0; i < n; i++) th_inserir(t, chaves[i], (int)i);
        t1 = agora_ms();
        double t_ins_h = t1 - t0;

        t0 = agora_ms();
        for (long i = 0; i < n; i++) (void)th_buscar(t, chaves[i]);
        t1 = agora_ms();
        double t_bok_h = t1 - t0;

        t0 = agora_ms();
        for (long i = 0; i < n; i++) (void)th_buscar(t, ausentes[i]);
        t1 = agora_ms();
        double t_bnok_h = t1 - t0;

        th_metricas(t, &mt);
        long col = mt.colisoes; long cmax = mt.comprimento_max;
        double alfa = mt.fator_carga; long mem_h = mt.memoria_bytes;

        t0 = agora_ms();
        for (long i = 0; i < n; i++) (void)th_remover(t, chaves[i]);
        t1 = agora_ms();
        double t_rem_h = t1 - t0;
        th_destruir(t);

        printf("HASH,%ld,%.3f,%.3f,%.3f,%.3f,%ld,%ld,%d,%ld,%.4f,%.1f\n",
               n, t_ins_h, t_bok_h, t_bnok_h, t_rem_h,
               0L, col, 0, cmax, alfa, mem_h / 1024.0);

        liberar(ausentes, n);
        fflush(stdout);
    }

    liberar(todos, total);
    if (argc > 2) free(tamanhos);
    return 0;
}