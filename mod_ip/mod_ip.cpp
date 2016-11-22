/******************************************************************************

  Copyright (C), 2001-2011.

 ******************************************************************************
  File Name     : mod_ip.cpp
  Version       : Initial Draft
  Author        : wangsenbo
  Created       : 2010/1/26
  Last Modified :
  Description   : modify the local adapter ip address
  Function List :
  History       :
  1.Date        : 2010/1/26
    Author      : wangsenbo
    Modification: Created file

******************************************************************************/
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <iphlpapi.h>
#include <winsock.h>
#include "macro_def.h"
#include "mod_ip.h"

using namespace std;

void PrintLocalAdpInfo(ADP_INFO_LIST* plInfoList)
{
    #ifdef GLOBAL_DEBUG
    IP_ADAPTER_INFO *pAdapterInfo;
    if (NULL == plInfoList) return;

    pAdapterInfo = plInfoList->pHead;
    cout << "The Local adapters:" << endl;
    while (NULL != pAdapterInfo) {        
        cout << "  ADP:  " << pAdapterInfo->AdapterName << endl;
        cout << "  Desc: " << pAdapterInfo->Description << endl;
        cout << "  IP:   " << pAdapterInfo->IpAddressList.IpAddress.String <<" "  \
             << pAdapterInfo->IpAddressList.IpMask.String << endl;
        cout << endl;
        pAdapterInfo = pAdapterInfo->Next;
    }
    
    #endif

    return;
}

/*****************************************************************************
 Prototype    : GetLocalAdapterInfo
 Description  : get local adapters' iformation
 Input        : ADP_INFO_LIST* pInfoList  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2010/1/26
    Author       : wangsenbo
    Modification : Created function

*****************************************************************************/
int GetLocalAdapterInfo(ADP_INFO_LIST* pInfoList)
{
    ULONG ulAdapterInfoSize = sizeof(IP_ADAPTER_INFO);
    IP_ADAPTER_INFO *pAdapterInfoBkp, *pAdapterInfo ;

    #ifdef ADD_INFO_LIST
    if (NULL == pInfoList) return -1;
    #endif
    
    pAdapterInfoBkp = NULL;
    pAdapterInfo = (IP_ADAPTER_INFO*)new char[ulAdapterInfoSize];
    if (NULL == pAdapterInfo) return -1;
    
    if( GetAdaptersInfo(pAdapterInfo, &ulAdapterInfoSize) == ERROR_BUFFER_OVERFLOW ) // ??????
    {
        delete pAdapterInfo;
        pAdapterInfo = (IP_ADAPTER_INFO*)new char[ulAdapterInfoSize];
        if (NULL == pAdapterInfo) return -1;
        
        pAdapterInfoBkp = pAdapterInfo;
    }

    if( GetAdaptersInfo(pAdapterInfo, &ulAdapterInfoSize) == ERROR_SUCCESS )
    {
        do{ // ???????
            if(pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET) // ??????????
            {
                #ifdef ADD_INFO_LIST
                {
                    IP_ADAPTER_INFO *pAdapterInfoTmp;
                    pAdapterInfoTmp =(IP_ADAPTER_INFO*)new char[ulAdapterInfoSize];
                    if (NULL != pAdapterInfoTmp)
                    {
                        *pAdapterInfoTmp = *pAdapterInfo;
                        if (NULL == pInfoList->pHead) { 
                            pInfoList->pHead = pAdapterInfoTmp;
                            pInfoList->pTail = pAdapterInfoTmp;
                        } else {
                            pInfoList->pTail->Next = pAdapterInfoTmp;
                            pInfoList->pTail = pAdapterInfoTmp;
                            pAdapterInfoTmp->Next = NULL;
                        }
                    }
                }
                #else
                cout << " name: " << pAdapterInfo->AdapterName;
                cout << " desc: " << pAdapterInfo->Description;
                cout << "\n";
                #endif
            }
            pAdapterInfo = pAdapterInfo->Next;
        }while(pAdapterInfo);
    }
    
    if (NULL != pAdapterInfoBkp ) delete pAdapterInfoBkp; 
    
    return 0;
}

/*****************************************************************************
 Prototype    : GetLocalAdapterInfo
 Description  : get local adapters' iformation
                //OpenProcessToken(GetCurrentProcess, TOKEN_ADJUST_PRIVILEGES Or TOKEN_QUERY, hToken)
                //SetPrivilege()
                //CloseHandle(hToken)
 Input        : LPCTSTR lpszPrivilege
                BOOL bEnablePrivilege				
 Output       : BOOL
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2010/11/03
    Author       : wangsenbo
    Modification : Created function

*****************************************************************************/
BOOL SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable  <winnt.h> SE_SYSTEM_PROFILE_NAME
    BOOL bEnablePrivilege   // to enable or disable privilege
    ) 
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
    
	if ( !LookupPrivilegeValue( 
			NULL,            // lookup privilege on local system
			lpszPrivilege,   // privilege to lookup 
			&luid ) )        // receives LUID of privilege
	{
		cout << "LookupPrivilegeValue error: "<< GetLastError()<<endl; 
		return FALSE; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.
	if ( !AdjustTokenPrivileges(
		   hToken, 
		   FALSE, 
		   &tp, 
		   sizeof(TOKEN_PRIVILEGES), 
		   (PTOKEN_PRIVILEGES) NULL, 
		   (PDWORD) NULL) )
	{ 
		  cout << "AdjustTokenPrivileges error: "<< GetLastError() << endl; 
		  return FALSE; 
	}
    
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		  cout << "The token does not have the specified privilege." << endl;
		  return FALSE;
	}

    
	return TRUE;
}


/*****************************************************************************
 Prototype    : SetLocalAdapterInfo
 Description  : set adapter's information in the regedit
 Input        : LPCTSTR lpszAdapterName           
                LOCAL_ADP_INFO_S* pstAdapterInfo  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2010/1/26
    Author       : wangsenbo
    Modification : Created function

*****************************************************************************/
BOOL SetLocalAdapterInfo(ULONG ulSetMask, LOCAL_ADP_INFO_S* pstAdapterInfo)
{
    HKEY hKey;
    HANDLE hToken;
    string strKeyName = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";

    char mszIPAddress[IP_LEN_DEF+2]; // REG_MULTI_SZ data should add '0' in the end.
    char mszNetMask[IP_LEN_DEF+2];
    char mszNetGate[IP_LEN_DEF+2];
    char mszNetNameSer[IP_LEN_DEF*2+1];
    
    int nIP, nMask, nGate, nNmSer, ret;

    if (NULL == pstAdapterInfo) 
        return FALSE;
    
    strKeyName += pstAdapterInfo->adpName;
    
    #ifdef GLOBAL_DEBUG
    cout << "Regedit Entry: " <<strKeyName<< endl;
    #endif
	
	#if 0
    // get current process token
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken)){
        cout << "OpenProcessToken error: "<< GetLastError()<<endl; 
        return FALSE;
    }

    // set privilege
    if (!SetPrivilege(hToken, SE_SYSTEM_PROFILE_NAME, TRUE)) {
        CloseHandle(hToken);
        return FALSE;
    }
	#endif
    
    // Open the regedit 
    ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        strKeyName.c_str(),
        0,
        KEY_WRITE,
        &hKey);        
    if( ret != ERROR_SUCCESS){
        //CloseHandle(hToken);
		cout << "RegOpenKeyEx return error: " << ret <<endl;
		return FALSE;
	}

    
    // modified the ip address 
    if ((ulSetMask & MODIFY_IP) != 0) {
        strncpy(mszIPAddress, pstAdapterInfo->ipAddr, IP_LEN_DEF);
        nIP = strlen(mszIPAddress);
        *(mszIPAddress + nIP + 1) = 0x00; // REG_MULTI_SZ data should add '0' in the end.
        nIP += 2;
        
        // write the data to regedit 
		#ifdef GLOBAL_DEBUG
        cout << "Modifid IP Entry: " <<mszIPAddress<<" ... ";
        ret = RegSetValueEx(hKey, "IPAddress", 0, REG_MULTI_SZ, (unsigned char*)mszIPAddress, nIP);		
        if (ret == ERROR_SUCCESS)
            cout << " OK!" <<endl;
        else
            cout << " Failed: " <<ret<<endl;
        #else
        RegSetValueEx(hKey, "IPAddress", 0, REG_MULTI_SZ, (unsigned char*)mszIPAddress, nIP);		
        #endif
    }
    
    // modified the ip netmask 
    if ((ulSetMask & MODIFY_MASK) != 0) {
        strncpy(mszNetMask, pstAdapterInfo->netMask, IP_LEN_DEF);            
        nMask = strlen(mszNetMask);
        *(mszNetMask + nMask + 1) = 0x00; // REG_MULTI_SZ data should add '0' in the end.
        nMask += 2;  
        #ifdef GLOBAL_DEBUG
        cout << "Modifid Mask Entry: " <<mszNetMask<<" ... ";
        ret = RegSetValueEx(hKey, "SubnetMask", 0, REG_MULTI_SZ, (unsigned char*)mszNetMask, nMask);
        if (ret == ERROR_SUCCESS)
            cout << " OK!" <<endl;
        else
            cout << " Failed: " <<ret<<endl;
        #else
        RegSetValueEx(hKey, "SubnetMask", 0, REG_MULTI_SZ, (unsigned char*)mszNetMask, nMask);
        #endif        

    }
    
    // modified the default gateway 
    if ((ulSetMask & MODIFY_GW) != 0 ) {
        strncpy(mszNetGate, pstAdapterInfo->gw, IP_LEN_DEF);
        nGate = strlen(mszNetGate);
        *(mszNetGate + nGate + 1) = 0x00; // REG_MULTI_SZ data should add '0' in the end.
        nGate += 2;
        #ifdef GLOBAL_DEBUG
        cout << "Modifid Default Gateway Entry: " <<mszNetGate<<" ... ";
		ret = RegSetValueEx(hKey, "DefaultGateway", 0, REG_MULTI_SZ, (unsigned char*)mszNetGate, nGate);
        if (ret == ERROR_SUCCESS)
            cout << " OK!" <<endl;
        else
            cout << " Failed: " <<ret<<endl;
        #else
		RegSetValueEx(hKey, "DefaultGateway", 0, REG_MULTI_SZ, (unsigned char*)mszNetGate, nGate);
        #endif
    }

    // set name server ip address 
    if ((ulSetMask & MODIFY_NS) != 0 ) {
        strncpy(mszNetNameSer, pstAdapterInfo->ns, IP_LEN_DEF*2);
        nNmSer = strlen(mszNetNameSer);
        *(mszNetNameSer + nNmSer) = 0x00;
        nNmSer += 1;
        #ifdef GLOBAL_DEBUG
        cout << "Modifid NameServer Entry: " <<mszNetNameSer<< " ... ";
		ret = RegSetValueEx(hKey, "NameServer", 0, REG_SZ, (unsigned char*)mszNetNameSer, nNmSer);
        if (ret == ERROR_SUCCESS)
            cout << " OK!" <<endl;
        else
            cout << " Failed: " <<ret<<endl;
        #else
		RegSetValueEx(hKey, "NameServer", 0, REG_SZ, (unsigned char*)mszNetNameSer, nNmSer);
        #endif
    }
   
    // close the regedit 
    //RegCloseKey(hKey);
    
    // close current process token 
    CloseHandle(hToken);
    
    return TRUE;
}

#ifdef DHCP_NOTIFY
/*****************************************************************************
 Prototype    : NotifyAdpChange
 Description  : Notify the system IP changed  by DHCP api
 Input        : LPCTSTR lpszAdapterName  
                BOOL bNewIp [TRUE: ip modified]
                int nIndex  [Index of ip address,0 if the first IP (an adapter may has many ip addresses)]
                LPCTSTR pIPAddress 
                LPCTSTR pNetMask   
                int nDhcpAction [Enable DHCP,  0:no change, 1:enable DHCP, 2:disable DHCP]
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2010/1/26
    Author       : wangsenbo
    Modification : Created function

*****************************************************************************/
BOOL NotifyAdpChange(LPCTSTR lpszAdapterName, BOOL bNewIp, int nIndex, 
                            LPCTSTR pIPAddress, LPCTSTR pNetMask, int nDhcpAction)
{
    BOOL bResult = FALSE;
    HINSTANCE hDhcpDll;
    DHCPNOTIFYPROC pDhcpNotifyProc;
    WCHAR wcAdapterName[256];

    MultiByteToWideChar(CP_ACP, 0, lpszAdapterName, -1, wcAdapterName,256);

    if((hDhcpDll = LoadLibrary("dhcpcsvc")) == NULL)
        return FALSE;

    if((pDhcpNotifyProc = (DHCPNOTIFYPROC)GetProcAddress(hDhcpDll, "DhcpNotifyConfigChange")) != NULL)
        if((pDhcpNotifyProc)(NULL, wcAdapterName, bNewIp, nIndex, inet_addr(pIPAddress), inet_addr(pNetMask), 0) == ERROR_SUCCESS)
            bResult = TRUE;

    FreeLibrary(hDhcpDll);
    return bResult;
}
#endif

/*****************************************************************************
 Prototype    : SetDNS
 Description  : Set the DNS server
 Input        : ADP_INFO_LIST* plInfoList
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2010/1/26
    Author       : wangsenbo
    Modification : Created function

*****************************************************************************/
void SetDNS(ADP_INFO_LIST* plInfoList)
{
    IP_ADAPTER_INFO *pAdapterInfo;
    if (NULL == plInfoList) return;

    pAdapterInfo = plInfoList->pHead;
    
    while (NULL != pAdapterInfo) { 
        
        if (pAdapterInfo->DhcpEnabled) {
            pAdapterInfo = pAdapterInfo->Next;
            continue;
        }
        
        #ifdef GLOBAL_DEBUG
        cout << "  ADP:  " << pAdapterInfo->AdapterName << endl;
        cout << "  Desc: " << pAdapterInfo->Description << endl;
        cout << "  IP:   " << pAdapterInfo->IpAddressList.IpAddress.String <<" "  \
             << pAdapterInfo->IpAddressList.IpMask.String << endl;
        #endif
        
        LOCAL_ADP_INFO_S stAdapterInfo;
        strcpy(stAdapterInfo.adpName, pAdapterInfo->AdapterName);
        strcpy(stAdapterInfo.ns, "11.99.131.195,11.99.131.196");
        (void)SetLocalAdapterInfo((ULONG)MODIFY_NS, &stAdapterInfo);
		
		#ifdef DHCP_NOTIFY
        (void)NotifyAdpChange(pAdapterInfo->AdapterName, FALSE, 0, pAdapterInfo->IpAddressList.IpAddress.String, \
                              pAdapterInfo->IpAddressList.IpMask.String, 0);
        #endif
        pAdapterInfo = pAdapterInfo->Next;
    }
    
    return;
}

