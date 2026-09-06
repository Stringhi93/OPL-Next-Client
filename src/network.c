#ifndef NETWORK_H
#define NETWORK_H

int network_init(void);
int network_is_ready(void);
int network_test_server(const char *ip, int port);
void network_shutdown(void);

#endif
