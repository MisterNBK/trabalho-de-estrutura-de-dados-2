#include "logica.h"
#include <stdio.h>
#include <stdlib.h>

int indice_hash(Hash *hash, int id)
{
    if (hash == NULL || hash->tamanho <= 0) {
        return 0;
    }
    return id % hash->tamanho;
}

Nohash *buscar(Estacionamento *estacionamento, int id)
{
    int ID = indice_hash(estacionamento->hash, id);
    Nohash *atual = estacionamento->hash->tabela[ID];

    while (atual != NULL && atual->veiculo.id != id) {
        atual = atual->next;
    }
    return atual;
}

void inserirH(Estacionamento *estacionamento, Veiculo veiculo)
{
    int ID = indice_hash(estacionamento->hash, veiculo.id);
    Nohash *novo = (Nohash *) calloc(1, sizeof(Nohash));

    if (novo == NULL) { return;}
    novo->veiculo = veiculo;
    novo->next = estacionamento->hash->tabela[ID];
    estacionamento->hash->tabela[ID] = novo;
}

void removerH(Estacionamento *estacionamento, int id)
{
    int ID = indice_hash(estacionamento->hash, id);
    Nohash **atual = &estacionamento->hash->tabela[ID];

    while (*atual != NULL && (*atual)->veiculo.id != id) {
        atual = &(*atual)->next;
    }
    if (*atual == NULL) { return; }

    else{
        Nohash *removido = *atual;
        *atual = removido->next;
        free(removido);
    }
}

No *encVaga(Estacionamento *estacionamento, int tamanho){
    (void)tamanho;
    return avl_b_menor(&estacionamento->vagas);
}

void putVaga(Estacionamento *estacionamento, Veiculo veiculo, Veiculo tam){

    avl_rem(&estacionamento->vagas, tam);
    avl_rem(&estacionamento->partes, tam);
    avl_ins(&estacionamento->partes, veiculo);
    if(tam.tam > veiculo.tam){
        Veiculo nova_vaga = {0, tam.tam - veiculo.tam, veiculo.pos + veiculo.tam, 0};
        avl_ins(&estacionamento->vagas, nova_vaga);
        avl_ins(&estacionamento->partes, nova_vaga);
    }
}

void printEst(Estacionamento *estacionamento){
    printf("Mapa:\n");
    if (estacionamento->partes.raiz == NULL) {
        printf("  (vazia)\n");
    } else {
        avl_exibir(&estacionamento->partes);
    }
    printf("\nvagas:\n");
    if (estacionamento->vagas.raiz == NULL) {
        printf("  (vazia)\n");
    } else {
        avl_exibir(&estacionamento->vagas);
    }
}

bool est_ini(Estacionamento *est, int tam){
    est->hash = malloc(sizeof(Hash));
    if(est->hash == NULL){ return false; }
    est->hash->tabela = malloc(sizeof(Nohash *) * tam);
    if(est->hash->tabela == NULL){ return false; }
    est->hash->tamanho = tam;
    for(int i = 0; i < tam; i++){
        est->hash->tabela[i] = NULL;
    }
    avl_ini(&est->partes, 0);
    avl_ini(&est->vagas, 1);
    Veiculo vaga_inicial = {0, tam, 0, 0};
    avl_ins(&est->partes, vaga_inicial);
    avl_ins(&est->vagas, vaga_inicial);
    est->Ini_fila = NULL;
    est->Fim_fila = NULL;
    return true;
}

Status est_entrar(Estacionamento *est , int id, int tam){
    
    if(buscar(est, id) != NULL){
        return res_erro_id_duplicado;
    }
    if(tam <= 0 || tam > est->hash->tamanho){
        return res_erro_taminv;
    }
    Veiculo veiculo = {id, tam, 0, 1};
    No *vaga = encVaga(est, tam);
    if(vaga == NULL){
        Fila *novo = malloc(sizeof(Fila));
        if(novo == NULL){ return res_erro_mem; }
        novo->veiculo = veiculo;
        novo->prox = NULL;
        if(est->Fim_fila == NULL){
            est->Ini_fila = novo;
            est->Fim_fila = novo;
        } else {
            est->Fim_fila->prox = novo;
            est->Fim_fila = novo;
        }
        inserirH(est, veiculo);
        return res_sucesso_enfileirado;
    } else {
        putVaga(est, veiculo, vaga->veiculo);
        inserirH(est, veiculo);
        return res_sucesso_estacionado;
    }
}

Status est_ver(Estacionamento *est, int id, int tam){
    (void)est;
    (void)id;
    (void)tam;
    return res_sucesso_estacionado;
}

bool est_busca(Estacionamento *est, int id, Veiculo *v){
    Nohash *registro = buscar(est, id);
    if(registro == NULL){ return false; }
    if(v != NULL){
        *v = registro->veiculo;
    }
    return true;
}

bool est_saida(Estacionamento *est, int id){
    Nohash *registro = buscar(est, id);
    if(registro == NULL || registro->veiculo.status != 0){
        return false;
    }
    Veiculo ocupado = registro->veiculo;
    Veiculo nova_vaga = {0, ocupado.tam, ocupado.pos, 2};
    avl_rem(&est->partes, ocupado);
    avl_ins(&est->partes, nova_vaga);
    avl_ins(&est->vagas, nova_vaga);
    removerH(est, id);
    return true;
}

void est_fila(Estacionamento *est, Estacionado f, void *c){
    Fila *atual = est->Ini_fila;
    while(atual != NULL){
        f(&atual->veiculo, c);
        atual = atual->prox;
    }
}

void est_exibir(Estacionamento *est){
    printEst(est);
    printf("\nFila:\n");
    Fila *atual = est->Ini_fila;
    if(atual == NULL){
        printf("  (vazia)\n");
    } else {
        while(atual != NULL){
            printf("ID: %d, TAM: %d, POS: %d, STATUS: %d\n", atual->veiculo.id, atual->veiculo.tam, atual->veiculo.pos, atual->veiculo.status);
            atual = atual->prox;
        }
    }
}

void est_del(Estacionamento *est){
    avl_del(&est->partes);
    avl_del(&est->vagas);
    for(int i = 0; i < est->hash->tamanho; i++){
        Nohash *atual = est->hash->tabela[i];
        while(atual != NULL){
            Nohash *prox = atual->next;
            free(atual);
            atual = prox;
        }
    }
    free(est->hash->tabela);
    free(est->hash);
    est->Ini_fila = NULL;
    est->Fim_fila = NULL;
}