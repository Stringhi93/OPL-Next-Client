#include <stdio.h>
#include <debug.h>
#include <sifrpc.h>

#include "input.h"
#include "ui.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /*
     * Inicializa RPC.
     */
    SifInitRpc(0);

    /*
     * Inicializa a tela.
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
     * Não usamos DelayThread porque algumas versões/
     * configurações do PS2SDK não expõem essa função
     * diretamente.
     */
    while (1)
    {
        input_update();
        ui_update();
        ui_render();
    }

    input_shutdown();

    return 0;
}
