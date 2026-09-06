#include <stdio.h>
#include <debug.h>
#include <sifrpc.h>

#include "input.h"
#include "ui.h"
#include "network.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /*
     * Inicializa RPC.
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
     * Inicializa rede.
     */
    if (!network_init())
    {
        scr_printf("ERRO: falha ao inicializar rede.\n");
        scr_printf("Pressione START para continuar.\n");
    }

    /*
     * Inicializa interface.
     */
    ui_init();

    /*
     * Loop principal.
     *
     * ui_update() ja chama input_update().
     */
    while (1)
    {
        ui_update();
        ui_render();
    }

    /*
     * Limpeza.
     */
    network_shutdown();
    input_shutdown();

    return 0;
}
