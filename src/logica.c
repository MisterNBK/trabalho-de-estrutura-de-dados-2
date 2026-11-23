#include "logica.h"
#include <stdio.h>
#include <stdlib.h>

int conversorCM(double m) {
    return (int)(m * 100.0 + 0.5);
}

double conversorM(int cm) {
    return (double)cm / 100.0;
}

 int indice_hash(const Hash *hash, int id) {
    if (hash == NULL || hash->tamanho <= 0) {
        return 0;
    }
    return id % hash->tamanho;
}

 Nohash *buscar_hash(const Estacionamento *est, int id) {
    int idx = indice_hash(est->hash, id);
    Nohash *atual = est->hash->tabela[idx];

    while (atual != NULL && atual->veiculo.id != id) {
        atual = atual->next;
    }
    return atual;
}

bool inserir_hash(Estacionamento *est, Veiculo veiculo) {
    int idx = indice_hash(est->hash, veiculo.id);
    Nohash *novo = (Nohash *)calloc(1, sizeof(Nohash));
    if (novo == NULL) {
        return false;
    }
    novo->veiculo = veiculo;
    novo->next = est->hash->tabela[idx];
    est->hash->tabela[idx] = novo;
    est->hash->qtd++;
    return true;
}
 void remover_hash(Estacionamento *est, int id) {
    int idx = indice_hash(est->hash, id);
    Nohash **atual = &est->hash->tabela[idx];

    while (*atual != NULL && (*atual)->veiculo.id != id) {
        atual = &(*atual)->next;
    }
    if (*atual != NULL) {
        Nohash *removido = *atual;
        *atual = removido->next;
        free(removido);
        est->hash->qtd--;
    }
}

void estaciona(Estacionamento *est, Veiculo *veiculo, Veiculo vaga) {
    avl_rem(&est->vagas, vaga);
    avl_rem(&est->partes, vaga);

    veiculo->pos = vaga.pos;
    veiculo->status = 0;
    avl_ins(&est->partes, *veiculo);
    est->tam_ocupado += veiculo->tam;

    if (vaga.tam > veiculo->tam) {
        Veiculo nova_vaga = {0, vaga.tam - veiculo->tam, vaga.pos + veiculo->tam, 3};
        avl_ins(&est->vagas, nova_vaga);
        avl_ins(&est->partes, nova_vaga);
    }
}

bool est_ini(Estacionamento *est, int tam_via_cm, int tam_hash_primo) {
    Veiculo vaga_inicial;
    if (est == NULL) {
        return false;
    }

    est->hash = (Hash *)malloc(sizeof(Hash));
    if (est->hash == NULL) {
        return false;
    }

    if (tam_hash_primo <= 0) {
        free(est->hash);
        est->hash = NULL;
        return false;
    }

    est->hash->tabela = (Nohash **)calloc(tam_hash_primo, sizeof(Nohash *));
    if (est->hash->tabela == NULL) {
        free(est->hash);
        est->hash = NULL;
        return false;
    }

    est->hash->tamanho = tam_hash_primo;
    est->hash->qtd = 0;
    est->tam_via = tam_via_cm;
    est->tam_ocupado = 0;

    avl_ini(&est->partes, 0);
    avl_ini(&est->vagas, 1);

    vaga_inicial.id = 0;
    vaga_inicial.tam = tam_via_cm;
    vaga_inicial.pos = 0;
    vaga_inicial.status = 3;

    avl_ins(&est->partes, vaga_inicial);
    avl_ins(&est->vagas, vaga_inicial);

    est->Ini_fila = NULL;
    est->Fim_fila = NULL;
    return true;
}

Status est_entrar(Estacionamento *est, int id, int tam_cm) {
    Veiculo veiculo;
    No *vaga;

    if (id <= 0 || buscar_hash(est, id) != NULL) {
        return res_erro_id_duplicado;
    }
    if (tam_cm <= 0 || tam_cm > est->tam_via) {
        return res_erro_taminv;
    }

    veiculo.id = id;
    veiculo.tam = tam_cm;
    veiculo.pos = -1;
    veiculo.status = 1;

    vaga = avl_b_menor(&est->vagas, tam_cm);
    if (vaga == NULL) {
        Fila *novo_fila = (Fila *)malloc(sizeof(Fila));
        if (novo_fila == NULL) {
            return res_erro_mem;
        }
        if (!inserir_hash(est, veiculo)) {
            free(novo_fila);
            return res_erro_mem;
        }
        novo_fila->id = id;
        novo_fila->prox = NULL;
        if (est->Fim_fila == NULL) {
            est->Ini_fila = novo_fila;
            est->Fim_fila = novo_fila;
        } else {
            est->Fim_fila->prox = novo_fila;
            est->Fim_fila = novo_fila;
        }
        return res_sucesso_enfileirado;
    } else {
        estaciona(est, &veiculo, vaga->veiculo);
        if (!inserir_hash(est, veiculo)) {
            return res_erro_mem;
        }
        return res_sucesso_estacionado;
    }
}

bool est_busca(Estacionamento *est, int id, Veiculo *v) {
    Nohash *registro = buscar_hash(est, id);
    if (registro == NULL) {
        return false;
    }
    if (v != NULL) {
        *v = registro->veiculo;
    }
    return true;
}

bool est_saida(Estacionamento *est, int id) {
    Nohash *registro = buscar_hash(est, id);
    Veiculo ocupado;

    if (registro == NULL || registro->veiculo.status != 0) {
        return false;
    }

    ocupado = registro->veiculo;
    est->tam_ocupado -= ocupado.tam;

    No *ant = avl_ant(&est->partes, ocupado.pos);
    No *suc = avl_suc(&est->partes, ocupado.pos);
    int newP = ocupado.pos;
    int newT = ocupado.tam;

    bool cp_ant; bool cp_suc;
    Veiculo v_ant; Veiculo v_suc;

    cp_ant = (ant != NULL && (ant->veiculo.status == 3 || ant->veiculo.id == 0));
    v_ant = cp_ant ? ant->veiculo : ocupado;

    cp_suc = (suc != NULL && (suc->veiculo.status == 3 || suc->veiculo.id == 0));
    v_suc = cp_suc ? suc->veiculo : ocupado;

    avl_rem(&est->partes, ocupado);
    remover_hash(est, id);

    if (cp_ant) {
        avl_rem(&est->partes, v_ant);
        avl_rem(&est->vagas, v_ant);
        newP = v_ant.pos;
        newT += v_ant.tam;
    }

    if (cp_suc) {
        avl_rem(&est->partes, v_suc);
        avl_rem(&est->vagas, v_suc);
        newT += v_suc.tam;
    }

    Veiculo vaga_msc;
    vaga_msc.id = 0;
    vaga_msc.tam = newT;
    vaga_msc.pos = newP;
    vaga_msc.status = 3;

    avl_ins(&est->partes, vaga_msc);
    avl_ins(&est->vagas, vaga_msc);
    return true;
}

bool est_ver_fila(Estacionamento *est, int *id, int *tam) {
    Nohash *reg;
    No *vaga;

    if (est == NULL || est->Ini_fila == NULL) {
        return false;
    }
    reg = buscar_hash(est, est->Ini_fila->id);
    if (reg == NULL) {
        return false;
    }
    vaga = avl_b_menor(&est->vagas, reg->veiculo.tam);
    if (vaga == NULL) {
        return false;
    }
    if (id != NULL) *id = reg->veiculo.id;
    if (tam != NULL) *tam = reg->veiculo.tam;
    return true;
}

bool estacionar_fila(Estacionamento *est) {
    Fila *primeiro;
    Nohash *reg;
    No *vaga;

    if (est == NULL || est->Ini_fila == NULL) {
        return false;
    }
    primeiro = est->Ini_fila;
    reg = buscar_hash(est, primeiro->id);
    if (reg == NULL) {
        return false;
    }
    vaga = avl_b_menor(&est->vagas, reg->veiculo.tam);
    if (vaga == NULL) {
        return false;
    }

    est->Ini_fila = primeiro->prox;
    if (est->Ini_fila == NULL) {
        est->Fim_fila = NULL;
    }
    free(primeiro);

  
    estaciona(est, &reg->veiculo, vaga->veiculo);
    return true;
}

void est_exibir(Estacionamento *est) {
    double pct = 0;
    double carga = 0;
    if (est->tam_via > 0) {
        pct = ((double)est->tam_ocupado / (double)est->tam_via) * 100.0;
    }
    if(est->hash != NULL && est->hash->tamanho > 0){
        carga = ((double)est->hash->qtd / (double)est->hash->tamanho);
    }
        
    int i;
    Fila *f;

    printf("\n");
    printf("Via: %.2f m | Ocupacao: %.2f%%\n", conversorM(est->tam_via), pct);
    avl_exibir_mp(&est->partes);

    printf("\nvagas disponiveis:\n");
    avl_exibir_vg(&est->vagas);

    printf("\ntabela hash (m=%d, itens=%d, fator de carga=%.2f):\n", est->hash->tamanho, est->hash->qtd, carga);
    for (i = 0; i < est->hash->tamanho; i++) {
        Nohash *atual = est->hash->tabela[i];
        printf("  [%d]: ", i);
        if (atual == NULL) { printf("NULL"); }
         else {
            while (atual != NULL) {
                char st = (atual->veiculo.status == 0) ? 'E' : 'A';
                if (atual->veiculo.status == 0) {
                    printf("[id:%d tam:%.2f ini:%.2f %c] ->0 ", atual->veiculo.id, conversorM(atual->veiculo.tam), conversorM(atual->veiculo.pos), st);
                } else {
                    printf("[id:%d tam:%.2f ini:-- %c] -> ", atual->veiculo.id, conversorM(atual->veiculo.tam), st);
                }
                atual = atual->next;
            }
            printf("NULL");
        }
        printf("\n");
    }

    printf("\nfila de espera:\n");
    f = est->Ini_fila;
    if (f == NULL) {
        printf("vazia\n");
    } else {
        while (f != NULL) {
            Nohash *reg = buscar_hash(est, f->id);
            if (reg != NULL) {
                printf("  ID %d (%.2f m)\n", reg->veiculo.id, conversorM(reg->veiculo.tam));
            }
            f = f->prox;
        }
    }
    printf("\n\n");
}

void est_del(Estacionamento *est) {
    int i;
    Fila *f;

    avl_del(&est->partes);
    avl_del(&est->vagas);

    if (est->hash != NULL) {
        for (i = 0; i < est->hash->tamanho; i++) {
            Nohash *atual = est->hash->tabela[i];
            while (atual != NULL) {
                Nohash *prox = atual->next;
                free(atual);
                atual = prox;
            }
        }
        free(est->hash->tabela);
        free(est->hash);
        est->hash = NULL;
    }

    f = est->Ini_fila;
    while (f != NULL) {
        Fila *prox = f->prox;
        free(f);
        f = prox;
    }
    est->Ini_fila = NULL;
    est->Fim_fila = NULL;
}