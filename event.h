/* 
 *  Only for linux x86_64 and gcc!!!
 *  
 *  yalung929@gmail.com 
 *  2014.3 F1 Bantian Shenzhen China 
 */


#ifndef EVENT__HHH
#define EVENT__HHH

#include "common.h"
#include "list.h"
#include <sys/epoll.h>

#define EV_TIMER_RESOLUTION 1	/* 1 msec */

#define EV_READ_EVENT EPOLLIN
#define EV_WRITE_EVENT EPOLLOUT

struct ev_event;
struct ev_timer;

typedef void *ev_user_ptr;
typedef void (*ev_event_callback_t) (struct ev_event * event);

typedef void (*ev_timer_callback_t) (struct ev_timer * timer);

/* embed this to user data struct */
typedef struct ev_event {

	int fd;

	int events;

	ev_event_callback_t callback;

} ev_event_t;

typedef struct ev_timer {

	uint64_t msec;

	uint64_t abs_msec;

	ev_timer_callback_t callback;

	ev_user_ptr data;

	list_node_t list;
} ev_timer_t;

typedef struct ev_context {

	int efd;

	volatile int stopped;

	list_head_t timer_list;	/* timer! */

	int max_events;
	struct epoll_event events[0];
} ev_context_t;

ev_context_t *ev_create_context(int max_events);
void ev_destory_context(ev_context_t * c);

int ev_run(ev_context_t * c);

int ev_register_event(ev_context_t * c, ev_event_t * event);

void ev_unregister_event(ev_context_t * c, ev_event_t * event);

void ev_init_timer(ev_timer_t * timer, uint64_t msec,
		   ev_timer_callback_t callback);

/* must called in same thread as ev_run  */
void ev_start_timer(ev_context_t * c, ev_timer_t * timer);

/* must called in same thread as ev_run  */
void ev_cancel_timer(ev_context_t * c, ev_timer_t * timer);

#endif
