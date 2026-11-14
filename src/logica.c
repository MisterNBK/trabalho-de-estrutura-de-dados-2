#include "logica.h"
#include <stdio.h>
#include <stdlib.h>

int conversorCM(double m){
    return (int)(m * 100);
}

double conversorM(int cm){
    return (double)cm / 100.0;
}

int indice_hash(Hash *hash, int id){
    if (hash == NULL || hash->tamanho <= 0) {
        return 0;
    }
    return id % hash->tamanho;
}

Nohash *buscar(Estacionamento *est, int id){
    int ID = indice_hash(est->hash, id);
    Nohash *atual = est->hash->tabela[ID];

    while (atual != NULL && atual->veiculo.id != id) {
        atual = atual->next;
    }
    return atual;
}

bool inserirH(Estacionamento *est, Veiculo veiculo){
    int ID = indice_hash(est->hash, veiculo.id);
    Nohash *novo = (Nohash *) calloc(1, sizeof(Nohash));

    if (novo == NULL) { 
        return;
        
    }
    novo->veiculo = veiculo;
    novo->next = est->hash->tabela[ID];
    est->hash->tabela[ID] = novo;
}

void removerH(Estacionamento *est, int id){
    int ID = indice_hash(est->hash, id);
    Nohash **atual = &est->hash->tabela[ID];

    while (*atual != NULL && (*atual)->veiculo.id != id) {
        atual = &(*atual)->next;
    }
    if (*atual == NULL) { return; }

    else{
        Nohash *removido = *atual;
        *atual = removido->next;
        free(removido);
        est->hash->qtd--;
    }
}

void putVaga(Estacionamento *est, Veiculo *veiculo, Veiculo vaga){
    avl_rem(&est->vagas, vaga);
    avl_rem(&est->partes, vaga);
    veiculo->pos = vaga.pos;
    veiculo->status = 0;
    avl_ins(&est->partes, *veiculo);
    est->tam_ocupado += veiculo->tam;

    if(vaga.tam > veiculo->tam){
        Veiculo nova_vaga = {0, vaga.tam - veiculo->tam, vaga.pos + veiculo->tam, 3};
        avl_ins(&est->vagas, nova_vaga);
        avl_ins(&est->partes, nova_vaga);
    }
}

bool est_ini(Estacionamento *est, int tam, int tam_hash){
   if(tam <= 0 || tam_hash <= 0){
        return false;
    }
    est->hash = malloc(sizeof(Hash));
    if(est->hash == NULL){ return false; }

    if(tam_hash <= 0){
        free(est->hash);
        return false;
    }

    est->hash->tabela =(Nohash **) calloc(tam_hash, sizeof(Nohash *));
    if(est->hash->tabela == NULL){
        free(est->hash);
        return false;
    }
    
    est->hash->tamanho = tam_hash;
    est->hash->qtd = 0;
    est->tam_via = tam;
    est->tam_ocupado = 0;
    avl_ini(&est->partes, 0);
    avl_ini(&est->vagas, 1);
    Veiculo v = {0, tam, 0, 3};
    avl_ins(&est->partes, v);
    avl_ins(&est->vagas, v);
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

    Veiculo veiculo = {id, tam, -1, 1};
    No *vaga = avl_b_menor(&est->vagas, tam);

    if(vaga == NULL){
        Fila *fila = malloc(sizeof(Fila));
        if(fila == NULL){
            return res_erro_mem;
        }
        if(!inserirH(est, veiculo)){
            free(fila);
            return res_erro_mem;
        }

        fila->veiculo.id = id;
        fila->prox = NULL;
        if(est->Fim_fila == NULL){
            est->Ini_fila = fila;
            est->Fim_fila = fila;
        }else{
            est->Fim_fila->prox = fila;
            est->Fim_fila = fila;
        }
        return res_sucesso_enfileirado;
    } else{
        putVaga(est, &veiculo, vaga->veiculo);
            if(!inserirH(est, veiculo)){
                return res_erro_mem;
            }
            return res_sucesso_estacionado;
        }
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
    est->tam_ocupado -= ocupado.tam;

    No *suc = avl_suc(&est->partes, ocupado.pos);
    No *ant = avl_ant(&est->partes, ocupado.pos);

    int newP = ocupado.pos, newT = ocupado.tam;

    Veiculo v_ant, v_suc;

    bool cp_ant = (ant != NULL && ant->veiculo.id == 0);
    if(!cp_ant){
        v_ant = ocupado;
    }else{
        v_ant = ant->veiculo;
    }
    bool cp_suc = (suc != NULL && suc->veiculo.id == 0);
    if(!cp_suc){
        v_suc = ocupado;
    }else{
        v_suc = suc->veiculo;
    }

    avl_rem(&est->partes, ocupado);
    removerH(est, id);

    if(cp_ant){
        avl_rem(&est->partes, v_ant);
        avl_rem(&est->vagas, v_ant);
        newT += v_ant.tam;
        newP = v_ant.pos;
    }

    if(cp_suc){
        avl_rem(&est->partes, v_suc);
        avl_rem(&est->vagas, v_suc);
        newT += v_suc.tam;
    }

    Veiculo vaga_msc = {0, newT, newP, 3};
    avl_ins(&est->partes, vaga_msc);
    avl_ins(&est->vagas, vaga_msc);
    return true;
}

void est_fila(Estacionamento *est, int *id, int *tam){
    Fila *atual = est->Ini_fila;
    while(atual != NULL){
        Nohash *reg = buscar(est, est->Ini_fila->veiculo.id);
        if(reg == NULL){
            return false;
        }
    No *vaga = avl_b_menor(&est->vagas, reg->veiculo.tam);
    if(vaga != NULL){
        return false;
    }
    No *vaga = avl_b_menor()
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