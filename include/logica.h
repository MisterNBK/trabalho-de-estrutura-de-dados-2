#ifndef LOGICA_H
#define LOGICA_H

#include "logica.h"

typedef struct Nohash{
    Veiculo veiculo;
    struct Nohash *next;
} Nohash;

typedef struct Hash{
    Nohash **tabela;
    int tamanho;
} Hash;

#endif