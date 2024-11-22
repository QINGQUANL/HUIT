/*
*********************************************************************************************************
*
* Filename      : yp3_list_head.h
* Description   : header file
* Revision      : V1.0
* Author        : ZYJ
* Created       : 2021/03/23
* Last Updated  : 2021/03/23
*
* Revision History:
* 2021/03/23   Revision 1.0
*
*
*********************************************************************************************************
*/
#ifndef __SPARK_LIST_HEAD_H__
#define __SPARK_LIST_HEAD_H__

#ifdef STM32F429xx
#include "sys.h"
#else
#include "sys_inc.h"
#endif /* STM32F429xx */
/********************************************************************************************************
*   DEFINE
********************************************************************************************************/
#define dlist_is_empty(head)     (((head)->next == (head)) && ((head)->prev == (head)))
/********************************************************************************************************
*   CONSTANTS
********************************************************************************************************/

/********************************************************************************************************
*   CUSTOMIZATION
********************************************************************************************************/

/********************************************************************************************************
*   DATA TYPES
********************************************************************************************************/
typedef struct _doubly_linked_list_header
{
    struct _doubly_linked_list_header *next;
    struct _doubly_linked_list_header *prev;

    /* list payload followed */

} dlist_hdr_t;

typedef struct _singly_linked_list_header
{
    struct _singly_linked_list_header *next;

    /* list payload followed */
} slist_hdr_t;
/********************************************************************************************************
*   GLOBAL VARIABLES
********************************************************************************************************/

/********************************************************************************************************
*   FUNCTIONS
********************************************************************************************************/
/* API for doubly linked list */
int8_t dlist_init(void *head);
int8_t dlist_seek_next(void *dlist, void *item);
int8_t dlist_seek_prev(void *dlist, void *item);
int8_t dlist_add_tail(void *dlist, void *item);
int8_t dlist_add_head(void *dlist, void *item);
int8_t dlist_rmv(void *item);

/* API for singly linked list */
int8_t slist_add_lifo(slist_hdr_t **slist, void *item);
int8_t slist_rmv_lifo(slist_hdr_t **slist, void *item);
int8_t slist_add_fifo(slist_hdr_t **slist, void *item);
int8_t slist_rmv_fifo(slist_hdr_t **slist, void *item);
#endif /* __SPARK_LIST_HEAD_H__ */


