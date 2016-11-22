/******************************************************************************

  Copyright (C), 2001-2011.

 ******************************************************************************
  File Name     : mylist.c
  Version       : Initial Draft
  Author        : wangsenbo
  Created       : 2010/1/4
  Last Modified :
  Description   : define the list functions
  Function List :
  History       :
  1.Date        : 2010/1/4
    Author      : wangsenbo
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro_def.h"
#include "listfunc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#if GLOBAL_DEBUG
#define LIST_DEBUG(s) printf("FILE:%s LINE: %d, error occured.\n", __FILE__, __LINE__)
#else
#define LIST_DEBUG(s)
#endif

LIST_S* LIST_Init(void)
{
    LIST_S* pstHead;

    pstHead = (LIST_S*)malloc(sizeof(LIST_S));
    if (NULL == pstHead) {
        LIST_DEBUG(0);
        return NULL;
    }

    memset(pstHead, 0, sizeof(LIST_S));

    return pstHead;
}

void LIST_Destroy(LIST_S* pstList)
{
    LIST_NODE_S* pnode;
    LIST_NODE_S* pnext;
    
    if (NULL == pstList) return;

    pnode = pstList->pstHead;
    
    while (pnode!=NULL && pnode!=pstList->pstTail){
        pnext = pnode->next;
        LIST_Del_Node(pstList, pnode);
        LIST_Free_Node(pnode);
        pnode = pnext;
    }
}

LIST_NODE_S *LIST_Alloc_Node(void)
{
    LIST_NODE_S* p;

    p = (LIST_NODE_S*)malloc(sizeof(LIST_NODE_S));
    if (NULL == p) return NULL;

    memset(p, 0, sizeof(LIST_NODE_S));
    return p;
}

void LIST_Free_Node(LIST_NODE_S* pnode)
{
    if (NULL == pnode)  return;
    free(pnode);
}

void LIST_Del_Node(LIST_S* plist, LIST_NODE_S* pnode)
{
    if (NULL == pnode) return;

    /* if node is the head of list, make the next node to be head */
    if (pnode == plist->pstHead) plist->pstHead = pnode->next;
    /* if node is the tail of list, make the prev node to be tail */
    if (pnode == plist->pstTail) plist->pstTail = pnode->prev;

    /* delete node */
    if (pnode->prev != NULL) pnode->prev->next = pnode->next;
    if (pnode->next != NULL) pnode->next->prev = pnode->prev;

    pnode->next = NULL;
    pnode->prev = NULL;

    return;
}

bool LIST_AddToHead(LIST_S* plist, LIST_NODE_S* pnode)
{
    if (NULL == plist || NULL == pnode) {
        LIST_DEBUG(0);
        return false;
    }

    /* the list has no node */
    if (NULL == plist->pstHead) {
        plist->pstHead = pnode;
        plist->pstTail = pnode;
    }

    /* add the node to the head of list */
    pnode->prev = plist->pstHead->prev;
    pnode->next = plist->pstHead;
    plist->pstHead->prev = pnode;
    if (pnode->prev != NULL) pnode->prev->next = pnode;

    /* make this node be the head of list */    
    plist->pstHead = pnode;

    return true;
}

bool LIST_AddToTail(LIST_S* plist, LIST_NODE_S* pnode)
{
    if (NULL == plist || NULL == pnode) {
        LIST_DEBUG(0);
        return false;
    }

    /* the list has no node */
    if (NULL == plist->pstHead || NULL == plist->pstTail) {
        plist->pstHead = pnode;
        plist->pstTail = pnode;
    }
    
    /* add node to tail */
    pnode->next = plist->pstTail->next;
    pnode->prev = plist->pstTail;
    plist->pstTail->next = pnode;
    if (pnode->next != NULL) pnode->next->prev = pnode;

    /* make this node be the tail of list */
    plist->pstTail = pnode;

    return true;
}

#define LIST_TEST 
#ifdef LIST_TEST
int main(void)
{
    LIST_S* plist;
    LIST_NODE_S* pnode;
    int i;
    
    plist = LIST_Init();

    for (i=0; i<10; i++){
        pnode = LIST_Alloc_Node();
        pnode->uiDate = i;
        LIST_AddToHead(plist, pnode);
    }
    pnode = plist.pstHead;
    while(pnode != NULL){
        printf("node data:%d\n", pnode->uiDate);
        pnode = pnode->next;
    }
    LIST_Destroy(plist);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */



