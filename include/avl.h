#ifndef AVL_H
#define AVL_H


typedef struct{
    int id, tam, pos;
    int status; // 0 = estacionado, 1 = na fila, 2 = saiu, 3 = livre
} Veiculo;

typedef struct No{
    Veiculo veiculo;
    int altura;
    struct No *esq;
    struct No *dir;
} No;

typedef struct AVL{
    struct No *raiz;
    int criterio; // 0 = pos, 1 = tam
} AVL;

typedef void (*resAVL)(const Veiculo *veiculo, void *c);

void avl_ini(struct AVL *avl, int c);
void avl_ins(struct AVL *avl, Veiculo v);
void avl_rem(struct AVL *avl, Veiculo v);
No *avl_busca(struct AVL *avl, Veiculo v);
No *avl_b_menor(struct AVL *avl, int tam);
No *avl_ant(struct AVL *avl, int pos);
No *avl_suc(struct AVL *avl, int pos);
void avl_exibir_mp(struct AVL *avl);
void avl_exibir_vg(struct AVL *avl);
void avl_del(struct AVL *avl);
void avl_percorrer(struct AVL *avl, resAVL f, void *c);

#endif