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
 * IRX gerados pelo bin2c no Makefile.
 */
extern unsigned char DEV9_irx[];
extern unsigned int size_DEV9_irx;

extern unsigned char NETMAN_irx[];
extern unsigned int size_NETMAN_irx;

extern unsigned char SMAP_irx[];
extern unsigned int size_SMAP_irx;

static int initialized = 0;
static int link_ready = 0;


/*
 * Inicializa a rede PS2.
 */
int network_init(void)
{
    struct ip4_addr ip;
    struct ip4_addr netmask;
    struct ip4_addr gateway;

    if (initialized)
        return 1;

    scr_printf("========================================\n");
    scr_printf("INICIALIZANDO REDE\n");
    scr_printf("========================================\n");

    /*
     * Permite executar módulos IOP
     * armazenados na memória.
     */
    sbv_patch_enable_lmb();

    /*
     * ----------------------------------------------------
     * DEV9
     * ----------------------------------------------------
     */
    scr_printf("Carregando DEV9...\n");

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

    scr_printf("DEV9: OK\n");

    /*
     * ----------------------------------------------------
     * NETMAN
     * ----------------------------------------------------
     */
    scr_printf("Carregando NETMAN...\n");

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

    scr_printf("NETMAN: OK\n");

    /*
     * ----------------------------------------------------
     * SMAP
     * ----------------------------------------------------
     */
    scr_printf("Carregando SMAP...\n");

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

    scr_printf("SMAP: OK\n");

    /*
     * ----------------------------------------------------
     * NETMAN INIT
     * ----------------------------------------------------
     */
    scr_printf("Inicializando NETMAN...\n");

    if (NetManInit() < 0)
    {
        scr_printf("NETMAN INIT: ERRO\n");
        return 0;
    }

    scr_printf("NETMAN INIT: OK\n");

    /*
     * ----------------------------------------------------
     * CONFIGURACAO DE REDE
     * ----------------------------------------------------
     *
     * PC:
     *     192.168.1.8
     *
     * PS2:
     *     192.168.1.20
     *
     * Mascara:
     *     255.255.255.0
     *
     * Gateway:
     *     192.168.1.1
     */
    IP4_ADDR(&ip, 192, 168, 1, 20);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gateway, 192, 168, 1, 1);

    /*
     * ----------------------------------------------------
     * PS2IP
     * ----------------------------------------------------
     */
    scr_printf("Inicializando PS2IP...\n");

    if (ps2ipInit(
            &ip,
            &netmask,
            &gateway) < 0)
    {
        scr_printf("PS2IP: ERRO\n");

        NetManDeinit();

        return 0;
    }

    scr_printf("PS2IP: OK\n");

    initialized = 1;

    /*
     * ----------------------------------------------------
     * AUTO NEGOCIACAO ETHERNET
     * ----------------------------------------------------
     */
    NetManSetLinkMode(
        NETMAN_NETIF_ETH_LINK_MODE_AUTO);

    /*
     * ----------------------------------------------------
     * VERIFICA LINK
     * ----------------------------------------------------
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

    scr_printf("\n");
    scr_printf("========================================\n");
    scr_printf("CONFIGURACAO DE REDE\n");
    scr_printf("========================================\n");

    scr_printf("IP PS2 : 192.168.1.20\n");
    scr_printf("PC     : 192.168.1.8\n");
    scr_printf("MASK   : 255.255.255.0\n");
    scr_printf("GATEWAY: 192.168.1.1\n");

    if (link_ready)
    {
        scr_printf("LINK   : CONECTADO\n");
    }
    else
    {
        scr_printf("LINK   : DESCONECTADO\n");
    }

    scr_printf("========================================\n");

    return 1;
}


/*
 * Verifica se o cabo/link Ethernet está ativo.
 */
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


/*
 * Testa conexão TCP com um servidor.
 *
 * Retorno:
 *
 *   0  = conexão realizada
 *  -1  = rede não inicializada
 *  -2  = link Ethernet não está ativo
 *  -3  = erro ao criar socket
 *  -4  = erro ao conectar
 */
int network_test_server(const char *ip, int port)
{
    int sock;
    int result;

    struct sockaddr_in server;

    if (!initialized)
        return -1;

    if (!network_is_ready())
        return -2;

    /*
     * Cria socket TCP.
     */
    sock = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (sock < 0)
        return -3;

    /*
     * Limpa a estrutura.
     */
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    /*
     * Tenta conectar.
     */
    result = connect(
        sock,
        (struct sockaddr *)&server,
        sizeof(server));

    /*
     * Fecha socket.
     */
    close(sock);

    if (result < 0)
        return -4;

    return 0;
}


/*
 * Desliga a rede.
 */
void network_shutdown(void)
{
    if (!initialized)
        return;

    ps2ipDeinit();
    NetManDeinit();

    initialized = 0;
    link_ready = 0;
}
