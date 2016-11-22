/******************************************************************************

  Copyright (C), 2001-2011.

 ******************************************************************************
  File Name     : putfile.c
  Version       : Initial Draft
  Author        : wangsenbo
  Created       : 2010/1/22
  Last Modified :
  Description   : test ping 
  Function List :
              main
              WriteStr2File
  History       :
  1.Date        : 2010/1/22
    Author      : wangsenbo
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <time.h>

#define SRC_FILE_NAME "computer.txt"
#define DST_FILE_NAME "ping_test.txt"
#define DST_IP_ADDRESS "10.0.10.16"
#define GW_IP_ADDRESS  "10.0.11.254"
//#define DST_IP_ADDRESS "10.100.1.78"
//#define GW_IP_ADDRESS "172.16.21.254"

/*****************************************************************************
 Prototype    : WriteStr2File
 Description  : write string 2 file
 Input        : char* filename    
                char* str         
                unsigned int len  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2010/1/22
    Author       : wangsenbo
    Modification : Created function

*****************************************************************************/
int WriteStr2File(char* filename, char* str, unsigned int len)
{
    FILE *stream;
    int iWriteLen = 0;
    
    if (NULL == filename || NULL == str) return -1;
    if (0 == len) return 0;
    
    stream = fopen(filename, "a+");
    if (NULL == stream) return -1;
    
    iWriteLen = fwrite(str, len, 1, stream);
    
    fclose(stream);
    
    return iWriteLen;
}

/*****************************************************************************
 Prototype    : main
 Description  : got ping result to file
 Input        : int argc      
                char* argv[]  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2010/1/22
    Author       : wangsenbo
    Modification : Created function

*****************************************************************************/
int main(int argc, char* argv[])
{

    time_t t;  
    char a[1024];
    char tmp[64];
    struct tm* tblock;   
    FILE *fh;
    
    printf("测试开始，不要随便关闭此窗口\n"); 

    t = time(NULL);
    tblock = localtime(&t); 
    sprintf(tmp,"%s",asctime(tblock));
    sprintf(a, "\n==================================================  \
                \n   Start Ping at: %s                                \
                \n   Write to FILE: %s                                \
                \n==================================================  \
                \n", tmp,   DST_FILE_NAME);        
    printf(a); 
    WriteStr2File(DST_FILE_NAME, a, strlen(a));       

    fh = fopen(SRC_FILE_NAME, "a+");
    if (NULL == fh) {
        printf("open file %s error!\n", SRC_FILE_NAME);
        return -1;
    }

    
    memset(tmp,0,64);
    while(NULL != fgets(tmp, 64, fh ))
    {
        /*
        t = time(NULL);
        tblock = localtime(&t); 
        sprintf(tmp,"%s",asctime(tblock));
        sprintf(a, "\n###########Start Ping at: %s", tmp);  
        WriteStr2File(DST_FILE_NAME, a, strlen(a));
        */
        int tmplen = strlen(tmp);
        
        while ('\r' == tmp[tmplen-1] || '\n' == tmp[tmplen-1] ) {
            tmp[tmplen-1] = 0;  
            tmplen --;
        }
        
        printf("ping %s... \n", tmp);
        
        sprintf(a, "ping %s  >> %s " ,  tmp, DST_FILE_NAME);        
        system(a);
        /*
        sprintf(a, "ping %s  -n 5 -l 3000 >> %s " , DST_IP_ADDRESS, DST_FILE_NAME);
        system(a); 
        */
        memset(tmp,0,64);
        sleep(2000);
    }        

    fclose(fh);
    return 0;
}

