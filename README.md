direto no terminal: 
compilar-> gcc -std=c11 -Wall -Wextra -pedantic -g -fsanitize=address,undefined -Iinclude apps/app.c src/avl.c src/logica.c -o bin/estacionamento
executar -> ./bin/estacionamento
apagar -> rm -rf obj/* bin/*


pelo makefile: make all

