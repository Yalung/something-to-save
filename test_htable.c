#include "htable.h"
#include "common.h"
#include <stdio.h>
#include <assert.h>

#define SIZE 10

typedef struct tcpm_host {
	ip_addr_t ip;
	list_node_t list;

} tcpm_host_t;

uint32_t tcpm_host_hash(htable_key_t key)
{
	return htable_default_hash(key, sizeof(ip_addr_t));
}

int tcpm_host_cmp(htable_key_t key, list_node_t * node)
{
	ip_addr_t ip = *(ip_addr_t *) key;
	tcpm_host_t *host = list_entry(node, tcpm_host_t, list);

	return ip == host->ip;
}

int main()
{
	uint32_t i;
	ip_addr_t ip = 3;
	tcpm_host_t host, *hostp;
	list_node_t *node;

	htable_t *t = htable_create(SIZE, tcpm_host_cmp, tcpm_host_hash);

	htable_del(t, &ip);
	assert(!htable_find(t, &ip));

	INIT_LIST_NODE(&host.list);
	host.ip = ip;
	htable_add(t, &ip, &host.list);
	assert(htable_find(t, &ip));

	htable_del(t, &ip);
	assert(!htable_find(t, &ip));

	for (i = 0; i < SIZE / 2; i++) {
		hostp = (tcpm_host_t *) malloc(sizeof(tcpm_host_t));
		hostp->ip = i * i;
		ip = i * i;
		htable_add(t, &ip, &hostp->list);
	}

	for (i = 0; i < SIZE / 2; i++) {
		ip = i * i;
		node = htable_find(t, &ip);
		hostp = list_entry(node, tcpm_host_t, list);
		assert(hostp->ip == i * i);
	}

	i = 0;
	for_each_htable_entry(t, hostp, list) {
		i++;
	}
	assert(i == (SIZE / 2));

	for (i = 0; i < SIZE; i++) {
		list_head_t *head = &t->bucket[i];
		printf("bucket %d: ", i);
		list_for_each_entry(hostp, head, list) {
			printf(". ");
		}
		printf("\n");
	}

	for_each_htable_entry(t, hostp, list) {
		list_del(&hostp->list);
		free(hostp);
	}

	i = 0;
	for_each_htable_entry(t, hostp, list) {
		i++;
	}
	assert(i == 0);

	htable_del(t, &ip);
	assert(!htable_find(t, &ip));

	htable_destroy(t);
}
