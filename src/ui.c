#include "ui.h"
#include "input.h"

#include <debug.h>

#define SCREEN_MAIN     0
#define SCREEN_GAMES    1
#define SCREEN_NETWORK  2
#define SCREEN_CONFIG   3

#define MENU_COUNT 3

static int screen = SCREEN_MAIN;
static int selected = 0;
static int needs_redraw = 1;

static const char *main_menu[] =
{
    "JOGOS",
    "TESTE DE REDE",
    "CONFIGURACOES"
};


/*
 * Tela principal.
 */
static void draw_main_menu(void)
{
    int i;

    scr_printf("========================================\n");
    scr_printf("              OPL NEXT\n");
    scr_printf("========================================\n\n");

    scr_printf("MENU PRINCIPAL\n\n");

    for (i = 0; i < MENU_COUNT; i++)
    {
        if (i == selected)
            scr_printf(" > %s\n", main_menu[i]);
        else
            scr_printf("   %s\n", main_menu[i]);
    }

    scr_printf("\n");
    scr_printf("----------------------------------------\n");
    scr_printf("CIMA/BAIXO : MOVER\n");
    scr_printf("X           : ENTRAR\n");
    scr_printf("O           : VOLTAR\n");
    scr_printf("----------------------------------------\n");
}


/*
 * Tela de jogos.
 */
static void draw_games(void)
{
    scr_printf("========================================\n");
    scr_printf("                 JOGOS\n");
    scr_printf("========================================\n\n");

    scr_printf("A lista de jogos sera adicionada\n");
    scr_printf("na proxima etapa.\n\n");

    scr_printf("Rede SMB planejada:\n");
    scr_printf("Servidor: 192.168.1.8\n");
    scr_printf("Share:    ps2\n");
    scr_printf("Pasta:    DVD\n\n");

    scr_printf("----------------------------------------\n");
    scr_printf("O : VOLTAR\n");
    scr_printf("----------------------------------------\n");
}


/*
 * Tela de teste de rede.
 */
static void draw_network(void)
{
    scr_printf("========================================\n");
    scr_printf("             TESTE DE REDE\n");
    scr_printf("========================================\n\n");

    scr_printf("SERVIDOR DE TESTE\n\n");

    scr_printf("IP DO PC:\n");
    scr_printf("192.168.1.8\n\n");

    scr_printf("PORTA:\n");
    scr_printf("445\n\n");

    scr_printf("STATUS:\n");

    if (network_is_ready())
        scr_printf("LINK CONECTADO\n");
    else
        scr_printf("LINK DESCONECTADO\n");

    scr_printf("\n");
    scr_printf("----------------------------------------\n");
    scr_printf("X : INICIAR TESTE\n");
    scr_printf("O : VOLTAR\n");
    scr_printf("----------------------------------------\n");
}


/*
 * Tela de configurações.
 */
static void draw_config(void)
{
    scr_printf("========================================\n");
    scr_printf("             CONFIGURACOES\n");
    scr_printf("========================================\n\n");

    scr_printf("CONFIGURACAO SMB\n\n");

    scr_printf("Servidor : 192.168.1.8\n");
    scr_printf("Share    : ps2\n");
    scr_printf("Pasta    : DVD\n");
    scr_printf("Porta    : 445\n\n");

    scr_printf("Modo de teste:\n");
    scr_printf("Rede local\n\n");

    scr_printf("----------------------------------------\n");
    scr_printf("O : VOLTAR\n");
    scr_printf("----------------------------------------\n");
}


/*
 * Inicializa a interface.
 */
void ui_init(void)
{
    screen = SCREEN_MAIN;
    selected = 0;
    needs_redraw = 1;
}


/*
 * Atualiza a interface.
 */
void ui_update(void)
{
    const InputState *in;

    /*
     * Atualiza o controle uma única vez.
     */
    input_update();

    in = input_get();

    /*
     * MENU PRINCIPAL
     */
    if (screen == SCREEN_MAIN)
    {
        if (in->up)
        {
            if (selected > 0)
                selected--;

            needs_redraw = 1;
        }

        if (in->down)
        {
            if (selected < MENU_COUNT - 1)
                selected++;

            needs_redraw = 1;
        }

        if (in->cross)
        {
            if (selected == 0)
            {
                screen = SCREEN_GAMES;
            }
            else if (selected == 1)
            {
                screen = SCREEN_NETWORK;
            }
            else if (selected == 2)
            {
                screen = SCREEN_CONFIG;
            }

            needs_redraw = 1;
        }
    }
    /*
     * TELAS INTERNAS
     */
    else
    {
        /*
         * X na tela de teste de rede.
         */
        if (screen == SCREEN_NETWORK && in->cross)
        {
            int result;

            scr_clear();

            scr_printf("========================================\n");
            scr_printf("             TESTE DE REDE\n");
            scr_printf("========================================\n\n");

            scr_printf("Conectando ao PC...\n");
            scr_printf("192.168.1.8:445\n\n");

            result = network_test_server(
                "192.168.1.8",
                445
            );

            if (result == 0)
            {
                scr_printf("RESULTADO: CONECTADO!\n");
            }
            else if (result == -1)
            {
                scr_printf("RESULTADO: REDE NAO INICIALIZADA\n");
            }
            else if (result == -2)
            {
                scr_printf("RESULTADO: LINK DESCONECTADO\n");
            }
            else if (result == -3)
            {
                scr_printf("RESULTADO: ERRO AO CRIAR SOCKET\n");
            }
            else
            {
                scr_printf("RESULTADO: FALHA NA CONEXAO\n");
            }

            scr_printf("\nPressione O para voltar.\n");

            /*
             * Evita redesenhar imediatamente a tela.
             */
            needs_redraw = 0;

            return;
        }

        /*
         * O volta ao menu principal.
         */
        if (in->circle)
        {
            screen = SCREEN_MAIN;
            selected = 0;
            needs_redraw = 1;
        }

        /*
         * START também volta ao menu.
         */
        if (in->start)
        {
            screen = SCREEN_MAIN;
            selected = 0;
            needs_redraw = 1;
        }
    }
}


/*
 * Desenha a tela atual.
 */
void ui_render(void)
{
    if (!needs_redraw)
        return;

    scr_clear();

    if (screen == SCREEN_MAIN)
    {
        draw_main_menu();
    }
    else if (screen == SCREEN_GAMES)
    {
        draw_games();
    }
    else if (screen == SCREEN_NETWORK)
    {
        draw_network();
    }
    else if (screen == SCREEN_CONFIG)
    {
        draw_config();
    }

    needs_redraw = 0;
}
