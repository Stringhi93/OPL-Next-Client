#include "ui.h"
#include "input.h"

#include <stdio.h>
#include <debug.h>

typedef enum
{
    SCREEN_MAIN = 0,
    SCREEN_GAMES,
    SCREEN_NETWORK,
    SCREEN_SETTINGS
} UIScreen;

static UIScreen screen = SCREEN_MAIN;

static int selected = 0;

static const char *menu_items[] =
{
    "JOGOS",
    "REDE",
    "CONFIGURACOES"
};

#define MENU_COUNT 3

static const char *screen_name(void)
{
    switch (screen)
    {
        case SCREEN_GAMES:
            return "JOGOS";

        case SCREEN_NETWORK:
            return "REDE";

        case SCREEN_SETTINGS:
            return "CONFIGURACOES";

        default:
            return "OPL NEXT";
    }
}

void ui_init(void)
{
    screen = SCREEN_MAIN;
    selected = 0;
}

void ui_update(void)
{
    const InputState *input;

    input = input_get();

    if (screen == SCREEN_MAIN)
    {
        /*
         * Navegar para cima.
         */
        if (input->up)
        {
            selected--;

            if (selected < 0)
            {
                selected = MENU_COUNT - 1;
            }
        }

        /*
         * Navegar para baixo.
         */
        if (input->down)
        {
            selected++;

            if (selected >= MENU_COUNT)
            {
                selected = 0;
            }
        }

        /*
         * X abre a opcao selecionada.
         */
        if (input->cross)
        {
            switch (selected)
            {
                case 0:
                    screen = SCREEN_GAMES;
                    break;

                case 1:
                    screen = SCREEN_NETWORK;
                    break;

                case 2:
                    screen = SCREEN_SETTINGS;
                    break;
            }
        }
    }
    else
    {
        /*
         * O volta para o menu principal.
         */
        if (input->circle)
        {
            screen = SCREEN_MAIN;
        }
    }
}

void ui_render(void)
{
    int i;

    scr_clear();

    scr_printf("========================================\n");
    scr_printf("              OPL NEXT                  \n");
    scr_printf("========================================\n\n");

    if (screen == SCREEN_MAIN)
    {
        scr_printf("MENU PRINCIPAL\n\n");

        for (i = 0; i < MENU_COUNT; i++)
        {
            if (i == selected)
            {
                scr_printf(" > %s\n", menu_items[i]);
            }
            else
            {
                scr_printf("   %s\n", menu_items[i]);
            }
        }

        scr_printf("\n");
        scr_printf("----------------------------------------\n");
        scr_printf(" CIMA/BAIXO : Navegar\n");
        scr_printf(" X          : Selecionar\n");
        scr_printf(" O          : Voltar\n");
        scr_printf(" START      : Em breve\n");
        scr_printf("----------------------------------------\n");
    }
    else
    {
        scr_printf("%s\n\n", screen_name());

        scr_printf("----------------------------------------\n\n");

        switch (screen)
        {
            case SCREEN_GAMES:

                scr_printf("Biblioteca de jogos\n\n");
                scr_printf("Nenhum jogo carregado ainda.\n\n");
                scr_printf("A funcao de jogos sera adicionada\n");
                scr_printf("em uma proxima versao.\n");

                break;

            case SCREEN_NETWORK:

                scr_printf("Configuracao de rede\n\n");
                scr_printf("Rede ainda nao configurada.\n\n");
                scr_printf("O teste de rede sera adicionado\n");
                scr_printf("em uma proxima versao.\n");

                break;

            case SCREEN_SETTINGS:

                scr_printf("Configuracoes\n\n");
                scr_printf("Configuracoes do OPL Next.\n\n");
                scr_printf("Mais opcoes serao adicionadas\n");
                scr_printf("posteriormente.\n");

                break;

            default:
                break;
        }

        scr_printf("\n");
        scr_printf("----------------------------------------\n");
        scr_printf(" O : Voltar ao menu principal\n");
        scr_printf("----------------------------------------\n");
    }
}
