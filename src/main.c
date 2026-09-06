#include <stdio.h>
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
    sceSifInitRpc(0);

    /*
     * Inicializa o controle.
     */
    if (!input_init())
    {
        /*
         * Nao usamos mais debug screen nesta versao.
         * Se o controle falhar, simplesmente permanecemos
         * no programa para evitar chamadas de APIs
         * desnecessarias.
         */
        while (1)
        {
        }
    }

    /*
     * Inicializa a interface grafica.
     */
    ui_init();

    /*
     * Loop principal.
     */
    while (1)
    {
        ui_update();
        ui_render();
    }

    ui_shutdown();
    input_shutdown();

    return 0;
}
