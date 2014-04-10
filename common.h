/* 
 *  Only for linux x86_64 and gcc!!!
 *  
 *  yalung929@gmail.com 
 *  2014.3 F1 Bantian Shenzhen China 
 */

#ifndef COMMON__H
#define COMMON__H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/epoll.h>

#define POWEROF2(x) ((((x)-1) & (x)) == 0)

#define CACHE_LINE_SIZE 64
#define __cache_aligned __attribute__((__aligned__(CACHE_LINE_SIZE)))

#define CACHE_LINE_MASK (CACHE_LINE_SIZE-1)

#define CACHE_LINE_ROUNDUP(size) \
    (CACHE_LINE_SIZE * ((size + CACHE_LINE_SIZE - 1) / CACHE_LINE_SIZE))

#define smp_rmb()    asm volatile("lfence":::"memory")
#define smp_mb()    asm volatile("mfence":::"memory")
#define smp_wmb()    asm volatile("sfence" ::: "memory")

#define __LOCAL(var, line) __ ## var ## line
#define _LOCAL(var, line) __LOCAL(var, line)
#define LOCAL(var) _LOCAL(var, __LINE__)

#define container_of(ptr, type, member) ({          \
    const typeof(((type *)0)->member) *__mptr = (ptr);  \
    (type *)((char *)__mptr - offsetof(type, member)); })

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

#define atomic_compare_and_swap __sync_bool_compare_and_swap
#define atomic_add __sync_fetch_and_add
#define atomic_sub __sync_fetch_and_sub
#define compile_barrier() asm volatile ( "" ::: "memroy" )

typedef int socket_t;
typedef uint32_t ip_addr_t;
typedef uint16_t port_t;
typedef uint64_t mac_addr_t;

#define UNUSED(x) (void)(x)

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

#endif
