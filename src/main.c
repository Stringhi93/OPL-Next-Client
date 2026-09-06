#include <stdio.h>

#include <kernel.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <loadfile.h>
#include <sbv_patches.h>
#include <debug.h>

#include "input.h"
#include "ui.h"
#include "network.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /*
     * Reinicia o IOP para iniciar em estado limpo.
     */
    SifInitRpc(0);

    while (!SifIopReset("", 0))
        ;

    while (!SifIopSync())
        ;

    /*
     * Inicializa servicos SIF.
     */
    SifInitRpc(0);

    SifLoadFileInit();

    sbv_patch_enable_lmb();

    /*
     * Tela.
     */
    init_scr();
    scr_clear();

    scr_printf("========================================\n");
    scr_printf("          OPL NEXT CLIENT\n");
    scr_printf("========================================\n\n");

    /*
     * Controle.
     */
    input_init();

    /*
     * Rede.
     */
    network_init();

    /*
     * Interface.
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
         * Evita consumir 100% da EE.
         */
        nopdelay();
    }

    /*
     * Normalmente nunca chega aqui.
     */
    network_shutdown();
    input_shutdown();

    SifExitRpc();

    return 0;
}
