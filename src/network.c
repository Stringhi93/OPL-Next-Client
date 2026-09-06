#include "network.h"

#include <stdio.h>
#include <string.h>

#include <kernel.h>
#include <debug.h>
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
 * Drivers IRX incorporados pelo bin2c.
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

    int result;

    if (initialized)
        return 1;

    scr_printf("========================================\n");
    scr_printf("INICIALIZANDO REDE\n");
    scr_printf("========================================\n");

    /*
     * Permite executar os IRX diretamente da memoria.
     */
    sbv_patch_enable_lmb();

    /*
     * DEV9
     */
    scr_printf("Carregando DEV9...\n");

    result = SifExecModuleBuffer(
        DEV9_irx,
        size_DEV9_irx,
        0,
        NULL,
        NULL
    );

    if (result < 0)
    {
        scr_printf("ERRO: DEV9\n");
        return 0;
    }

    scr_printf("DEV9 OK\n");

    /*
     * NETMAN
     */
    scr_printf("Carregando NETMAN...\n");

    result = SifExecModuleBuffer(
        NETMAN_irx,
        size_NETMAN_irx,
        0,
        NULL,
        NULL
    );

    if (result < 0)
    {
        scr_printf("ERRO: NETMAN\n");
        return 0;
    }

    scr_printf("NETMAN OK\n");

    /*
     * SMAP
     */
    scr_printf("Carregando SMAP...\n");

    result = SifExecModuleBuffer(
        SMAP_irx,
        size_SMAP_irx,
        0,
        NULL,
        NULL
    );

    if (result < 0)
    {
        scr_printf("ERRO: SMAP\n");
        return 0;
    }

    scr_printf("SMAP OK\n");

    /*
     * Inicializa NETMAN.
     */
    scr_printf("Inicializando NETMAN...\n");

    result = NetManInit();

    if (result < 0)
    {
        scr_printf("ERRO: NetManInit\n");
        return 0;
    }

    scr_printf("NETMAN INIT OK\n");

    /*
     * Configuracao de rede.
     *
     * PS2: 192.168.1.20
     * PC:  192.168.1.8
     * GW:  192.168.1.1
     */
    IP4_ADDR(&ip, 192, 168, 1, 20);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gateway, 192, 168, 1, 1);

    /*
     * Inicializa TCP/IP.
     */
    scr_printf("Inicializando TCP/IP...\n");

    result = ps2ipInit(
        &ip,
        &netmask,
        &gateway
    );

    if (result < 0)
    {
        scr_printf("ERRO: ps2ipInit\n");

        NetManDeinit();

        return 0;
    }

    scr_printf("TCP/IP OK\n");

    /*
     * Auto-negociacao Ethernet.
     */
    result = NetManSetLinkMode(
        NETMAN_NETIF_ETH_LINK_MODE_AUTO
    );

    if (result != 0)
    {
        scr_printf("AVISO: falha ao configurar link\n");
    }

    /*
     * Inicializacao concluida.
     */
    initialized = 1;

    /*
     * Verifica o link.
     */
    if (NetManIoctl(
            NETMAN_NETIF_IOCTL_GET_LINK_STATUS,
            NULL,
            0,
            NULL,
            0
        ) == NETMAN_NETIF_ETH_LINK_STATE_UP)
    {
        link_ready = 1;
    }
    else
    {
        link_ready = 0;
    }

    scr_printf("----------------------------------------\n");
    scr_printf("IP PS2: 192.168.1.20\n");

    if (link_ready)
        scr_printf("LINK: CONECTADO\n");
    else
        scr_printf("LINK: DESCONECTADO\n");

    scr_printf("========================================\n");

    return 1;
}

int network_is_ready(void)
{
    int status;

    if (!initialized)
        return 0;

    status = NetManIoctl(
        NETMAN_NETIF_IOCTL_GET_LINK_STATUS,
        NULL,
        0,
        NULL,
        0
    );

    if (status == NETMAN_NETIF_ETH_LINK_STATE_UP)
        link_ready = 1;
    else
        link_ready = 0;

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

    if (ip == NULL)
        return -3;

    /*
     * Cria socket TCP.
     */
    sock = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (sock < 0)
    {
        scr_printf("ERRO: socket()\n");
        return -3;
    }

    /*
     * Limpa estrutura.
     */
    memset(
        &server,
        0,
        sizeof(server)
    );

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    scr_printf(
        "Testando servidor %s:%d...\n",
        ip,
        port
    );

    /*
     * Conecta.
     */
    result = connect(
        sock,
        (struct sockaddr *)&server,
        sizeof(server)
    );

    /*
     * Fecha socket.
     */
    close(sock);

    if (result < 0)
    {
        scr_printf("CONEXAO: FALHOU\n");
        return -4;
    }

    scr_printf("CONEXAO: OK\n");

    return 0;
}

void network_shutdown(void)
{
    if (!initialized)
        return;

    scr_printf("Desligando rede...\n");

    ps2ipDeinit();

    NetManDeinit();

    initialized = 0;
    link_ready = 0;

    scr_printf("Rede desligada.\n");
}
