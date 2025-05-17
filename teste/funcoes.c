#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "funcoes.h"

// Imprime uma linha intermediária (visual)
void mid_line() {
    printf("-----------------------------------------------------\n");
}

// Imprime uma linha de topo/rodapé (visual)
void top_bottom() {
    printf("=====================================================\n");
}

// Registra um pedido e salva no arquivo binário
void registrar_e_salvar(
    struct dados_pedido *vetor,
    int *total_pedidos,
    int mesa,
    int pessoas,
    char item[],
    int tipo,
    int qtd_item
) {
    struct dados_pedido pedido;

    // Preenchimento da struct com os dados recebidos
    pedido.mesa = mesa;
    pedido.pessoas = pessoas;
    strncpy(pedido.item, item, sizeof(pedido.item));
    pedido.item[sizeof(pedido.item) - 1] = '\0';
    pedido.tipo = tipo;
    pedido.quantidade = qtd_item;

    // Preços fictícios
    float valor_comida = 30.0;
    float valor_bebida = 5.0;

    // Cálculo do subtotal baseado no tipo
    if (tipo == 1) {
        pedido.subtotal = qtd_item * valor_comida;
    } else if (tipo == 2) {
        pedido.subtotal = qtd_item * valor_bebida;
    } else {
        return;
    }

    // Valor individual da conta
    pedido.valor_individual = pedido.subtotal / pessoas;

    // Adiciona ao vetor
    vetor[*total_pedidos] = pedido;
    (*total_pedidos)++;

    // Salva no arquivo no modo append
    FILE *arquivo = fopen(ARQUIVO_BINARIO, "ab");
    if (arquivo == NULL) {
        printf("Erro ao salvar pedido no arquivo binario\n");
        return;
    }

    fwrite(&pedido, sizeof(struct dados_pedido), 1, arquivo);
    fclose(arquivo);
}

// Carrega os pedidos do arquivo binário para o vetor
int carregar_pedidos(struct dados_pedido vetor[], int *total_pedidos) {
    FILE *arquivo = fopen(ARQUIVO_BINARIO, "rb");
    if (arquivo == NULL) {
        *total_pedidos = 0;
        return 0;
    }

    int lidos = fread(vetor, sizeof(struct dados_pedido), 200, arquivo);
    fclose(arquivo);

    *total_pedidos = lidos;
    return lidos;
}

// Exibe um pedido individual
void exibir_pedido(struct dados_pedido p) {
    printf("Mesa %d | Pessoas: %d\n", p.mesa, p.pessoas);
    printf("Item: %s (%dx) [%s]\n", p.item, p.quantidade,
           (p.tipo == 1 ? "Comida" : "Bebida"));
    printf("Subtotal: R$%.2f | Valor por pessoa: R$%.2f\n", p.subtotal, p.valor_individual);
    mid_line();
}

// Exibe um resumo de todos os pedidos
void exibir_resumo(struct dados_pedido vetor[], int total_pedidos) {
    float total_geral = 0;
    printf("\n === RESUMO DOS PEDIDOS === \n");

    for (int i = 0; i < total_pedidos; i++) {
        exibir_pedido(vetor[i]);
        total_geral += vetor[i].subtotal;
    }

    printf("TOTAL ARRECADADO: R$%.2f\n", total_geral);
    top_bottom();
}

// Gera o relatório final do dia com item mais pedido e total arrecadado por mesa
void gerar_relatorio_final() {
    struct dados_pedido pedidos[200];
    int total_pedidos = 0;

    if (!carregar_pedidos(pedidos, &total_pedidos)) {
        printf("Nenhum pedido registrado ainda.\n");
        return;
    }

    printf("\n=== RELATORIO FINAL DO DIA ===\n");

    // Total por mesa
    int mesas_exibidas[100] = {0};
    int n_mesas = 0;

    for (int i = 0; i < total_pedidos; i++) {
        int ja_exibida = 0;
        for (int j = 0; j < n_mesas; j++) {
            if (mesas_exibidas[j] == pedidos[i].mesa) {
                ja_exibida = 1;
                break;
            }
        }

        if (!ja_exibida) {
            int mesa_atual = pedidos[i].mesa;
            float total_mesa = 0;
            printf("\nMESA %d\n", mesa_atual);

            mid_line();

            for (int k = 0; k < total_pedidos; k++) {
                if (pedidos[k].mesa == mesa_atual) {
                    exibir_pedido(pedidos[k]);
                    total_mesa += pedidos[k].subtotal;
                }
            }

            printf("TOTAL DA MESA %d: R$%.2f\n", mesa_atual, total_mesa);
            top_bottom();
            mesas_exibidas[n_mesas++] = mesa_atual;
        }
    }

    // Determinar o item mais pedido
    char nomes[50][50];
    int quantidades[50] = {0};
    int n_itens = 0;

    for (int i = 0; i < total_pedidos; i++) {
        int encontrado = 0;
        for (int j = 0; j < n_itens; j++) {
            if (strcmp(pedidos[i].item, nomes[j]) == 0) {
                quantidades[j] += pedidos[i].quantidade;
                encontrado = 1;
                break;
            }
        }

        if (!encontrado) {
            strcpy(nomes[n_itens], pedidos[i].item);
            quantidades[n_itens] = pedidos[i].quantidade;
            n_itens++;
        }
    }

    // Encontra o item com maior quantidade
    int max_index = 0;
    for (int i = 1; i < n_itens; i++) {
        if (quantidades[i] > quantidades[max_index]) {
            max_index = i;
        }
    }

    printf("ITEM MAIS PEDIDO: %s (%d unidades)\n", nomes[max_index], quantidades[max_index]);
    top_bottom();
}
