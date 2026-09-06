#include <stdio.h>
#include <debug.h>

#include "ui.h"
#include "input.h"
#include "network.h"

static int menu_selected = 0;
static int network_result = -1;

static void draw_header(void)
{
    scr_printf("========================================\n");
    scr_printf("          OPL NEXT CLIENT\n");
    scr_printf("========================================\n");
    scr_printf("\n");
}

static void draw_main_menu(void)
{
    draw_header();

    scr_printf("%s Iniciar\n",
               menu_selected == 0 ? ">" : " ");

    scr_printf("%s Rede\n",
               menu_selected == 1 ? ">" : " ");

    scr_printf("%s Sair\n",
               menu_selected == 2 ? ">" : " ");

    scr_printf("\n");
    scr_printf("Use UP/DOWN para navegar\n");
    scr_printf("X para selecionar\n");
}

static void draw_network(void)
{
    scr_printf("========================================\n");
    scr_printf("              REDE\n");
    scr_printf("========================================\n");
    scr_printf("\n");

    if (network_is_ready())
    {
        scr_printf("LINK: CONECTADO\n");
        scr_printf("IP:   192.168.1.20\n");
    }
    else
    {
        scr_printf("LINK: DESCONECTADO\n");
    }

    scr_printf("\n");

    if (network_result == 0)
    {
        scr_printf("SERVIDOR: CONECTADO\n");
    }
    else if (network_result == -4)
    {
        scr_printf("SERVIDOR: NAO CONECTOU\n");
    }
    else if (network_result == -2)
    {
        scr_printf("SERVIDOR: SEM LINK\n");
    }

    scr_printf("\n");
    scr_printf("Pressione CIRCULO para voltar\n");
}

void ui_init(void)
{
    menu_selected = 0;
    network_result = -1;
}

void ui_update(void)
{
    /*
     * Navegacao simples.
     *
     * Se o seu input.c usar nomes diferentes para os botoes,
     * ajuste somente esta parte.
     */

    if (input_pressed(PAD_UP))
    {
        if (menu_selected > 0)
            menu_selected--;
    }

    if (input_pressed(PAD_DOWN))
    {
        if (menu_selected < 2)
            menu_selected++;
    }

    if (input_pressed(PAD_CROSS))
    {
        if (menu_selected == 1)
        {
            /*
             * Teste do servidor.
             *
             * Troque o IP/porta posteriormente pelo servidor
             * que o OPL Next Client realmente vai utilizar.
             */
            network_result =
                network_test_server("192.168.1.8", 21);
        }
    }
}

void ui_render(void)
{
    scr_clear();

    if (menu_selected == 1)
        draw_network();
    else
        draw_main_menu();
}
