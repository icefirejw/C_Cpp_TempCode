#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TMPBUF_LEN    2048
#define TMP_FILE_BUF  128
#define FILENAME_LEN  128
/*
#define SRC_FILE      "10.99.28.254_any_any.htm"
#define DST_FILE      "bbb.txt"
*/
#define STR_START     "src="
#define STR_END       "smac="
#define IS_NUMBER(a) ((a)>=0 && (a)<=9)

typedef struct stDataList_def
{
    struct stDataList_def* prev;
    struct stDataList_def* next;
    char*  pData;
    int    DataLen;
}stDataList;


char* DST_FILENAME;
char* SRC_FILENAME;
stDataList *g_pstDataList;



/*****************************************************************************
 Prototype    : AllocDataNode
 Description  : alloc a node memery and initialize the memory
 Input        : void  
 Output       : None
 Return Value : stDataList
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2009/10/26
    Author       : wangsenbo
    Modification : Created function

*****************************************************************************/
stDataList *AllocDataNode(void)
{
    stDataList *p;    
    p = malloc(sizeof(stDataList));
    if (NULL != p) memset(p, 0, sizeof(stDataList));

    return p;
}

void FreeDataNode(stDataList *pstNode)
{
    if (NULL == pstNode) return;
    if (NULL != pstNode->pData) free(pstNode->pData);
    memset(pstNode,0,sizeof(stDataList));
    free(pstNode);
    
    return;
}
    
void initDataList(void)
{
    g_pstDataList = NULL;
    
    return;
}

void DestroyDataList(void)
{
    stDataList *pNext;
    stDataList *p;

    p = g_pstDataList;
    
    while (NULL != p){
        pNext = p->next;
        FreeDataNode(p);
        p = pNext;
    }
    g_pstDataList = NULL;
}

void AddData(stDataList *pstDataNode)
{
    if (NULL == pstDataNode) return;

    if (NULL == g_pstDataList){
        g_pstDataList = pstDataNode;
        return;
    }
    /* add the node to head */
    pstDataNode->next = g_pstDataList;
    g_pstDataList->prev = pstDataNode;
    g_pstDataList = pstDataNode;
    return;
}

void DelData(stDataList *pstDataNode)
{
    if (NULL == pstDataNode) return;
    
    /* if this node is the head of list,
       make the next node of this node to be the head.
    */
    if (NULL == pstDataNode->prev) g_pstDataList = pstDataNode->next;

    /* delete node from list */
    if (NULL != pstDataNode->prev)
        pstDataNode->prev->next = pstDataNode->next;
    if (NULL != pstDataNode->next)
        pstDataNode->next->prev = pstDataNode->prev;

    FreeDataNode(pstDataNode);

    return;
}

int write2DataList(char* buff, int bufLen)
{
    stDataList* pstDataNode;
    char* pData = NULL;

    pstDataNode = AllocDataNode();
    if (NULL == pstDataNode) return -1;

    pData = malloc(bufLen);
    if (NULL == pData) {
        FreeDataNode(pstDataNode);
        return -1;
    }

    strcpy(pData, buff);
    pstDataNode->pData = pData;
    AddData(pstDataNode);
    return 0;        
}

void AdjustDataList(void)
{
    stDataList* pCur = NULL;
    stDataList* pNext = NULL;
    stDataList* p = NULL;
    stDataList* pN = NULL;
    
    pCur = g_pstDataList;
    while (NULL != pCur){
        pNext = pCur->next;
        p=pNext;
        while (NULL != p){
            if ((NULL != pCur->pData && NULL != p->pData) 
                && (0 == strcmp(pCur->pData, p->pData)))  {
                    pN = p->next;
                    DelData(p);
                    if (p == pNext) pNext = pN;
                    p = pN;
                    continue;
                }
            p = p->next;
        }
        pCur = pNext;
    }        
}

int initFile(char* srcFile, char* dstFile)
{
    FILE* fHandle;
    
    if (NULL == srcFile || NULL == dstFile)
        return -1;

    SRC_FILENAME = srcFile;
    fHandle = fopen(SRC_FILENAME, "rb");
    if (NULL == fHandle)
        return -1;
    fclose(fHandle);
        
    DST_FILENAME = dstFile;
    fHandle = fopen(DST_FILENAME, "wb");
    if (NULL != fHandle ) fclose(fHandle);
    
    return 0;
}

int write2File(char* buff, int buffLen)
{
    FILE* dstFile = NULL;

    dstFile = fopen(DST_FILENAME, "ab+");
    if (NULL == dstFile) return 0;
    
    fwrite(buff, buffLen, 1, dstFile);
    fclose(dstFile);
    return 0;
}


int writeList2File(void)
{
    stDataList *p;
    int ret = 0;
    FILE* dstFile = NULL;
    
    dstFile = fopen(DST_FILENAME, "ab+");
    if (NULL == dstFile) return 0;
    
    p = g_pstDataList;
    while (NULL != p){
        if (NULL != p->pData)
            ret = fwrite(p->pData, strlen(p->pData), 1, dstFile);
        p = p->next;
    }

    fclose(dstFile);
    
    return ret;
}

#ifndef SPORT
#define SPORT_STR "sport"
#define DPORT_STR "dport"
void delSPort(char* StringCon)
{
    char* tmp_start;
    char* tmp_end;

    tmp_start = strstr(StringCon,SPORT_STR);
    if (NULL == tmp_start) return ;     /* cant found the string */
    
    tmp_end  = strstr(StringCon,DPORT_STR);
    if (NULL == tmp_end) return;       /* cant found the string */
    
    strcpy(tmp_start, tmp_end);
    
    return;
}
#endif
char* searchStr(char* outPutBuf,  int BufLen, char* inPutBuf,  char* strStart, char* strEnd)
{
    char* tmpbuf_start = NULL;
    char* tmpbuf_end = NULL;
    int    Len = 0;

    outPutBuf[0] = '\0';   

    tmpbuf_start = strstr(inPutBuf,strStart);
    if (NULL == tmpbuf_start) return NULL;     /* cant found the string */
    
    tmpbuf_end  = strstr(tmpbuf_start,strEnd);
    if (NULL == tmpbuf_end) return NULL;      /* cant found the string */
    
    /* copy the searched string to buffer */
    Len = tmpbuf_end - tmpbuf_start;
    if (Len >= BufLen ) Len = BufLen - 1;  /* \0 */
    memcpy(outPutBuf, tmpbuf_start, Len);
    outPutBuf[Len] = '\0';
    
    #ifndef SPORT /* delete sport content 'sport=xxxx' */
    delSPort(outPutBuf);
    #endif
    
    /* return the point for searching next time */
    return tmpbuf_end + strlen(strEnd); 
    
}

int getStr(char* outPutBuf, int outPutBufLen, char* strStart, char* strEnd)
{
    int getLen = 0;
    int bufLen = 0; 
    char* tmpBuf;
    char* p;
    int i;
    
    FILE* pSrcFile;
    
    if (NULL == outPutBuf || NULL == strStart || NULL == strEnd)  
        return 0;

    outPutBuf[0] = 0;

    pSrcFile = fopen(SRC_FILENAME, "rb");
    if (NULL == pSrcFile) return 0;
    tmpBuf = malloc(TMPBUF_LEN);

    for (i=0;;i++) {
        /* get content from file */
        if (NULL != fgets(tmpBuf, TMPBUF_LEN, pSrcFile)){
            p = tmpBuf;
            while (NULL != p) {
                /* -1 for '\n' */
                p = searchStr( outPutBuf, outPutBufLen - 1, p, strStart, strEnd);
                bufLen =  strlen(outPutBuf);
                if (bufLen > 0 ) {
                    /* delete the */
                    while (!IS_NUMBER(outPutBuf[bufLen-1] - '0')) 
                        bufLen = bufLen-1;
                    outPutBuf[bufLen] = '\n';
                    bufLen++; /* added \n */
                    outPutBuf[bufLen] = '\0';
                    /* write2File(outPutBuf,bufLen); */
                    write2DataList(outPutBuf,bufLen+1); /* +1 for \0 */
                    getLen +=bufLen;
                }
            }
        } else {
            break;
        }        
        if (i > 0xFFFFFFFF) break; /* avoid deadloop */
    }
    
    free(tmpBuf);
    fclose(pSrcFile);
    
    return getLen;
}
/*
#define TEST_STR "recorder=FW-NAT src=193.168.10.51 dst=10.99.29.6 sport=30190 dport=8001 smac=00:14:f2:82:41:bf dmac=00:13:32:02:6f:50 "
*/

int main(void)
{
    char tmp[TMP_FILE_BUF];
    int strLen;
    char srcFile[FILENAME_LEN];
    char dstFile[FILENAME_LEN];    

    printf("Please input the source and destnation filename. \n");
    printf("source filename: ");
    gets(srcFile);
    printf("dest filename: ");
    gets(dstFile);
    if (0 != initFile(srcFile,dstFile)) {
        printf("Open file Failed, press ENTER to end ...\n");    
        getchar();
        return -1;
    }

    initDataList();

    /* this function will alloc the memory for DataList */
    strLen = getStr(tmp, TMP_FILE_BUF, STR_START, STR_END);
    if (strLen <= 0) {
        printf("Get string Failed, press ENTER to end ...\n");
        getchar();
        return -1;
    }
    
    AdjustDataList(); /* delete some same nodes */
    writeList2File(); /* write the content to file */
    DestroyDataList(); /* destroy the list for memory leak */
    printf("Process Successfully, press ENTER to end ...\n"); 
    getchar();
    
    return 0;        
}

#ifdef __cplusplus
}
#endif

