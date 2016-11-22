/******************************************************************************

  Copyright (C), 2001-2011.

 ******************************************************************************
  File Name     : listfunc.h
  Version       : Initial Draft
  Author        : wangsenbo
  Created       : 2010/1/4
  Last Modified :
  Description   : listfunc.c header file
  Function List :
  History       :
  1.Date        : 2010/1/4
    Author      : wangsenbo
    Modification: Created file

******************************************************************************/


#ifndef __LISTFUNC_H__
#define __LISTFUNC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef struct LIST_NODE
{
    unsigned int uiDate;
    unsigned int uiLen;
    struct LIST_NODE* next;
    struct LIST_NODE* prev;
}LIST_NODE_S;

typedef struct LIST
{
    LIST_NODE_S *pstHead;
    LIST_NODE_S *pstTail;
    unsigned int uiCount; 
}LIST_S;


LIST_S* LIST_Init(void);
void LIST_Destroy(LIST_S* pstList);
LIST_NODE_S *LIST_Alloc_Node(void);
void LIST_Free_Node(LIST_NODE_S* pnode);
void LIST_Del_Node(LIST_S* plist, LIST_NODE_S* pnode);
bool LIST_AddToHead(LIST_S* plist, LIST_NODE_S* pnode);
bool LIST_AddToTail(LIST_S* plist, LIST_NODE_S* pnode);
    
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __LISTFUNC_H__ */
