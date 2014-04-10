/* 
 *  Only for linux x86_64 and gcc!!!
 *  
 *  yalung929@gmail.com 
 *  2014.3 F1 Bantian Shenzhen China 
 */


#ifndef NETHHHH
#define NETHHHH

#include "common.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef int sock_t;

#ifndef INVALID_SOCK
#define INVALID_SOCK -1
#endif

int sock_setnonblock(sock_t s);

int sock_unsetnonblock(sock_t s);

int set_sock_nodelay(sock_t s);

int set_sock_rcv_timeout(sock_t s, int sec);

int set_sock_snd_timeout(sock_t s, int sec);

int set_sock_keepalive(sock_t s, int timeout);

int sock_read(sock_t s, void *buf, size_t len, uint32_t retries);

int sock_write(sock_t s, struct msghdr *msg, size_t len, uint32_t retries);

sock_t sock_connect_to(ip_addr_t ip, port_t port, int sec);

sock_t create_listen_sock(port_t port, ip_addr_t ip);

#endif
