#include "network.h"

#include <stdio.h>
#include <string.h>

#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <sbv_patches.h>
#include <debug.h>

#include <netman.h>
#include <ps2ip.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

extern unsigned char DEV9_irx[];
extern unsigned int size_DEV9_irx;

extern unsigned char NETMAN_irx[];
extern unsigned int size_NETMAN_irx;

extern unsigned char SMAP_irx[];
extern unsigned int size_SMAP_irx;

static int initialized = 0;

static network_config_t config =
{
    {192, 168, 1, 20},
    {255, 255, 255, 0},
    {192, 168, 1, 1},
    {192, 168, 1, 1},

    {192, 168, 1, 8},

    445,
    80,
    21
};

static void copy_ip(
    struct ip4_addr *dst,
    const int src[4]
)
{
    IP4_ADDR(
        dst,
        src[0],
        src[1],
        src[2],
        src[3]
    );
}

static int load_network_modules(void)
{
    int ret;

    scr_printf("NET: carregando DEV9...\n");

    ret = SifExecModuleBuffer(
        DEV9_irx,
        size_DEV9_irx,
        0,
        NULL,
        NULL
    );

    if (ret < 0)
    {
        scr_printf("NET: DEV9 ERRO %d\n", ret);
        return 0;
    }

    scr_printf("NET: carregando NETMAN...\n");

    ret = SifExecModuleBuffer(
        NETMAN_irx,
        size_NETMAN_irx,
        0,
        NULL,
        NULL
    );

    if (ret < 0)
    {
        scr_printf("NET: NETMAN ERRO %d\n", ret);
        return 0;
    }

    scr_printf("NET: carregando SMAP...\n");

    ret = SifExecModuleBuffer(
        SMAP_irx,
        size_SMAP_irx,
        0,
        NULL,
        NULL
    );

    if (ret < 0)
    {
        scr_printf("NET: SMAP ERRO %d\n", ret);
        return 0;
    }

    return 1;
}

int network_init(void)
{
    struct ip4_addr ip;
    struct ip4_addr netmask;
    struct ip4_addr gateway;
    struct ip4_addr dns;

    if (initialized)
        return 1;

    scr_printf("========================================\n");
    scr_printf("INICIALIZANDO REDE\n");
    scr_printf("========================================\n");

    /*
     * Permite execucao de IRX a partir da memoria EE.
     */
    sbv_patch_enable_lmb();

    if (!load_network_modules())
        return 0;

    if (NetManInit() < 0)
    {
        scr_printf("NET: NetManInit ERRO\n");
        return 0;
    }

    copy_ip(&ip, config.ip);
    copy_ip(&netmask, config.netmask);
    copy_ip(&gateway, config.gateway);
    copy_ip(&dns, config.dns);

    /*
     * TCP/IP com configuracao estatica.
     */
    if (ps2ipInit(
            &ip,
            &netmask,
            &gateway
        ) < 0)
    {
        scr_printf("NET: ps2ipInit ERRO\n");
        NetManDeinit();
        return 0;
    }

    dns_setserver(0, &dns);

    /*
     * Auto-negociacao Ethernet.
     */
    if (NetManSetLinkMode(
            NETMAN_NETIF_ETH_LINK_MODE_AUTO
        ) != 0)
    {
        scr_printf("NET: aviso - link mode\n");
    }

    initialized = 1;

    scr_printf(
        "NET: IP %d.%d.%d.%d\n",
        config.ip[0],
        config.ip[1],
        config.ip[2],
        config.ip[3]
    );

    return 1;
}

int network_is_ready(void)
{
    if (!initialized)
        return 0;

    return (
        NetManIoctl(
            NETMAN_NETIF_IOCTL_GET_LINK_STATUS,
            NULL,
            0,
            NULL,
            0
        ) == NETMAN_NETIF_ETH_LINK_STATE_UP
    );
}

void network_get_config(network_config_t *out)
{
    if (out == NULL)
        return;

    memcpy(
        out,
        &config,
        sizeof(network_config_t)
    );
}

int network_set_config(const network_config_t *new_config)
{
    struct ip4_addr ip;
    struct ip4_addr netmask;
    struct ip4_addr gateway;
    struct ip4_addr dns;

    if (new_config == NULL)
        return -1;

    memcpy(
        &config,
        new_config,
        sizeof(network_config_t)
    );

    if (!initialized)
        return 0;

    copy_ip(&ip, config.ip);
    copy_ip(&netmask, config.netmask);
    copy_ip(&gateway, config.gateway);
    copy_ip(&dns, config.dns);

    if (ps2ip_getconfig("sm0", NULL) < 0)
    {
        /*
         * Alguns builds nao aceitam NULL.
         * O teste real sera feito abaixo pelo setconfig.
         */
    }

    /*
     * Reaplica a configuracao.
     */
    {
        t_ip_info info;

        if (ps2ip_getconfig("sm0", &info) < 0)
            return -2;

        ip_addr_set(
            (struct ip4_addr *)&info.ipaddr,
            &ip
        );

        ip_addr_set(
            (struct ip4_addr *)&info.netmask,
            &netmask
        );

        ip_addr_set(
            (struct ip4_addr *)&info.gw,
            &gateway
        );

        info.dhcp_enabled = 0;

        if (ps2ip_setconfig(&info) < 0)
            return -3;
    }

    dns_setserver(0, &dns);

    return 0;
}

int network_test_server(
    const char *ip,
    int port
)
{
    int sock;
    int result;

    struct sockaddr_in server;

    if (!initialized)
        return -1;

    if (!network_is_ready())
        return -2;

    memset(
        &server,
        0,
        sizeof(server)
    );

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

int network_test_smb(void)
{
    char ip[32];

    sprintf(
        ip,
        "%d.%d.%d.%d",
        config.server[0],
        config.server[1],
        config.server[2],
        config.server[3]
    );

    return network_test_server(
        ip,
        config.smb_port
    );
}

int network_test_http(void)
{
    char ip[32];

    sprintf(
        ip,
        "%d.%d.%d.%d",
        config.server[0],
        config.server[1],
        config.server[2],
        config.server[3]
    );

    return network_test_server(
        ip,
        config.http_port
    );
}

int network_test_ftp(void)
{
    char ip[32];

    sprintf(
        ip,
        "%d.%d.%d.%d",
        config.server[0],
        config.server[1],
        config.server[2],
        config.server[3]
    );

    return network_test_server(
        ip,
        config.ftp_port
    );
}

void network_shutdown(void)
{
    if (!initialized)
        return;

    ps2ipDeinit();
    NetManDeinit();

    initialized = 0;
}
