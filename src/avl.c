#include "avl.h"

#include <stdlib.h>
#include <stdio.h>

int fator;

int alt(No *no){
    if(no == NULL){ 
      return -1;
    }
    else{
      return no->altura;
    }
}
int fat_bal(No *no){
    if(no == NULL){ return 0;}
      
    return alt(no->esq) - alt(no->dir);   
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
    no->altura = 1 + (alt(no->esq) > alt(no->dir) ? alt(no->esq) : alt(no->dir));
    aux->altura = 1 + (alt(aux->esq) > alt(aux->dir) ? alt(aux->esq) : alt(aux->dir));
    return aux;
}

No *rotacao_esq(No *no){
    No *aux = no->dir;
    no->dir = aux->esq;
    aux->esq = no;
    no->altura = 1 + (alt(no->esq) > alt(no->dir) ? alt(no->esq) : alt(no->dir));
    aux->altura = 1 + (alt(aux->esq) > alt(aux->dir) ? alt(aux->esq) : alt(aux->dir));
    return aux;
}

No *balanco(No *no){
 if(no == NULL){ return NULL; }

int f, maior_alt;

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