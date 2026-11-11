#ifndef LOGICA_H
#define LOGICA_H

#include "avl.h"
#include <stdbool.h>

typedef struct Nohash{
    Veiculo veiculo;
    struct Nohash *next;
} Nohash;

typedef struct Hash{
    Nohash **tabela;
    int tamanho;
} Hash;

typedef struct fila{
    Veiculo veiculo;
    struct fila *prox;
} Fila;

typedef struct{
    AVL partes, vagas;
    Hash *hash;
    Fila *Ini_fila;
    Fila *Fim_fila;
} Estacionamento;

typedef enum{
    res_sucesso_estacionado,
    res_sucesso_enfileirado,
    res_erro_id_duplicado,
    res_erro_taminv,
    res_erro_mem
} Status;

typedef bool (*Estacionado)(const Veiculo *v, void *c);

bool est_ini(Estacionamento *est, int tam);
Status est_entrar(Estacionamento *est, int id, int tam);
Status est_ver(Estacionamento *est, int id, int tam);
bool est_busca(Estacionamento *est, int id, Veiculo *v);
bool est_saida(Estacionamento *est, int id);
void est_fila(Estacionamento *est, Estacionado f, void *c);
void est_exibir(Estacionamento *est);
void est_del(Estacionamento *est);

#endif