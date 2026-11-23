
#include "logica.h"
#include <stdio.h>

int main() {    
    Estacionamento est;
    double via_m = 0.0;
    int tam_hash = 0, tam_fila = 0, id_fila = 0, opcao = 0;

    printf("unforme o comprimento da via em metros: ");
    scanf("%lf", &via_m);
    if(via_m < 0){
        printf("comprimento invalido\n");
        return 1;
    }

    printf("informe o tamanho da tabela hash: ");
    scanf("%d", &tam_hash);
    if (tam_hash <= 0) {
        while(scanf("%d", &tam_hash) != 1 || tam_hash <= 0){
            printf("tamanho invalido\n");
        }
    }

    if (!est_ini(&est, conversorCM(via_m), tam_hash)) {
        printf("erro ao inicializar estacionamento\n");
        return 1;
    }

    do {
        printf("\n1 - estacionar veiculo\n2 - saida de veiculo\n3 - exibir estado\n0 - sair\nescolha: ");
        if (scanf("%d", &opcao) != 1) {
            break;
        }

        if (opcao == 1) {
            int id = 0;
            double m = 0.0;
            Status st;
            printf("ID do veiculo: ");
            if (scanf("%d", &id) != 1){ break;}
            printf("tamanho em metros: ");
            if (scanf("%lf", &m) != 1){ break;}

            st = est_entrar(&est, id, conversorCM(m));
            if (st == res_sucesso_estacionado) { printf("veiculo %d estacionado com sucesso\n", id);}
            else if (st == res_sucesso_enfileirado) { printf("sem vaga disponivel no momento o veiculo %d entrou na fila\n", id);}
            else if (st == res_erro_id_duplicado) { printf("erro: ID %d ja existe no sistema\n", id);}
            else if (st == res_erro_taminv) { printf("erro: tamanho invalido para a via\n");}
            else { printf("erro de memoria \n");}
            est_exibir(&est);
            
        } else if (opcao == 2) {
            int id = 0;
            printf("ID do veiculo para saida: ");
            if (scanf("%d", &id) != 1){ break;}

            if (est_saida(&est, id)){
                printf("veiculo %d saiu da via\n", id);
                while (est_ver_fila(&est, &id_fila, &tam_fila)){
                    char resp = 'n';
                    printf("veiculo %d (%.2f m) na fila possui vaga compativel, estacionar? (s/n): ", id_fila, conversorM(tam_fila));
                    scanf(" %c", &resp);
                    if (resp == 's' || resp == 'S'){
                        if (estacionar_fila(&est)){
                            printf("Veiculo %d estacionado!\n", id_fila);
                        }
                    }
                    else{
                        break;
                    }
                }
            }

            else {
                printf("veiculo %d nao encontrado ou nao esta estacionado\n", id);
            }
            est_exibir(&est);
        } else if (opcao == 3) { est_exibir(&est);}
    } while (opcao != 0);

    est_del(&est);
    return 0;
}
