#include <stdio.h>
#include <debug.h>
#include <kernel.h>

int main(int argc, char *argv[])
{
    int blink = 0;

    (void)argc;
    (void)argv;

    init_scr();
    scr_clear();

    while (1)
    {
        scr_clear();

        scr_printf("\n\n");
        scr_printf("========================================\n");
        scr_printf("\n");
        scr_printf("                 P S 2\n");
        scr_printf("\n");

        if (blink)
        {
            scr_printf("                 [ ]\n");
        }
        else
        {
            scr_printf("                  \n");
        }

        scr_printf("\n");
        scr_printf("            OPL NEXT\n");
        scr_printf("\n");
        scr_printf("========================================\n");

        blink = !blink;

        /*
         * SleepThread coloca a thread para dormir.
         * Nao depende de DelayThread.
         */
        SleepThread();
    }

    return 0;
}
