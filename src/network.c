#include "network.h"

#include <stdio.h>

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
 * Drivers IRX gerados pelo bin2c no Makefile.
 */
extern unsigned char DEV9_irx[];
extern unsigned int size_DEV9_irx;

extern unsigned char NETMAN_irx[];
extern unsigned int size_NETMAN_irx;

extern unsigned char SMAP_irx[];
extern unsigned int size_SMAP_irx;

static int initialized = 0;
static int link_ready = 0;

int network_init(void)
{
    struct ip4_addr ip;
    struct ip4_addr netmask;
    struct ip4_addr gateway;

    if (initialized)
        return 1;

    scr_printf("Inicializando rede...\n");

    /*
     * Permite carregar os IRX a partir da memoria.
     */
    sbv_patch_enable_lmb();

    /*
     * Carrega os drivers.
     */
    if (SifExecModuleBuffer(
            DEV9_irx,
            size_DEV9_irx,
            0,
            NULL,
            NULL) < 0)
    {
        scr_printf("DEV9: ERRO\n");
        return 0;
    }

    if (SifExecModuleBuffer(
            NETMAN_irx,
            size_NETMAN_irx,
            0,
            NULL,
            NULL) < 0)
    {
        scr_printf("NETMAN: ERRO\n");
        return 0;
    }

    if (SifExecModuleBuffer(
            SMAP_irx,
            size_SMAP_irx,
            0,
            NULL,
            NULL) < 0)
    {
        scr_printf("SMAP: ERRO\n");
        return 0;
    }

    /*
     * Inicializa NETMAN.
     */
    if (NetManInit() < 0)
    {
        scr_printf("NETMAN INIT: ERRO\n");
        return 0;
    }

    /*
     * IP fixo para o primeiro teste.
     *
     * PC:  192.168.1.8
     * PS2: 192.168.1.20
     */
    IP4_ADDR(&ip, 192, 168, 1, 20);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gateway, 192, 168, 1, 1);

    /*
     * Inicializa TCP/IP.
     */
    if (ps2ipInit(&ip, &netmask, &gateway) < 0)
    {
        scr_printf("PS2IP: ERRO\n");
        NetManDeinit();
        return 0;
    }

    initialized = 1;

    /*
     * Coloca Ethernet em auto-negociacao.
     */
    NetManSetLinkMode(NETMAN_NETIF_ETH_LINK_MODE_AUTO);

    /*
     * Verifica o link.
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
    NetManDeinit();

    initialized = 0;
    link_ready = 0;
}
