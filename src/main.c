#include <stdio.h>
#include <kernel.h>
#include <sifrpc.h>
#include <debug.h>

#include "input.h"
#include "ui.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /*
     * Inicializa o sistema RPC do PS2.
     */
    sceSifInitRpc(0);

    /*
     * Inicializa o sistema de controle.
     */
    if (!input_init())
    {
        init_scr();
        scr_clear();

        scr_printf("\n");
        scr_printf("============================================================\n");
        scr_printf("                    OPL NEXT CLIENT                         \n");
        scr_printf("============================================================\n\n");

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
     * Inicializa a interface.
     */
    ui_init();

    /*
     * Desenha a primeira tela.
     */
    ui_render();

    /*
     * Loop principal.
     */
    while (1)
    {
        ui_update();

        ui_render();

        /*
         * Pequena pausa para evitar uso desnecessario
         * da CPU enquanto aguardamos o proximo frame.
         */
        DelayThread(16000);
    }

    /*
     * Nunca deve chegar aqui, mas mantemos a
     * finalizacao organizada.
     */
    ui_shutdown();
    input_shutdown();

    return 0;
}
