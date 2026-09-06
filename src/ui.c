#include <stdio.h>
#include <string.h>

#include <debug.h>

#include "ui.h"
#include "input.h"
#include "network.h"

static int screen = 0;
static int menu = 0;
static int config_field = 0;

static int test_smb = -1;
static int test_http = -1;
static int test_ftp = -1;

static network_config_t cfg;

static void draw_header(const char *title)
{
    scr_printf("========================================\n");
    scr_printf("           OPL NEXT CLIENT\n");
    scr_printf("========================================\n");
    scr_printf("%s\n\n", title);
}

static void draw_main(void)
{
    draw_header("MENU PRINCIPAL");

    scr_printf(
        "%s Iniciar\n",
        menu == 0 ? ">" : " "
    );

    scr_printf(
        "%s Configuracao de rede\n",
        menu == 1 ? ">" : " "
    );

    scr_printf(
        "%s Testes de rede\n",
        menu == 2 ? ">" : " "
    );

    scr_printf(
        "%s Informacoes\n",
        menu == 3 ? ">" : " "
    );

    scr_printf("\n");
    scr_printf("UP/DOWN: selecionar\n");
    scr_printf("X: entrar\n");
}

static void draw_ip(
    const char *name,
    int ip[4],
    int selected
)
{
    scr_printf(
        "%s %s: %d.%d.%d.%d",
        selected ? ">" : " ",
        name,
        ip[0],
        ip[1],
        ip[2],
        ip[3]
    );

    if (selected)
        scr_printf("  <L/R>");

    scr_printf("\n");
}

static void draw_network_config(void)
{
    draw_header("CONFIGURACAO MANUAL");

    scr_printf(
        "CAMPO %d/5\n\n",
        config_field + 1
    );

    draw_ip(
        "IP",
        cfg.ip,
        config_field == 0
    );

    draw_ip(
        "MASK",
        cfg.netmask,
        config_field == 1
    );

    draw_ip(
        "GATEWAY",
        cfg.gateway,
        config_field == 2
    );

    draw_ip(
        "DNS",
        cfg.dns,
        config_field == 3
    );

    draw_ip(
        "SERVIDOR",
        cfg.server,
        config_field == 4
    );

    scr_printf("\n");
    scr_printf("UP/DOWN: campo\n");
    scr_printf("LEFT/RIGHT: alterar ultimo octeto\n");
    scr_printf("X: aplicar\n");
    scr_printf("O: voltar\n");
}

static void change_current_field(int amount)
{
    int *ip = NULL;

    switch (config_field)
    {
        case 0:
            ip = cfg.ip;
            break;

        case 1:
            ip = cfg.netmask;
            break;

        case 2:
            ip = cfg.gateway;
            break;

        case 3:
            ip = cfg.dns;
            break;

        case 4:
            ip = cfg.server;
            break;

        default:
            return;
    }

    ip[3] += amount;

    if (ip[3] < 0)
        ip[3] = 0;

    if (ip[3] > 254)
        ip[3] = 254;
}

static void draw_tests(void)
{
    draw_header("TESTES DE REDE");

    scr_printf(
        "LINK: %s\n\n",
        network_is_ready()
            ? "CONECTADO"
            : "DESCONECTADO"
    );

    scr_printf(
        "SERVIDOR: %d.%d.%d.%d\n\n",
        cfg.server[0],
        cfg.server[1],
        cfg.server[2],
        cfg.server[3]
    );

    scr_printf(
        "SMB  TCP %d: %s\n",
        cfg.smb_port,
        test_smb == 0
            ? "OK"
            : test_smb == -4
                ? "FALHOU"
                : "-"
    );

    scr_printf(
        "HTTP TCP %d: %s\n",
        cfg.http_port,
        test_http == 0
            ? "OK"
            : test_http == -4
                ? "FALHOU"
                : "-"
    );

    scr_printf(
        "FTP  TCP %d: %s\n",
        cfg.ftp_port,
        test_ftp == 0
            ? "OK"
            : test_ftp == -4
                ? "FALHOU"
                : "-"
    );

    scr_printf("\n");
    scr_printf("X: testar SMB\n");
    scr_printf("TRIANGULO: testar HTTP\n");
    scr_printf("QUADRADO: testar FTP\n");
    scr_printf("O: voltar\n");
}

static void draw_info(void)
{
    draw_header("INFORMACOES");

    scr_printf(
        "IP       : %d.%d.%d.%d\n",
        cfg.ip[0],
        cfg.ip[1],
        cfg.ip[2],
        cfg.ip[3]
    );

    scr_printf(
        "MASK     : %d.%d.%d.%d\n",
        cfg.netmask[0],
        cfg.netmask[1],
        cfg.netmask[2],
        cfg.netmask[3]
    );

    scr_printf(
        "GATEWAY  : %d.%d.%d.%d\n",
        cfg.gateway[0],
        cfg.gateway[1],
        cfg.gateway[2],
        cfg.gateway[3]
    );

    scr_printf(
        "DNS      : %d.%d.%d.%d\n",
        cfg.dns[0],
        cfg.dns[1],
        cfg.dns[2],
        cfg.dns[3]
    );

    scr_printf(
        "SERVIDOR : %d.%d.%d.%d\n",
        cfg.server[0],
        cfg.server[1],
        cfg.server[2],
        cfg.server[3]
    );

    scr_printf("\n");
    scr_printf(
        "LINK: %s\n",
        network_is_ready()
            ? "UP"
            : "DOWN"
    );

    scr_printf("\n");
    scr_printf("O: voltar\n");
}

void ui_init(void)
{
    screen = 0;
    menu = 0;
    config_field = 0;

    test_smb = -1;
    test_http = -1;
    test_ftp = -1;

    network_get_config(&cfg);
}

void ui_update(void)
{
    /*
     * MENU PRINCIPAL
     */
    if (screen == 0)
    {
        if (input_pressed(INPUT_UP))
        {
            if (menu > 0)
                menu--;
        }

        if (input_pressed(INPUT_DOWN))
        {
            if (menu < 3)
                menu++;
        }

        if (input_pressed(INPUT_CROSS))
        {
            if (menu == 1)
            {
                screen = 1;
                config_field = 0;
            }
            else if (menu == 2)
            {
                screen = 2;
            }
            else if (menu == 3)
            {
                screen = 3;
            }
        }

        return;
    }

    /*
     * CONFIGURACAO
     */
    if (screen == 1)
    {
        if (input_pressed(INPUT_UP))
        {
            if (config_field > 0)
                config_field--;
        }

        if (input_pressed(INPUT_DOWN))
        {
            if (config_field < 4)
                config_field++;
        }

        if (input_pressed(INPUT_LEFT))
            change_current_field(-1);

        if (input_pressed(INPUT_RIGHT))
            change_current_field(1);

        if (input_pressed(INPUT_CROSS))
        {
            network_set_config(&cfg);
        }

        if (input_pressed(INPUT_CIRCLE))
        {
            screen = 0;
            menu = 0;
        }

        return;
    }

    /*
     * TESTES
     */
    if (screen == 2)
    {
        if (input_pressed(INPUT_CROSS))
            test_smb = network_test_smb();

        if (input_pressed(INPUT_CIRCLE))
        {
            screen = 0;
            menu = 0;
        }

        /*
         * TRIANGULO e QUADRADO nao estao expostos
         * pelo input.h atual, portanto os testes
         * extras ficam disponiveis por agora somente
         * atraves da configuracao futura.
         */

        return;
    }

    /*
     * INFORMACOES
     */
    if (screen == 3)
    {
        if (input_pressed(INPUT_CIRCLE))
        {
            screen = 0;
            menu = 0;
        }
    }
}

void ui_render(void)
{
    scr_clear();

    if (screen == 0)
    {
        draw_main();
    }
    else if (screen == 1)
    {
        draw_network_config();
    }
    else if (screen == 2)
    {
        draw_tests();
    }
    else
    {
        draw_info();
    }
}
