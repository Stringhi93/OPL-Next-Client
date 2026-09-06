#include "ui.h"
#include "input.h"
#include "network.h"

#define SCREEN_MAIN     0
#define SCREEN_GAMES    1
#define SCREEN_NETWORK  2
#define SCREEN_CONFIG   3

#define MENU_COUNT 3

static int screen = SCREEN_MAIN;
static int selected = 0;
static int needs_redraw = 1;

static int network_tested = 0;
static int network_result = -99;

static const char *main_menu[] =
{
    "JOGOS",
    "TESTE DE REDE",
    "CONFIGURACOES"
};

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
    scr_printf("X          : ENTRAR\n");
    scr_printf("O          : VOLTAR\n");
    scr_printf("----------------------------------------\n");
}

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

static void draw_network(void)
{
    scr_printf("========================================\n");
    scr_printf("             TESTE DE REDE\n");
    scr_printf("========================================\n\n");

    scr_printf("PS2:\n");
    scr_printf("192.168.1.20\n\n");

    scr_printf("PC:\n");
    scr_printf("192.168.1.8\n\n");

    scr_printf("SMB:\n");
    scr_printf("ps2\n");
    scr_printf("Porta: 445\n\n");

    scr_printf("STATUS:\n");

    if (!network_is_ready())
    {
        scr_printf("REDE DESCONECTADA\n");
    }
    else if (!network_tested)
    {
        scr_printf("REDE CONECTADA\n");
        scr_printf("AGUARDANDO TESTE\n");
    }
    else if (network_result == 0)
    {
        scr_printf("CONEXAO COM PC: OK\n");
        scr_printf("SMB: PORTA 445 OK\n");
    }
    else
    {
        scr_printf("CONEXAO COM PC: FALHOU\n");
        scr_printf("VERIFIQUE O PC E O SMB\n");
    }

    scr_printf("\n");
    scr_printf("----------------------------------------\n");
    scr_printf("X : INICIAR TESTE\n");
    scr_printf("O : VOLTAR\n");
    scr_printf("----------------------------------------\n");
}

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

void ui_init(void)
{
    screen = SCREEN_MAIN;
    selected = 0;
    needs_redraw = 1;
    network_tested = 0;
    network_result = -99;
}

void ui_update(void)
{
    const InputState *in;

    in = input_get();

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
                screen = SCREEN_GAMES;
            else if (selected == 1)
                screen = SCREEN_NETWORK;
            else
                screen = SCREEN_CONFIG;

            needs_redraw = 1;
        }
    }
    else if (screen == SCREEN_NETWORK)
    {
        if (in->cross)
        {
            network_tested = 1;

            network_result =
                network_test_server(
                    "192.168.1.8",
                    445
                );

            needs_redraw = 1;
        }

        if (in->circle || in->start)
        {
            screen = SCREEN_MAIN;
            needs_redraw = 1;
        }
    }
    else
    {
        if (in->circle || in->start)
        {
            screen = SCREEN_MAIN;
            needs_redraw = 1;
        }
    }
}

void ui_render(void)
{
    if (!needs_redraw)
        return;

    scr_clear();

    if (screen == SCREEN_MAIN)
        draw_main_menu();
    else if (screen == SCREEN_GAMES)
        draw_games();
    else if (screen == SCREEN_NETWORK)
        draw_network();
    else if (screen == SCREEN_CONFIG)
        draw_config();

    needs_redraw = 0;
}
