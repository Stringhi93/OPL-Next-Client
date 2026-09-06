#include <stdio.h>
#include <debug.h>

#include "input.h"
#include "ui.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /*
     * Inicializa a tela de texto do PS2SDK.
     */
    init_scr();
    scr_clear();

    /*
     * Inicializa o controle.
     */
    input_init();

    /*
     * Inicializa a interface.
     */
    ui_init();

    /*
     * Loop principal.
     *
     * Nao usamos SleepThread nem DelayThread.
     * A V0.1 ja demonstrou que a tela de texto funciona
     * dessa maneira no seu PS2.
     */
    while (1)
    {
        input_update();
        ui_update();
        ui_render();
    }

    return 0;
}
