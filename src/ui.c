#include <stdio.h>
#include <debug.h>

#include "ui.h"
#include "input.h"

static int selected = 0;
static int needs_redraw = 1;

static const char *menu_items[] =
{
    "JOGOS",
    "TESTE DE REDE",
    "CONFIGURACOES"
};

#define MENU_COUNT 3

void ui_init(void)
{
    selected = 0;
    needs_redraw = 1;
}

void ui_update(void)
{
    const InputState *in;

    input_update();

    in = input_get();

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

    /*
     * X:
     * por enquanto apenas confirma a opção.
     *
     * O teste real de rede será colocado na próxima etapa,
     * depois que confirmarmos que o menu e o controle estão
     * funcionando de forma estável.
     */
    if (in->cross)
    {
        needs_redraw = 1;
    }
}

void ui_render(void)
{
    int i;

    if (!needs_redraw)
        return;

    scr_clear();

    scr_printf("========================================\n");
    scr_printf("              OPL NEXT\n");
    scr_printf("========================================\n\n");

    scr_printf("MENU PRINCIPAL\n\n");

    for (i = 0; i < MENU_COUNT; i++)
    {
        if (i == selected)
            scr_printf(" > %s\n", menu_items[i]);
        else
            scr_printf("   %s\n", menu_items[i]);
    }

    scr_printf("\n");
    scr_printf("----------------------------------------\n");
    scr_printf("CIMA/BAIXO : MOVER\n");
    scr_printf("X          : SELECIONAR\n");
    scr_printf("----------------------------------------\n");

    needs_redraw = 0;
}
