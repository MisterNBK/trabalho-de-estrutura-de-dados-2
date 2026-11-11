#include "avl.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int fator;

int altura(No *no){
    if(no == NULL){ 
      return -1;
    }
    else{
      return no->altura;
    }
}

int fator_balanceamento(No *no){
    if(no == NULL){ return 0;}
      
    return altura(no->esq) - altura(no->dir);   
}

int comparar(AVL *avl, Veiculo v1, Veiculo v2){
    if(avl->criterio == 0){
        if(v1.pos < v2.pos){ return -1; }
        else if(v1.pos > v2.pos) { return 1; }
        else { return 0; }
    }
    else{
        if(v1.tam < v2.tam){ return -1; }
        else if(v1.tam > v2.tam) { return 1; }
        else { return 0; }

    }
}

No *rotacao_dir(No *no){
    No *aux = no->esq;
    no->esq = aux->dir;
    aux->dir = no;
    no->altura = 1 + (altura(no->esq) > altura(no->dir) ? altura(no->esq) : altura(no->dir));
    aux->altura = 1 + (altura(aux->esq) > altura(aux->dir) ? altura(aux->esq) : altura(aux->dir));
    return aux;
}

No *rotacao_esq(No *no){
    No *aux = no->dir;
    no->dir = aux->esq;
    aux->esq = no;
    no->altura = 1 + (altura(no->esq) > altura(no->dir) ? altura(no->esq) : altura(no->dir));
    aux->altura = 1 + (altura(aux->esq) > altura(aux->dir) ? altura(aux->esq) : altura(aux->dir));
    return aux;
}

No *rebalanco(No *no){
 if(no == NULL){ return NULL; }

int maior_alt;

if (altura(no->esq) > altura(no->dir)) {
    maior_alt = altura(no->esq);
} else {
    maior_alt = altura(no->dir);
}

no->altura = 1 + maior_alt;
fator = fator_balanceamento(no);

if(fator > 1){
    if(fator_balanceamento(no->esq) < 0){
        no->esq = rotacao_esq(no->esq);
    } 
    return rotacao_dir(no);
}

if(fator< -1){
    if(fator_balanceamento(no->dir) > 0){    
    no->dir = rotacao_dir(no->dir);
    }
    return rotacao_esq(no);
}
return no;
}

No *ins(AVL *avl, No *no, Veiculo v, bool teste){
    if(no == NULL){
        no = (No *) calloc(1, sizeof(No));
        if(no == NULL){
            teste = false;
            return NULL;
        }
        no->veiculo = v;
        no->altura = 0;
        no->esq = NULL;
        no->dir = NULL;
        teste = true;
        return no;
    }
    int c = comparar(avl, v, no->veiculo);
    if(c < 0){
        no->esq = ins(avl, no->esq, v, teste);
    } else if(c > 0){
        no->dir = ins(avl, no->dir, v, teste);
    } else {
        teste = false;
        return no;
    }
    return rebalanco(no);
}

No *menor(No *no){

    if(no == NULL){ return NULL; }
    while(no->esq != NULL){
        no = no->esq;
    }
    return no;
}

No *rem(AVL *avl, No *no, Veiculo v, bool *teste){
    int c;
    if(no == NULL){ return NULL;}

    c = comparar(avl, v, no->veiculo);
    if (c < 0) {
        no->esq = rem(avl, no->esq, v, teste);
    } else if(c>0) {
        no->dir = rem(avl, no->dir, v, teste);
    }else{
        No *no1;
        if (teste != NULL) {
            *teste = true;
        }
        if(no->esq == NULL || no->dir == NULL){
            if(no->esq != NULL){
                no1 = no->esq;
                free(no);
                return no1;
            }else{
                no1 = no->dir;
                free(no);
                return no1;
            }
        }else{
        No *suc = menor(no->dir);
        no->veiculo = suc->veiculo;
        no->dir = rem(avl, no->dir, suc->veiculo, teste);
        }
    }
    return rebalanco(no);
}

void percorrer(No *no, resAVL e, void *c){
    if(no == NULL){ return; }
    percorrer(no->esq, e, c);
    e(&no->veiculo, c);
    percorrer(no->dir, e, c);
}

void exibir(No *no){
    if(no == NULL){ return; }
    exibir(no->esq);
    printf("ID: %d, TAM: %d, POS: %d, STATUS: %d\n", no->veiculo.id, no->veiculo.tam, no->veiculo.pos, no->veiculo.status);
    exibir(no->dir);
}

void del(No *no){
    if(no == NULL){ return; }
    del(no->esq);
    del(no->dir);
    free(no);
}

void avl_ini(AVL *avl, int c){
    avl->raiz = NULL;
    avl->criterio = c;
}

void avl_ins(AVL *avl, Veiculo v){
    bool teste = false;
    avl->raiz = ins(avl, avl->raiz, v, teste);
}

void avl_rem(AVL *avl, Veiculo v){
    bool teste = false;
    avl->raiz = rem(avl, avl->raiz, v, &teste);
}

No *avl_busca(AVL *avl, Veiculo v){
    No *no = avl->raiz;
    while(no != NULL){
        int c = comparar(avl, v, no->veiculo);
        if(c < 0){
            no = no->esq;
        } else if(c > 0){
            no = no->dir;
        } else {
            return no;
        }
    }
    return NULL;
}

No *avl_b_menor(AVL *avl){
    return menor(avl->raiz);
}

No *avl_ant(AVL *avl){
    No *no = avl->raiz;
    if(no == NULL || no->esq == NULL){ return NULL; }
    no = no->esq;
    while(no->dir != NULL){
        no = no->dir;
    }
    return no;
}

No *avl_suc(AVL *avl){
    No *no = avl->raiz;
    if(no == NULL || no->dir == NULL){ return NULL; }
    no = no->dir;
    while(no->esq != NULL){
        no = no->esq;
    }
    return no;
}

void avl_exibir(AVL *avl){
    exibir(avl->raiz);
}

void avl_del(AVL *avl){
    del(avl->raiz);
    avl->raiz = NULL;
}

void avl_percorrer(AVL *avl, resAVL e, void *c){
    percorrer(avl->raiz, e, c);
}
