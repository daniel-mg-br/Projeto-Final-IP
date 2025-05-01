#include <stdio.h>
#include "funcoes.h"

int main() {
    // Exemplo de entrada pronta
    struct dados_pedido p1 = registrar_pedido(3, "Lasanha", 2, "Suco de Laranja", 3);
    struct dados_pedido p2 = registrar_pedido(5, "Strogonoff", 1, "Refrigerante", 2);

    printf("---- Pedido 1 ----\n");
    exibir_pedido(p1);

    printf("\n---- Pedido 2 ----\n");
    exibir_pedido(p2);

    return 0;
}