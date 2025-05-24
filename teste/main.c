#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "funcoes.h"

int main() {
    struct dados_pedido pedidos[200];
    int total_pedidos = 0;

    // Carrega pedidos anteriores
    carregar_pedidos(pedidos, &total_pedidos);

    char continuar_programa;
    do {
        int mesa, pessoas;
        char continuar;

        top_bottom();

        // Dados gerais da mesa
        printf("Numero da mesa: ");
        scanf("%d", &mesa);

        printf("Quantidade de pessoas na mesa: ");
        scanf("%d", &pessoas);

        // Limpa buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        int inicio_pedidos_mesa = total_pedidos; // Marca início dos pedidos desta mesa
        char cupom_usado[20] = "";
        float desconto_aplicado = 0.0;

        // Registro de itens da mesa
        do {
            char item[50];
            int tipo, qtd;

            mid_line();

            printf("Item que deseja pedir: ");
            fgets(item, sizeof(item), stdin);
            item[strcspn(item, "\n")] = '\0';
            
            printf("Tipo do item (1 = Comida, 2 = Bebida): ");
            scanf("%d", &tipo);

            printf("Quantidade: ");
            scanf("%d", &qtd);

            // Registra e salva o item (com cupom vazio inicialmente)
            registrar_e_salvar(pedidos, &total_pedidos, mesa, pessoas, 
                             item, tipo, qtd, "", 0.0);

            // Limpa buffer
            while ((c = getchar()) != '\n' && c != EOF);

            printf("Deseja pedir mais alguma coisa? (s/n): ");
            scanf("%c", &continuar);

            while ((c = getchar()) != '\n' && c != EOF);

        } while (continuar == 's' || continuar == 'S');

        system("cls || clear");
        top_bottom();

        // Exibe pedidos da mesa
        printf("RESUMO DOS PEDIDOS DA MESA %d:\n", mesa);
        mid_line();

        float total_mesa = 0;
        for (int i = inicio_pedidos_mesa; i < total_pedidos; i++) {
            exibir_pedido(pedidos[i], 0); // 0 = modo cliente
            total_mesa += pedidos[i].subtotal;
        }

        // Sistema de cupons
        struct cupom_desconto cupons[MAX_CUPONS];
        inicializar_cupons(cupons);

        char usar_cupom;
        printf("Deseja usar um cupom de desconto? (s/n): ");
        scanf(" %c", &usar_cupom);

        mid_line();

        float total_final = total_mesa;
        if (usar_cupom == 's' || usar_cupom == 'S') {
            char codigo[20];
            printf("Digite o codigo do cupom: ");
            scanf("%19s", codigo);

            float desconto;
            if (aplicar_cupom(codigo, cupons, &desconto)) {
                // Atualiza o último pedido com o cupom
                strcpy(pedidos[total_pedidos-1].cupom, codigo);
                pedidos[total_pedidos-1].desconto = desconto;
                
                // Atualiza o arquivo binário
                FILE *arquivo = fopen(ARQUIVO_BINARIO, "r+b");
                if (arquivo) {
                    fseek(arquivo, (total_pedidos-1)*sizeof(struct dados_pedido), SEEK_SET);
                    fwrite(&pedidos[total_pedidos-1], sizeof(struct dados_pedido), 1, arquivo);
                    fclose(arquivo);
                }

                total_final = total_mesa * (1 - desconto);
                printf("\nCUPOM %s APLICADO (%.0f%% DE DESCONTO)\n", codigo, desconto*100);

                mid_line();

                printf("VALOR ORIGINAL: R$%.2f\n", total_mesa);
            } else {
                printf("Cupom invalido. Nenhum desconto aplicado.\n");

                mid_line();
            }
        }

        printf("TOTAL DA MESA %d: R$%.2f | VALOR POR PESSOA: R$%.2f\n", 
               mesa, total_final, total_final/pessoas);
        top_bottom();

        printf("Deseja registrar pedidos de outra mesa? (s/n): ");
        scanf(" %c", &continuar_programa);
        system("cls || clear");

    } while (continuar_programa == 's' || continuar_programa == 'S');

    // Relatório final
    char ver_relatorio;
    printf("\nDeseja visualizar o relatorio final do dia? (s/n): ");
    scanf(" %c", &ver_relatorio);

    if (ver_relatorio == 's' || ver_relatorio == 'S') {
        system("cls || clear");
        gerar_relatorio_final();
    }

    return 0;
}