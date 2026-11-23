#ifndef LOGICA_H
#define LOGICA_H

#include "avl.h"
#include <stdbool.h>

typedef struct Nohash {
    Veiculo veiculo;
    struct Nohash *next;
} Nohash;

typedef struct Hash {
    Nohash **tabela;
    int tamanho;
    int qtd;
} Hash;

typedef struct fila {
    int id;
    struct fila *prox;
} Fila;

typedef struct {
    AVL partes;
    AVL vagas;
    Hash *hash;
    Fila *Ini_fila;
    Fila *Fim_fila;
    int tam_via;
    int tam_ocupado;
} Estacionamento;

typedef enum {
    res_sucesso_estacionado,
    res_sucesso_enfileirado,
    res_erro_id_duplicado,
    res_erro_taminv,
    res_erro_mem
} Status;

typedef bool (*Estacionado)(const Veiculo *v, void *c);

int conversorCM(double m);
double conversorM(int cm);

bool est_ini(Estacionamento *est, int tam_via_cm, int tam_hash_primo);
Status est_entrar(Estacionamento *est, int id, int tam_cm);
bool est_busca(Estacionamento *est, int id, Veiculo *v);
bool est_saida(Estacionamento *est, int id);
bool est_ver_fila(Estacionamento *est, int *id, int *tam);
bool estacionar_fila(Estacionamento *est);
void est_exibir(Estacionamento *est);
void est_del(Estacionamento *est);

#endif