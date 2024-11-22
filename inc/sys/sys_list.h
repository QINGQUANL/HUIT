
#ifndef _SYS_LIST_H_
#define _SYS_LIST_H_

#include "sys_types.h"

#ifdef __cplusplus
 extern "C" {
#endif


/**
 * Double List structure
 */
struct sys_list_node
{
    struct sys_list_node *next;                          /**< point to next node. */
    struct sys_list_node *prev;                          /**< point to prev node. */
};
typedef struct sys_list_node sys_list_t;                  /**< Type for lists. */

/**
 * Single List structure
 */
struct sys_slist_node
{
    struct sys_slist_node *next;                         /**< point to next node. */
};
typedef struct sys_slist_node sys_slist_t;                /**< Type for single list. */

/**
 * sys_container_of - return the member address of ptr, if the type of ptr is the
 * struct type.
 */
#define sys_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))


/**
 * @brief initialize a list object
 */
#define RT_LIST_OBJECT_INIT(object) { &(object), &(object) }

/**
 * @brief initialize a list
 *
 * @param l list to be initialized
 */
static inline void sys_list_init(sys_list_t *l)
{
    l->next = l->prev = l;
}

/**
 * @brief insert a node after a list
 *
 * @param l list to insert it
 * @param n new node to be inserted
 */
static inline void sys_list_insesys_after(sys_list_t *l, sys_list_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/**
 * @brief insert a node before a list
 *
 * @param n new node to be inserted
 * @param l list to insert it
 */
static inline void sys_list_insesys_before(sys_list_t *l, sys_list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

/**
 * @brief remove node from list.
 * @param n the node to remove from the list.
 */
static inline void sys_list_remove(sys_list_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

/**
 * @brief tests whether a list is empty
 * @param l the list to test.
 */
static inline int sys_list_isempty(const sys_list_t *l)
{
    return l->next == l;
}

/**
 * @brief get the list length
 * @param l the list to get.
 */
static inline unsigned int sys_list_len(const sys_list_t *l)
{
    unsigned int len = 0;
    const sys_list_t *p = l;
    while (p->next != l)
    {
        p = p->next;
        len ++;
    }

    return len;
}

/**
 * @brief get the struct for this entry
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define sys_list_entry(node, type, member) \
    sys_container_of(node, type, member)

/**
 * sys_list_for_each - iterate over a list
 * @pos:    the sys_list_t * to use as a loop cursor.
 * @head:   the head for your list.
 */
#define sys_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * sys_list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:    the sys_list_t * to use as a loop cursor.
 * @n:      another sys_list_t * to use as temporary storage
 * @head:   the head for your list.
 */
#define sys_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * sys_list_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define sys_list_for_each_entry(pos, head, member) \
    for (pos = sys_list_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = sys_list_entry(pos->member.next, typeof(*pos), member))

/**
 * sys_list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define sys_list_for_each_entry_safe(pos, n, head, member) \
    for (pos = sys_list_entry((head)->next, typeof(*pos), member), \
         n = sys_list_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = n, n = sys_list_entry(n->member.next, typeof(*n), member))

/**
 * sys_list_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define sys_list_first_entry(ptr, type, member) \
    sys_list_entry((ptr)->next, type, member)

#define RT_SLIST_OBJECT_INIT(object) { NULL }

/**
 * @brief initialize a single list
 *
 * @param l the single list to be initialized
 */
static inline void sys_slist_init(sys_slist_t *l)
{
    l->next = NULL;
}

static inline void sys_slist_append(sys_slist_t *l, sys_slist_t *n)
{
    struct sys_slist_node *node;

    node = l;
    while (node->next) node = node->next;

    /* append the node to the tail */
    node->next = n;
    n->next = NULL;
}

static inline void sys_slist_insert(sys_slist_t *l, sys_slist_t *n)
{
    n->next = l->next;
    l->next = n;
}

static inline unsigned int sys_slist_len(const sys_slist_t *l)
{
    unsigned int len = 0;
    const sys_slist_t *list = l->next;
    while (list != NULL)
    {
        list = list->next;
        len ++;
    }

    return len;
}

static inline sys_slist_t *sys_slist_remove(sys_slist_t *l, sys_slist_t *n)
{
    /* remove slist head */
    struct sys_slist_node *node = l;
    while (node->next && node->next != n) node = node->next;

    /* remove node */
    if (node->next != (sys_slist_t *)0) node->next = node->next->next;

    return l;
}

static inline sys_slist_t *sys_slist_first(sys_slist_t *l)
{
    return l->next;
}

static inline sys_slist_t *sys_slist_tail(sys_slist_t *l)
{
    while (l->next) l = l->next;

    return l;
}

static inline sys_slist_t *sys_slist_next(sys_slist_t *n)
{
    return n->next;
}

static inline int sys_slist_isempty(sys_slist_t *l)
{
    return l->next == NULL;
}

/**
 * @brief get the struct for this single list node
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define sys_slist_entry(node, type, member) \
    sys_container_of(node, type, member)

/**
 * sys_slist_for_each - iterate over a single list
 * @pos:    the sys_slist_t * to use as a loop cursor.
 * @head:   the head for your single list.
 */
#define sys_slist_for_each(pos, head) \
    for (pos = (head)->next; pos != NULL; pos = pos->next)

/**
 * sys_slist_for_each_entry  -   iterate over single list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your single list.
 * @member: the name of the list_struct within the struct.
 */
#define sys_slist_for_each_entry(pos, head, member) \
    for (pos = sys_slist_entry((head)->next, typeof(*pos), member); \
         &pos->member != (NULL); \
         pos = sys_slist_entry(pos->member.next, typeof(*pos), member))

/**
 * sys_slist_first_entry - get the first element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define sys_slist_first_entry(ptr, type, member) \
    sys_slist_entry((ptr)->next, type, member)

/**
 * sys_slist_tail_entry - get the tail element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define sys_slist_tail_entry(ptr, type, member) \
    sys_slist_entry(sys_slist_tail(ptr), type, member)




#ifdef __cplusplus
}
#endif

#endif // _SYS_LIST_H_
