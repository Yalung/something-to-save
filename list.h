/* 
 *  Only for linux x86_64 and gcc!!!
 *  
 *  yalung929@gmail.com 
 *  2014.3 F1 Bantian Shenzhen China 
 */


#ifndef LISTHHHHHHH
#define LISTHHHHHHH

#include "common.h"

/* stolen from kernel */

typedef struct list_node {
	struct list_node *next;
	struct list_node *prev;
} list_node_t;

typedef struct list_head {
	struct list_node n;
} list_head_t;

#define LIST_HEAD_INIT(name) { { &(name.n), &(name.n) } }
#define LIST_NODE_INIT { NULL, NULL }

#define LIST_HEAD(name) \
struct list_head name = LIST_HEAD_INIT(name)
#define LIST_NODE(name) \
struct list_node name = LIST_NODE_INIT

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->n.next = &list->n;
	list->n.prev = &list->n;
}

static inline void INIT_LIST_NODE(struct list_node *list)
{
	list->next = NULL;
	list->prev = NULL;
}

#define list_first_entry(head, type, member) \
    list_entry((head)->n.next, type, member)

static inline bool list_empty(const struct list_head *head)
{
	return head->n.next == &head->n;
}

static inline bool list_linked(const struct list_node *node)
{
	return node->next != NULL;
}

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_for_each(pos, head)                    \
    for (typeof(pos) LOCAL(n) = (pos = (head)->n.next, pos->next);  \
    pos != &(head)->n;                     \
    pos = LOCAL(n), LOCAL(n) = pos->next)

#define list_for_each_entry(pos, head, member)              \
    for (typeof(pos) LOCAL(n) = (pos = list_entry((head)->n.next,   \
    typeof(*pos), \
    member),      \
    list_entry(pos->member.next,   \
    typeof(*pos),       \
    member));       \
    &pos->member != &(head)->n;                \
    pos = LOCAL(n), LOCAL(n) = list_entry(LOCAL(n)->member.next, \
    typeof(*LOCAL(n)),   \
    member))

static inline void __list_add(struct list_node *new,
			      struct list_node *prev, struct list_node *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_node *new, struct list_head *head)
{
	__list_add(new, &head->n, head->n.next);
}

static inline void list_add_tail(struct list_node *new, struct list_head *head)
{
	__list_add(new, head->n.prev, &head->n);
}

static inline void __list_del(struct list_node *prev, struct list_node *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void __list_del_entry(struct list_node *entry)
{
	__list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_node *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = entry->prev = NULL;
}

static inline void list_move(struct list_node *list, struct list_head *head)
{
	__list_del_entry(list);
	list_add(list, head);
}

static inline void list_move_tail(struct list_node *list,
				  struct list_head *head)
{
	__list_del_entry(list);
	list_add_tail(list, head);
}

static inline void __list_splice(const struct list_head *list,
				 struct list_node *prev, struct list_node *next)
{
	struct list_node *first = list->n.next;
	struct list_node *last = list->n.prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

static inline void list_splice_init(struct list_head *list,
				    struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, &head->n, head->n.next);
		INIT_LIST_HEAD(list);
	}
}

static inline void list_splice_tail_init(struct list_head *list,
					 struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head->n.prev, &head->n);
		INIT_LIST_HEAD(list);
	}
}

#endif
