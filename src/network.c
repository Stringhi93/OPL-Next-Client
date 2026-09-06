#include "network.h"

#include <stdio.h>
#include <string.h>

#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <sbv_patches.h>

#include <netman.h>
#include <ps2ip.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/*
 * Drivers de rede embutidos no ELF.
 */
extern unsigned char ps2dev9_irx[];
extern unsigned int size_ps2dev9_irx;

extern unsigned char netman_irx[];
extern unsigned int size_netman_irx;

extern unsigned char smap_irx[];
extern unsigned int size_smap_irx;

static int initialized = 0;
static int link_ready = 0;

static int load_module(
    const char *name,
    void *data,
    unsigned int size
)
{
    int result;
    int mod_result = 0;

    result = SifExecModuleBuffer(
        data,
        size,
        0,
        NULL,
        &mod_result
    );

    if (result < 0)
    {
        scr_printf("%s: ERRO %d\n", name, result);
        return -1;
    }

    if (mod_result < 0)
    {
        scr_printf("%s: START ERRO %d\n", name, mod_result);
        return -1;
    }

    scr_printf("%s: OK\n", name);

    return 0;
}

int network_init(void)
{
    struct ip4_addr ip;
    struct ip4_addr netmask;
    struct ip4_addr gateway;

    if (initialized)
        return 1;

    scr_printf("Inicializando rede...\n");

    /*
     * Permite carregar os IRX a partir do ELF.
     */
    sbv_patch_enable_lmb();

    /*
     * DEV9 primeiro.
     */
    if (load_module(
            "DEV9",
            ps2dev9_irx,
            size_ps2dev9_irx) < 0)
    {
        return 0;
    }

    /*
     * NETMAN depende do DEV9.
     */
    if (load_module(
            "NETMAN",
            netman_irx,
            size_netman_irx) < 0)
    {
        return 0;
    }

    /*
     * SMAP depende do DEV9/NETMAN.
     */
    if (load_module(
            "SMAP",
            smap_irx,
            size_smap_irx) < 0)
    {
        return 0;
    }

    /*
     * Configuração inicial.
     *
     * Usaremos IP fixo apenas nesta primeira versão.
     * Depois podemos trocar para DHCP.
     */
    IP4_ADDR(&ip, 192, 168, 1, 20);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gateway, 192, 168, 1, 1);

    /*
     * Inicializa a pilha TCP/IP.
     */
    if (ps2ipInit(&ip, &netmask, &gateway) < 0)
    {
        scr_printf("PS2IP: ERRO\n");
        return 0;
    }

    initialized = 1;

    /*
     * Verifica o link Ethernet.
     */
    if (NetManIoctl(
            NETMAN_NETIF_IOCTL_GET_LINK_STATUS,
            NULL,
            0,
            NULL,
            0) == NETMAN_NETIF_ETH_LINK_STATE_UP)
    {
        link_ready = 1;
    }
    else
    {
        link_ready = 0;
    }

    scr_printf("IP PS2: 192.168.1.20\n");

    if (link_ready)
        scr_printf("LINK: CONECTADO\n");
    else
        scr_printf("LINK: DESCONECTADO\n");

    return 1;
}

int network_is_ready(void)
{
    if (!initialized)
        return 0;

    if (NetManIoctl(
            NETMAN_NETIF_IOCTL_GET_LINK_STATUS,
            NULL,
            0,
            NULL,
            0) == NETMAN_NETIF_ETH_LINK_STATE_UP)
    {
        link_ready = 1;
    }
    else
    {
        link_ready = 0;
    }

    return link_ready;
}

int network_test_server(const char *ip, int port)
{
    int sock;
    int result;

    struct sockaddr_in server;

    if (!initialized)
        return -1;

    if (!network_is_ready())
        return -2;

    sock = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (sock < 0)
        return -3;

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    result = connect(
        sock,
        (struct sockaddr *)&server,
        sizeof(server)
    );

    close(sock);

    if (result < 0)
        return -4;

    return 0;
}

void network_shutdown(void)
{
    if (!initialized)
        return;

    ps2ipDeinit();

    initialized = 0;
    link_ready = 0;
}

Importante: nesta primeira versão o PS2 usará 192.168.1.20. Não configure outro equipamento da sua rede com esse IP.
4. Substitua src/main.c inteiro

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
    network_init();

    /*
     * Inicializa interface.
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
    }

    network_shutdown();
    input_shutdown();

    return 0;
}
