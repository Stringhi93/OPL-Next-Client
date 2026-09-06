#include <stdio.h>
#include <debug.h>
#include <kernel.h>

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
    if (!input_init())
    {
        scr_printf("Erro ao inicializar o controle.\n");
        scr_printf("\n");
        scr_printf("Pressione RESET para sair.\n");

        while (1)
        {
            SleepThread();
        }
    }

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
         * Pequena pausa para nao ocupar 100%% da CPU.
         */
        DelayThread(10000);
    }

    input_shutdown();

    return 0;
}
