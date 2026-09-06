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