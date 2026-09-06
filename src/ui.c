#include "ui.h"
#include "input.h"

#include <stdio.h>
#include <debug.h>

static int selected = 0;

#define MENU_COUNT 3

static const char *menu_items[MENU_COUNT] =
{
    "JOGOS",
    "REDE",
    "CONFIGURACOES"
};

void ui_init(void)
{
    selected = 0;
}

void ui_update(void)
{
    const InputState *input;

    input = input_get();

    if (input == NULL)
        return;

    if (input->up)
    {
        selected--;

        if (selected < 0)
            selected = MENU_COUNT - 1;
    }

    if (input->down)
    {
        selected++;

        if (selected >= MENU_COUNT)
            selected = 0;
    }

    /*
     * Por enquanto X apenas mostra a opcao selecionada.
     * Acoes reais serao adicionadas depois.
     */
    if (input->cross)
    {
        /* Mantemos o menu funcionando. */
    }
}

void ui_render(void)
{
    int i;

    scr_clear();

    scr_printf("\n");
    scr_printf("========================================\n");
    scr_printf("              OPL NEXT                  \n");
    scr_printf("========================================\n");
    scr_printf("\n");

    scr_printf("MENU PRINCIPAL\n");
    scr_printf("\n");

    for (i = 0; i < MENU_COUNT; i++)
    {
        if (i == selected)
            scr_printf(" > %s\n", menu_items[i]);
        else
            scr_printf("   %s\n", menu_items[i]);
    }

    scr_printf("\n");
    scr_printf("----------------------------------------\n");
    scr_printf(" CIMA / BAIXO : Navegar\n");
    scr_printf(" X             : Selecionar\n");
    scr_printf("----------------------------------------\n");
}
