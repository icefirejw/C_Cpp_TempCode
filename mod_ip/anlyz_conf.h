/******************************************************************************

  Copyright (C), 2001-2011.

 ******************************************************************************
  File Name     : anlyz_conf.h
  Version       : Initial Draft
  Author        : wangsenbo
  Created       : 2010/2/4
  Last Modified :
  Description   : anlyz_conf.cpp header file
  Function List :
  History       :
  1.Date        : 2010/2/4
    Author      : wangsenbo
    Modification: Created file

******************************************************************************/


#ifndef __ANLYZ_CONF_H__
#define __ANLYZ_CONF_H__

#define FILE_NAME    "net.cfg"
#define LINE_BUF_LEN 128
#define VALUE_LEN    64

#define OPEN_FILE_ERROR 0xFEFEFEFE

typedef enum  {
    UNDEF_TYPE = 0, // name defined 
    IP_ADDRESS,     // ip address
    IP_MASK,        // ip netmask
    IP_GATEWAY,     // default gateway
    IP_NS1,         // first name server
    IP_NS2 = 5      // second name server
} CONF_TYPE_DEF;

enum ANALYSE_TYPE{
    ANALYZ_NETWORK = 1
};

typedef struct AdpConfInfo{
    string Name;                /* config name */
    string AdapterId;           /* Adapter ID  */
    string IPList;              /* ip address  */
    string GWList;              /* default gateway */
    string NSList;              /* 1st name server */
}ADP_CONF_INFO_S;

extern char* conf_def[];

int fileAnalyse(const char* filename, int anlyz_type);
int confAnalyse(const char* inStr, unsigned int len);
int stringAnalyse(const char* inStr, unsigned int len, char* outStr, int* outLen);


#endif /* __ANLYZ_CONF_H__ */
