#include <stdio.h>
#include <debug.h>

int main(int argc, char *argv[])
{
    volatile unsigned int counter = 0;

    (void)argc;
    (void)argv;

    init_scr();
    scr_clear();

    while (1)
    {
        counter++;

        /*
         * Atualiza a tela somente de tempos em tempos.
         * Nao usamos SleepThread nem DelayThread.
         */
        if (counter >= 5000000)
        {
            counter = 0;

            scr_clear();

            scr_printf("\n\n");
            scr_printf("========================================\n");
            scr_printf("\n");
            scr_printf("                 P S 2\n");
            scr_printf("\n");
            scr_printf("                 [ ]\n");
            scr_printf("\n");
            scr_printf("              OPL NEXT\n");
            scr_printf("\n");
            scr_printf("========================================\n");
            scr_printf("\n");
            scr_printf("PROGRAMA RODANDO...\n");
        }
    }

    return 0;
}
