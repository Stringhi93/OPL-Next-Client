#ifndef NETWORK_H
#define NETWORK_H

typedef struct
{
    int ip[4];
    int netmask[4];
    int gateway[4];
    int dns[4];

    int server[4];

    int smb_port;
    int http_port;
    int ftp_port;
} network_config_t;

int network_init(void);
void network_shutdown(void);

int network_is_ready(void);

void network_get_config(network_config_t *config);
int network_set_config(const network_config_t *config);

int network_test_server(const char *ip, int port);

int network_test_smb(void);
int network_test_http(void);
int network_test_ftp(void);

#endif
