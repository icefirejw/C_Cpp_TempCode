/******************************************************************************

  Copyright (C), 2001-2011.

 ******************************************************************************
  File Name     : mod_ip.h
  Version       : Initial Draft
  Author        : wangsenbo
  Created       : 2010/1/26
  Last Modified :
  Description   : mod_ip.cpp header file
  Function List :
  History       :
  1.Date        : 2010/1/26
    Author      : wangsenbo
    Modification: Created file

******************************************************************************/


#ifndef __MOD_IP_H__
#define __MOD_IP_H__

#define MODIFY_ALL    0xFFFFFFFFUL
#define MODIFY_IP     0x00000001UL
#define MODIFY_MASK   0x00000002UL
#define MODIFY_GW     0x00000004UL
#define MODIFY_NS     0x00000008UL

typedef struct LocalAdpInfo{
    char adpName[MAX_ADAPTER_NAME_LENGTH+4];  /* adapter name */
    char ipAddr[IP_LEN_DEF];                  /* ip address */
    char netMask[IP_LEN_DEF];                 /* net mask   */
    char gw[IP_LEN_DEF];                      /* default gateway */
    char ns[IP_LEN_DEF*2];                    /* name server */
}LOCAL_ADP_INFO_S;

typedef struct LocalAdpInfoList{
    IP_ADAPTER_INFO *pHead;
    IP_ADAPTER_INFO *pTail;    
}ADP_INFO_LIST;

/*
BOOL DhcpNotifyConfigChange(
LPWSTR lpwszServerName,   // NULL for Local mochine
LPWSTR lpwszAdapterName,  // Name of adapter
BOOL bNewIpAddress,       // TRUE: ip modified 
DWORD dwIpIndex,          // Index of ip address,0 if the first IP (an adapter may has many ip addresses), 
                          // If only one ip address, this value should be 0.
DWORD dwIpAddress,        // Ip address.
DWORD dwSubNetMask,       // Net Mask
int nDhcpAction );        // Enable DHCP,  0:no change, 1:enable DHCP, 2:disable DHCP
*/

typedef ULONG (*DHCPNOTIFYPROC)(LPWSTR, LPWSTR, BOOL, DWORD, DWORD, DWORD, int);

int GetLocalAdapterInfo(ADP_INFO_LIST* plInfoList);
BOOL SetLocalAdapterInfo(LPCTSTR lpszAdapterName, LOCAL_ADP_INFO_S* pstAdapterInfo);
BOOL NotifyAdpChange(LPCTSTR lpszAdapterName, BOOL bNewIp, int nIndex, 
                            LPCTSTR pIPAddress, LPCTSTR pNetMask, int nDhcpAction);
void PrintLocalAdpInfo(ADP_INFO_LIST* plInfoList);
void SetDNS(ADP_INFO_LIST* plInfoList);

#endif /* __MOD_IP_H__ */

