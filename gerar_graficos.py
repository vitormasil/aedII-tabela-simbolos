import csv
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# Le os resultados reais do benchmark
dados = {"AVL": {}, "HASH": {}}
with open("resultados.csv", encoding="utf-8-sig") as f:
    r = csv.DictReader(f)
    for row in r:
        e = row["estrutura"]
        n = int(row["n"])
        dados[e][n] = {k: row[k] for k in row}

ns = sorted(dados["AVL"].keys())

def col(estrutura, campo):
    return [float(dados[estrutura][n][campo]) for n in ns]

# Para graficos em escala log, tempos medidos como 0 ms (abaixo da
# resolucao do relogio em n pequeno) sao elevados a um piso de 0,5 ms
# apenas para efeito de plotagem, evitando a "queda vertical" que o log
# de zero provoca. Os valores reais permanecem no CSV e nas tabelas.
PISO_MS = 0.5
def col_log(estrutura, campo):
    return [max(float(dados[estrutura][n][campo]), PISO_MS) for n in ns]

AZUL = "#185FA5"   # AVL
CORAL = "#D85A30"  # HASH

def base(ax):
    ax.grid(True, which="both", ls="--", lw=0.5, alpha=0.4)
    ax.set_xscale("log")
    ax.set_xlabel("Numero de titulos de filme (n)")

# ---- Figura 1: tempo de insercao ----
fig, ax = plt.subplots(figsize=(6.2, 3.8))
base(ax)
ax.plot(ns, col_log("AVL", "t_insercao_ms"), "o-", color=AZUL, label="AVL")
ax.plot(ns, col_log("HASH", "t_insercao_ms"), "s-", color=CORAL, label="Hash")
ax.set_ylabel("Tempo de insercao (ms)")
ax.set_yscale("log")
ax.legend()
fig.tight_layout()
fig.savefig("fig_insercao.png", dpi=150)
plt.close(fig)

# ---- Figura 2: busca com sucesso e sem sucesso ----
fig, ax = plt.subplots(figsize=(6.2, 3.8))
base(ax)
ax.plot(ns, col_log("AVL", "t_busca_ok_ms"), "o-", color=AZUL, label="AVL (busca com sucesso)")
ax.plot(ns, col_log("HASH", "t_busca_ok_ms"), "s-", color=CORAL, label="Hash (busca com sucesso)")
ax.plot(ns, col_log("AVL", "t_busca_nok_ms"), "o--", color=AZUL, alpha=0.6, label="AVL (sem sucesso)")
ax.plot(ns, col_log("HASH", "t_busca_nok_ms"), "s--", color=CORAL, alpha=0.6, label="Hash (sem sucesso)")
ax.set_ylabel("Tempo de busca (ms)")
ax.set_yscale("log")
ax.legend(fontsize=8)
fig.tight_layout()
fig.savefig("fig_busca.png", dpi=150)
plt.close(fig)

# ---- Figura 3: tempo de remocao ----
fig, ax = plt.subplots(figsize=(6.2, 3.8))
base(ax)
ax.plot(ns, col_log("AVL", "t_remocao_ms"), "o-", color=AZUL, label="AVL")
ax.plot(ns, col_log("HASH", "t_remocao_ms"), "s-", color=CORAL, label="Hash")
ax.set_ylabel("Tempo de remocao (ms)")
ax.set_yscale("log")
ax.legend()
fig.tight_layout()
fig.savefig("fig_remocao.png", dpi=150)
plt.close(fig)

# ---- Figura 4: altura da AVL vs log2(n) ----
import math
fig, ax = plt.subplots(figsize=(6.2, 3.8))
base(ax)
ax.plot(ns, col("AVL", "altura"), "o-", color=AZUL, label="Altura medida da AVL")
ax.plot(ns, [math.log2(n) for n in ns], "--", color="#5F5E5A", label="log2(n) (referencia)")
ax.plot(ns, [1.4405*math.log2(n+2)-0.33 for n in ns], ":", color="#3B6D11",
        label="1,44 log2(n) (limite AVL)")
ax.set_ylabel("Altura da arvore")
ax.legend(fontsize=8)
fig.tight_layout()
fig.savefig("fig_altura.png", dpi=150)
plt.close(fig)

# ---- Figura 5: memoria ----
fig, ax = plt.subplots(figsize=(6.2, 3.8))
base(ax)
ax.plot(ns, [v/1024 for v in col("AVL", "memoria_kb")], "o-", color=AZUL, label="AVL")
ax.plot(ns, [v/1024 for v in col("HASH", "memoria_kb")], "s-", color=CORAL, label="Hash")
ax.set_ylabel("Memoria estimada (MB)")
ax.set_yscale("log")
ax.legend()
fig.tight_layout()
fig.savefig("fig_memoria.png", dpi=150)
plt.close(fig)

print("Graficos gerados:")
for fn in ["fig_insercao.png","fig_busca.png","fig_remocao.png","fig_altura.png","fig_memoria.png"]:
    print(" -", fn)

# Fatores medios de aceleracao (hash em relacao a AVL)
def razao(campo):
    pares = [(float(dados["AVL"][n][campo]), float(dados["HASH"][n][campo])) for n in ns]
    validos = [(a, h) for (a, h) in pares if h > 0 and a > 0]
    if not validos:
        return float("nan")
    return sum(a/h for (a, h) in validos)/len(validos)
print("\nAceleracao media da hash sobre a AVL:")
for campo, nome in [("t_insercao_ms","insercao"),("t_busca_ok_ms","busca c/ sucesso"),
                    ("t_busca_nok_ms","busca s/ sucesso"),("t_remocao_ms","remocao")]:
    print(f"  {nome}: {razao(campo):.1f}x")