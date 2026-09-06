#include "ui.h"
#include "input.h"

#include <stdio.h>
#include <string.h>
#include <debug.h>

#define MENU_COUNT 4

static int selected = 0;

static const char *menu_items[MENU_COUNT] =
{
    "JOGOS",
    "REDE / SMB",
    "CONFIGURACOES",
    "SOBRE"
};

static void ui_clear(void)
{
    scr_clear();
}

static void ui_header(void)
{
    scr_printf("\n");
    scr_printf("============================================================\n");
    scr_printf("                    OPL NEXT CLIENT                         \n");
    scr_printf("                         V0.1                               \n");
    scr_printf("============================================================\n");
}

static void ui_footer(void)
{
    scr_printf("\n");
    scr_printf("------------------------------------------------------------\n");
    scr_printf(" D-PAD  Navegar       X  Selecionar       START  Opcoes\n");
    scr_printf(" O      Voltar        SELECT  Sistema\n");
    scr_printf("------------------------------------------------------------\n");
}

static void ui_draw_menu(void)
{
    int i;

    scr_printf("\n");

    for (i = 0; i < MENU_COUNT; i++)
    {
        if (i == selected)
        {
            scr_printf("       >>>  [ %s ]\n", menu_items[i]);
        }
        else
        {
            scr_printf("            %s\n", menu_items[i]);
        }

        scr_printf("\n");
    }
}

static void ui_draw_status(void)
{
    scr_printf("\n");
    scr_printf(" STATUS\n");
    scr_printf(" ----------------------------------------------------------\n");
    scr_printf(" Video       : OK\n");
    scr_printf(" Controller  : OK\n");
    scr_printf(" Graphics    : DEBUG UI\n");
    scr_printf(" Network     : NAO INICIALIZADA\n");
    scr_printf(" SMB         : NAO INICIALIZADO\n");
    scr_printf(" ----------------------------------------------------------\n");
}

static void ui_draw_about(void)
{
    ui_clear();

    scr_printf("\n");
    scr_printf("============================================================\n");
    scr_printf("                        SOBRE                               \n");
    scr_printf("============================================================\n\n");

    scr_printf(" OPL NEXT CLIENT\n\n");

    scr_printf(" Projeto independente de launcher para PlayStation 2.\n\n");

    scr_printf(" V0.1\n");
    scr_printf(" Interface inicial\n");
    scr_printf(" Sistema de controle\n");
    scr_printf(" Estrutura preparada para rede/SMB\n\n");

    scr_printf(" Esta versao ainda NAO carrega jogos.\n\n");

    scr_printf("------------------------------------------------------------\n");
    scr_printf(" O = Voltar\n");
    scr_printf("------------------------------------------------------------\n");
}

static void ui_draw_network(void)
{
    ui_clear();

    scr_printf("\n");
    scr_printf("============================================================\n");
    scr_printf("                       REDE / SMB                           \n");
    scr_printf("============================================================\n\n");

    scr_printf(" Ethernet       : PREPARADA PARA V0.2\n");
    scr_printf(" TCP/IP         : PREPARADO PARA V0.2\n");
    scr_printf(" SMB            : PREPARADO PARA V0.2\n\n");

    scr_printf(" Proximo passo:\n\n");

    scr_printf(" 1. Inicializar Ethernet\n");
    scr_printf(" 2. Obter endereco IP\n");
    scr_printf(" 3. Testar servidor\n");
    scr_printf(" 4. Conectar compartilhamento SMB\n");
    scr_printf(" 5. Listar jogos\n\n");

    scr_printf(" Nenhuma conexao de rede foi realizada nesta V0.1.\n\n");

    scr_printf("------------------------------------------------------------\n");
    scr_printf(" O = Voltar\n");
    scr_printf("------------------------------------------------------------\n");
}

static void ui_draw_games(void)
{
    ui_clear();

    scr_printf("\n");
    scr_printf("============================================================\n");
    scr_printf("                         JOGOS                              \n");
    scr_printf("============================================================\n\n");

    scr_printf(" Nenhum jogo encontrado.\n\n");

    scr_printf(" Fontes futuras:\n\n");
    scr_printf("   [ ] SMB\n");
    scr_printf("   [ ] USB\n");
    scr_printf("   [ ] HDD\n\n");

    scr_printf(" A biblioteca de jogos sera adicionada depois do\n");
    scr_printf(" sistema de rede e armazenamento.\n\n");

    scr_printf("------------------------------------------------------------\n");
    scr_printf(" O = Voltar\n");
    scr_printf("------------------------------------------------------------\n");
}

static void ui_draw_settings(void)
{
    ui_clear();

    scr_printf("\n");
    scr_printf("============================================================\n");
    scr_printf("                     CONFIGURACOES                          \n");
    scr_printf("============================================================\n\n");

    scr_printf(" Video\n");
    scr_printf("   Modo             : AUTO\n\n");

    scr_printf(" Interface\n");
    scr_printf("   Animacoes        : ON\n");
    scr_printf("   Som              : OFF\n\n");

    scr_printf(" Sistema\n");
    scr_printf("   Rede             : FUTURA\n");
    scr_printf("   SMB              : FUTURO\n\n");

    scr_printf("------------------------------------------------------------\n");
    scr_printf(" O = Voltar\n");
    scr_printf("------------------------------------------------------------\n");
}

void ui_init(void)
{
    init_scr();

    scr_clear();

    selected = 0;
}

void ui_shutdown(void)
{
    /*
     * Nothing to release yet.
     * Graphics resources will be managed here
     * when the full gsKit renderer is introduced.
     */
}

void ui_update(void)
{
    const InputState *input;

    input_update();

    input = input_get();

    /*
     * Main menu navigation.
     */
    if (input->up)
    {
        selected--;

        if (selected < 0)
            selected = MENU_COUNT - 1;
    }

    if (input->down)
    {
        selected++;

        if (selected >= MENU_COUNT)
            selected = 0;
    }

    /*
     * Selection.
     */
    if (input->cross)
    {
        switch (selected)
        {
            case 0:
                ui_draw_games();
                while (!input_get()->circle)
                {
                    input_update();
                }
                break;

            case 1:
                ui_draw_network();
                while (!input_get()->circle)
                {
                    input_update();
                }
                break;

            case 2:
                ui_draw_settings();
                while (!input_get()->circle)
                {
                    input_update();
                }
                break;

            case 3:
                ui_draw_about();
                while (!input_get()->circle)
                {
                    input_update();
                }
                break;
        }
    }
}

void ui_render(void)
{
    ui_clear();

    ui_header();

    ui_draw_menu();

    ui_draw_status();

    ui_footer();
}
