#include <stdio.h>
#include <debug.h>
#include <kernel.h>
#include <sifrpc.h>

#include "input.h"
#include "ui.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /*
     * Inicializa os serviços RPC necessários ao controle.
     */
    SifInitRpc(0);

    /*
     * Inicializa tela.
     */
    init_scr();
    scr_clear();

    /*
     * Inicializa controle.
     */
    input_init();

    /*
     * Inicializa a interface.
     */
    ui_init();

    /*
     * Loop principal.
     */
    while (1)
    {
        input_update();
        ui_update();
        ui_render();

        /*
         * Pequena pausa para não ocupar 100% da EE.
         * 1 tick = aproximadamente 1 microsegundo.
         */
        DelayThread(10000);
    }

    input_shutdown();

    return 0;
}
