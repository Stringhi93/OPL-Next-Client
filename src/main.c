#include <stdio.h>
#include <kernel.h>
#include <sifrpc.h>

#include "input.h"
#include "ui.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /*
     * Initialize SIF RPC.
     */
    sceSifInitRpc(0);

    /*
     * Initialize controller.
     */
    if (!input_init())
    {
        init_scr();
        scr_clear();

        scr_printf("\n");
        scr_printf("==================================================\n");
        scr_printf("                 OPL NEXT CLIENT                 \n");
        scr_printf("==================================================\n\n");

        scr_printf("ERRO: nao foi possivel inicializar o controle.\n\n");

        scr_printf("Verifique se o DualShock 2 esta conectado\n");
        scr_printf("na porta 1 do PlayStation 2.\n\n");

        scr_printf("O programa continuara executando.\n");

        while (1)
        {
            DelayThread(10000);
        }

        return 1;
    }

    /*
     * Initialize user interface.
     */
    ui_init();

    /*
     * Initial frame.
     */
    ui_render();

    /*
     * Main application loop.
     */
    while (1)
    {
        ui_update();

        /*
         * Render the main screen.
         *
         * Sub-pages currently draw themselves directly.
         * This will be replaced by a proper state machine
         * in the next UI iteration.
         */
        ui_render();

        /*
         * Small delay prevents unnecessary CPU usage
         * while keeping controller response comfortable.
         */
        DelayThread(16000);
    }

    ui_shutdown();
    input_shutdown();

    return 0;
}
#include <stdio.h>
#include <debug.h>

int main(int argc, char *argv[])
{
    init_scr();

    scr_clear();

    scr_printf("========================================\n");
    scr_printf("          OPL NEXT CLIENT\n");
    scr_printf("========================================\n\n");

    scr_printf("ELF iniciado com sucesso!\n\n");

    scr_printf("Projeto: OPL Next\n");
    scr_printf("Plataforma: PlayStation 2\n");
    scr_printf("Servidor: TV Box\n");
    scr_printf("Modo futuro: Rede / SMB\n\n");

    scr_printf("----------------------------------------\n");
    scr_printf("Build test OK!\n");
    scr_printf("----------------------------------------\n");

    while (1)
    {
        /* Programa permanece executando. */
    } 

    return 0;
}
