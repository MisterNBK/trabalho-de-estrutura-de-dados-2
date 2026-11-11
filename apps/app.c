#include "logica.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    Estacionamento est;

    if (!est_ini(&est, 10)) {
        puts("falha ao inicializar estacionamento");
        return 1;
    }

    est_del(&est);
    return 0;
}

