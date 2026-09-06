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
    scr_printf("UP/DOWN: Navegar\n");
    scr_printf("X: Selecionar\n");
}

static void draw_network(void)
{
    scr_printf("========================================\n");
    scr_printf("                  REDE\n");
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
    else if (network_result == -3)
    {
        scr_printf("SERVIDOR: ERRO SOCKET\n");
    }

    scr_printf("\n");
    scr_printf("X: Testar servidor\n");
    scr_printf("O: Voltar\n");
}

void ui_init(void)
{
    menu_selected = 0;
    network_result = -1;
}

void ui_update(void)
{
    /*
     * Menu principal.
     */

    if (input_pressed(INPUT_UP))
    {
        if (menu_selected > 0)
            menu_selected--;
    }

    if (input_pressed(INPUT_DOWN))
    {
        if (menu_selected < 2)
            menu_selected++;
    }

    /*
     * Selecionar.
     */

    if (input_pressed(INPUT_CROSS))
    {
        if (menu_selected == 1)
        {
            /*
             * Teste inicial do servidor.
             *
             * PC:
             * 192.168.1.8
             *
             * Porta:
             * 21
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
    {
        draw_network();
    }
    else
    {
        draw_main_menu();
    }
}
