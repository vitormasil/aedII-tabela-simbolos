**Universidade Federal do ABC (UFABC)**  
**Disciplina:** Algoritmos e Estruturas de Dados II (AED-II) - 2026.2  
**Grupo 12:**
- Aryanne Gramacho Acosta
- Gustavo Cesar Gomes
- Lucas Kato de Godoi
- Luis Felipe Ruiz Perez
- Thiago Sandoli Galvão
- Victor Braga
- Vinicius Miranda da Silva
- Vitor Malavasi Silva

---

## Sobre o Projeto

Este projeto consiste na implementação e análise comparativa de duas estruturas de dados fundamentais — **Tabela Hash** (com encadeamento exterior) e **Árvore AVL** — atuando como uma Tabela de Símbolos. 

O estudo de caso utiliza dados reais e massivos do catálogo de filmes do **IMDb (Internet Movie Database)** para avaliar o desempenho empírico das estruturas nas operações clássicas de Tabela de Símbolos:
1. `Inserir(chave, valor)`
2. `Buscar(chave)`
3. `Remover(chave)`

As métricas analisadas incluem tempo de execução de operações com e sem sucesso, consumo de memória estimado, garantia estrutural (alturas e rotações) e fator de carga.

## Estrutura do Repositório

- `tabela_simbolos.h`: Interface do TAD Tabela de Símbolos.
- `tabela_hash.c` / `.h`: Implementação da Tabela Hash com tratamento de colisões por encadeamento exterior (função `djb2`).
- `arvore_avl.c` / `.h`: Implementação da Árvore Binária de Busca Balanceada (AVL).
- `benchmark_imdb.c`: Arquivo principal (Main) que realiza o fluxo de leitura, execução das operações e medição de tempo usando `clock()`.
- `gerar_graficos.py`: Script em Python para geração de visualizações gráficas com base nos resultados obtidos.
- `resultados.csv`: Tabela consolidada com os dados brutos gerados pela execução do benchmark.

## Como Compilar e Executar

### 1. Download da Base de Dados
Devido ao limite de tamanho do GitHub (100 MB), a base de dados original não está versionada neste repositório. Para que o código funcione corretamente, você deve baixá-la manualmente:
1. Acesse o repositório oficial do IMDb: [IMDb Non-Commercial Datasets](https://datasets.imdbws.com/)
2. Baixe o arquivo descompactado chamado `title.basics.tsv` (aprox. 1 GB).
3. Coloque o arquivo `title.basics.tsv` na **raiz** do repositório clonado (mesma pasta do código-fonte).

### 2. Compilação
O projeto foi desenvolvido em **C**. Recomenda-se o uso do compilador `gcc` com a flag de otimização `-O2`. No terminal de sua preferência, execute:

```bash
gcc benchmark_imdb.c arvore_avl.c tabela_hash.c -o benchmark_imdb -O2
