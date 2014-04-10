/* 
 *  Only for linux x86_64 and gcc!!!
 *  
 *  yalung929@gmail.com 
 *  2014.3 F1 Bantian Shenzhen China 
 */


#include "network.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <strings.h>

int sock_setnonblock(sock_t s)
{
	int flags;

	flags = fcntl(s, F_GETFL);
	if (flags < 0)
		return flags;
	flags |= O_NONBLOCK;
	if (fcntl(s, F_SETFL, flags) < 0)
		return -1;

	return 0;
}

int sock_unsetnonblock(sock_t s)
{
	int flags;

	flags = fcntl(s, F_GETFL);
	if (flags < 0)
		return flags;
	flags &= ~O_NONBLOCK;
	if (fcntl(s, F_SETFL, flags) < 0)
		return -1;

	return 0;
}

int set_sock_nodelay(sock_t s)
{
	int flag = 1;
	return setsockopt(s, SOL_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
}

int set_sock_rcv_timeout(sock_t s, int sec)
{
	struct timeval timeout;

	timeout.tv_sec = sec;
	timeout.tv_usec = 0;

	return setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
			  sizeof(timeout));
}

int set_sock_snd_timeout(sock_t s, int sec)
{
	struct timeval timeout;

	timeout.tv_sec = sec;
	timeout.tv_usec = 0;

	return setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
			  sizeof(timeout));
}

/* timeout should same as value set by set_sock_rcv_timeout and set_sock_snd_timeout */

int set_sock_keepalive(sock_t s, int timeout)
{
	int val = 1;

	if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) < 0) {
		return -1;
	}
	val = timeout;
	if (setsockopt(s, SOL_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) {
		return -1;
	}
	val = 1;
	if (setsockopt(s, SOL_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {
		return -1;
	}
	val = 3;
	if (setsockopt(s, SOL_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) {
		return -1;
	}
	return 0;
}

static void forward_iov(struct msghdr *msg, size_t len)
{
	while (msg->msg_iov->iov_len <= len) {
		len -= msg->msg_iov->iov_len;
		msg->msg_iov++;
		msg->msg_iovlen--;
	}

	msg->msg_iov->iov_base = (char *)msg->msg_iov->iov_base + len;
	msg->msg_iov->iov_len -= len;
}

int sock_write(sock_t s, struct msghdr *msg, size_t len, uint32_t retries)
{
	int ret;

rewrite:
	ret = sendmsg(s, msg, 0);

	if (ret < 0) {

		if (errno == EINTR)
			goto rewrite;
		/*
		 *  after we set snd&rcv timeout of socket
		 *  it return EAGAIN even for non-blocking fd
		 */
		if (errno == EAGAIN && retries) {
			retries--;
			goto rewrite;
		}

		return 1;
	}

	len -= ret;
	if (len) {
		forward_iov(msg, ret);
		goto rewrite;
	}

	return 0;
}

int sock_read(sock_t s, void *buf, size_t len, uint32_t retries)
{
	int ret;

reread:
	ret = read(s, buf, len);
	if (ret == 0) {
		return 1;
	}
	if (ret < 0) {
		if (errno == EINTR)
			goto reread;
		/*
		 *  after we set snd&rcv timeout of socket
		 *  it return EAGAIN even for non-blocking fd
		 */
		if (errno == EAGAIN && retries) {
			retries--;
			goto reread;
		}

		return 1;
	}

	len -= ret;
	buf = (char *)buf + ret;
	if (len)
		goto reread;

	return 0;
}

sock_t create_listen_sock(port_t port, ip_addr_t ip)
{

	int fd;
	int optval;
	struct sockaddr_in addr;

	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = ip;

	optval = 1;

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
	    -1) {
		return -1;
	}

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		return -1;
	}

	if (listen(fd, 5) < 0) {
		return -1;
	}
	return fd;

}

static inline unsigned long time_diff_sec(struct timespec *t1,
					  struct timespec *t2)
{

	return ((t2->tv_sec * 1000000000 + t2->tv_nsec) -
		(t1->tv_sec * 1000000000 + t1->tv_nsec)) / 1000 / 1000 / 1000;

}

sock_t sock_connect_to(ip_addr_t ip, port_t port, int sec)
{
	int fd;
	fd_set set;
	struct sockaddr_in addr;

	struct timespec start, cur;
	struct timeval select_timeout;

	select_timeout.tv_sec = sec;
	select_timeout.tv_usec = 0;

	clock_gettime(CLOCK_MONOTONIC, &start);

	fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_setnonblock(fd) != 0) {
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = ip;

	for (;;) {

		clock_gettime(CLOCK_MONOTONIC, &cur);

		if (time_diff_sec(&start, &cur) >= (unsigned long)sec) {
			goto close_fd;
		}

		if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
			goto return_fd;
		}

		if (errno == EAGAIN || errno == EINTR)
			continue;
	}

	if (errno != EINPROGRESS)
		goto close_fd;

	FD_ZERO(&set);
	FD_SET(fd, &set);

	if (select(fd + 1, NULL, &set, NULL, &select_timeout) > 0)
		return fd;

close_fd:
	close(fd);
	return -1;

return_fd:
	if (sock_unsetnonblock(fd) != 0) {
		goto close_fd;
	}
	return fd;
}
