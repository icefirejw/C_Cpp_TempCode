/******************************************************************************

  Copyright (C), 2001-2011.

 ******************************************************************************
  File Name     : reg_edit.c
  Version       : Initial Draft
  Author        : wangsenbo
  Created       : 2010/1/4
  Last Modified :
  Description   : regedit opration: all the functions copied from the URLs bellow:
                  http://msdn.microsoft.com/en-us/library/ms724923(VS.85).aspx 
  Function List :
  History       :
  1.Date        : 2010/1/4
    Author      : wangsenbo
    Modification: Created file

******************************************************************************/

/***********************************************
Function                     |   Description                
----------------------------------------------------------------------------------------------
GetSystemRegistryQuota       |   Retrieves the current size of the registry and the maximum size that the registry is allowed to attain on the system. 
RegCloseKey                  |   Closes a handle to the specified registry key. 
RegConnectRegistry           |   Establishes a connection to a predefined registry handle on another computer. 
RegCopyTree                  |   Copies the specified registry key, along with its values and subkeys, to the specified destination key. 
RegCreateKeyEx               |   Creates the specified registry key. 
RegCreateKeyTransacted       |   Creates the specified registry key and associates it with a transaction. 
RegDeleteKey                 |   Deletes a subkey and its values. 
RegDeleteKeyEx               |   Deletes a subkey and its values from the specified platform-specific view of the registry. 
RegDeleteKeyTransacted       |   Deletes a subkey and its values from the specified platform-specific view of the registry as a transacted operation. 
RegDeleteKeyValue            |   Removes the specified value from the specified registry key and subkey. 
RegDeleteTree                |   Deletes the subkeys and values of the specified key recursively. 
RegDeleteValue               |   Removes a named value from the specified registry key. 
RegDisablePredefinedCache    |   Disables handle caching for the predefined registry handle for HKEY_CURRENT_USER for the current process. 
RegDisablePredefinedCacheEx  |   Disables handle caching for all predefined registry handles for the current process.  
RegDisableReflectionKey      |   Disables registry reflection for the specified key. 
RegEnableReflectionKey       |   Enables registry reflection for the specified disabled key. 
RegEnumKeyEx                 |   Enumerates the subkeys of the specified open registry key. 
RegEnumValue                 |   Enumerates the values for the specified open registry key. 
RegFlushKey                  |   Writes all attributes of the specified open registry key into the registry. 
RegGetKeySecurity            |   Retrieves a copy of the security descriptor protecting the specified open registry key. 
RegGetValue                  |   Retrieves the type and data for the specified registry value. 
RegLoadKey                   |   Creates a subkey under HKEY_USERS or HKEY_LOCAL_MACHINE and stores registration information from a specified file into that subkey. 
RegLoadMUIString             |   Loads the specified string from the specified key and subkey. 
RegNotifyChangeKeyValue      |   Notifies the caller about changes to the attributes or contents of a specified registry key. 
RegOpenCurrentUser           |   Retrieves a handle to the HKEY_CURRENT_USER key for the user the current thread is impersonating. 
RegOpenKeyEx                 |   Opens the specified registry key. 
RegOpenKeyTransacted         |   Opens the specified registry key and associates it with a transaction. 
RegOpenUserClassesRoot       |   Retrieves a handle to the HKEY_CLASSES_ROOT key for the specified user. 
RegOverridePredefKey         |   Maps a predefined registry key to a specified registry key. 
RegQueryInfoKey              |   Retrieves information about the specified registry key. 
RegQueryMultipleValues       |   Retrieves the type and data for a list of value names associated with an open registry key. 
RegQueryReflectionKey        |   Determines whether reflection has been disabled or enabled for the specified key. 
RegQueryValueEx              |   Retrieves the type and data for a specified value name associated with an open registry key. 
RegReplaceKey                |   Replaces the file backing a registry key and all its subkeys with another file. 
RegRestoreKey                |   Reads the registry information in a specified file and copies it over the specified key. 
RegSaveKey                   |   Saves the specified key and all of its subkeys and values to a new file. 
RegSaveKeyEx                 |   Saves the specified key and all of its subkeys and values to a new file. You can specify the format for the saved key or hive. 
RegSetKeyValue               |   Sets the data for the specified value in the specified registry key and subkey. 
RegSetKeySecurity            |   Sets the security of an open registry key. 
RegSetValueEx                |   Sets the data and type of a specified value under a registry key. 
RegUnLoadKey                 |   Unloads the specified registry key and its subkeys from the registry. 
************************************************/

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
//#include <strsafe.h>
#include <tchar.h>
#include <stdio.h>
//#include <pdh.h>

// QueryKey - Enumerates the subkeys of key and its associated values.
//     hKey - Key whose subkeys and values are to be enumerated.
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
 
void QueryKey(HKEY hKey) 
{ 
    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
 
    DWORD i, retCode; 
 
    TCHAR  achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 
 
    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 
 
    // Enumerate the subkeys, until RegEnumKeyEx fails.
    
    if (cSubKeys)
    {
        printf( "\nNumber of subkeys: %d\n", cSubKeys);

        for (i=0; i<cSubKeys; i++) 
        { 
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hKey, i,
                     achKey, 
                     &cbName, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &ftLastWriteTime); 
            if (retCode == ERROR_SUCCESS) 
            {
                _tprintf(TEXT("(%d) %s\n"), i+1, achKey);
            }
        }
    } 
 
    // Enumerate the key values. 

    if (cValues) 
    {
        printf( "\nNumber of values: %d\n", cValues);

        for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
        { 
            cchValue = MAX_VALUE_NAME; 
            achValue[0] = '\0'; 
            retCode = RegEnumValue(hKey, i, 
                achValue, 
                &cchValue, 
                NULL, 
                NULL,
                NULL,
                NULL);
 
            if (retCode == ERROR_SUCCESS ) 
            { 
                _tprintf(TEXT("(%d) %s\n"), i+1, achValue); 
            } 
        }
    }
}

//*************************************************************
//
//  RegDelnodeRecurse()
//
//  Purpose:    Deletes a registry key and all it's subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDelnodeRecurse (HKEY hKeyRoot, LPTSTR lpSubKey)
{
    LPTSTR lpEnd;
    LONG lResult;
    DWORD dwSize;
    TCHAR szName[MAX_PATH];
    HKEY hKey;
    FILETIME ftWrite;

    // First, see if we can delete the key without having
    // to recurse.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS) 
        return TRUE;

    lResult = RegOpenKeyEx (hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

    if (lResult != ERROR_SUCCESS) 
    {
        if (lResult == ERROR_FILE_NOT_FOUND) {
            printf("Key not found.\n");
            return TRUE;
        } 
        else {
            printf("Error opening key.\n");
            return FALSE;
        }
    }

    // Check for an ending slash and add one if it is missing.

    lpEnd = lpSubKey + lstrlen(lpSubKey);

    if (*(lpEnd - 1) != TEXT('\\')) 
    {
        *lpEnd =  TEXT('\\');
        lpEnd++;
        *lpEnd =  TEXT('\0');
    }

    // Enumerate the keys

    dwSize = MAX_PATH;
    lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
                           NULL, NULL, &ftWrite);

    if (lResult == ERROR_SUCCESS) 
    {
        do {

            /* StringCchCopy (lpEnd, MAX_PATH*2, szName);  --- modified by wangsenbo */
            strncpy(lpEnd, szName, MAX_PATH*2);
            if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
                break;
            }

            dwSize = MAX_PATH;

            lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
                                   NULL, NULL, &ftWrite);

        } while (lResult == ERROR_SUCCESS);
    }

    lpEnd--;
    *lpEnd = TEXT('\0');

    RegCloseKey (hKey);

    // Try again to delete the key.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS) 
        return TRUE;

    return FALSE;
}

//*************************************************************
//
//  RegDelnode()
//
//  Purpose:    Deletes a registry key and all it's subkeys / values.
//
//  Parameters: hKeyRoot    -   Root key
//              lpSubKey    -   SubKey to delete
//
//  Return:     TRUE if successful.
//              FALSE if an error occurs.
//
//*************************************************************

BOOL RegDelnode (HKEY hKeyRoot, LPTSTR lpSubKey)
{
    TCHAR szDelKey[MAX_PATH*2];

    /* StringCchCopy (szDelKey, MAX_PATH*2, lpSubKey); */
    strncpy(szDelKey, lpSubKey, MAX_PATH*2); 
    return RegDelnodeRecurse(hKeyRoot, szDelKey);

}
#if 0
/* --------------------------------------------------------------------------------------
Determining the Registry Size

On Windows 2000, it is common for an installation utility to check the current and maximum size of the registry to determine whether there is enough available space for the new data it will be adding. This sample demonstrates how to do this programmatically using the "% Registry Quota In Use" performance counter within the System object.

The following sample uses performance data helper (PDH) to obtain the counter value; it must be linked with Pdh.lib. PDH is a high-level set of APIs used to obtain performance data.

Note  It is not necessary to implement this registry size-check on Windows Server 2003 or Windows XP because they do not have a registry quota limit.


Copy Code 
*/
//*******************************************************************
// 
//  Retrieves the current and maximum registry size. It gets this
//  information from the raw counter values for the "% Registry Quota 
//  In Use" performance counter within the System object.
// 
//  PARAMETERS:   
//      szMachineName - Null-terminated string that specifies the
//          name of the computer whose registry you wish to query.
//          If this parameter is NULL, the local computer is used.
// 
//      lpdwCurrentSize - Receives the current registry size.
// 
//      lpdwMaximumSize - Receives the maximum registry size.
// 
//  RETURN VALUE: 
//      ERROR_SUCCESS if successful. Otherwise, the function
//      returns a PDH error code. These error codes can be
//      found in PDHMSG.H. A textual error message can be
//      retrieved from PDH.DLL using the FormatMessage function.
// 
//******************************************************************

PDH_STATUS GetRegistrySize( LPTSTR szMachineName, 
    LPDWORD lpdwCurrentSize, LPDWORD lpdwMaximumSize ) 
{
    PDH_STATUS  pdhResult   = 0;
    TCHAR       szCounterPath[1024];
    DWORD       dwPathSize  = 1024;
    PDH_COUNTER_PATH_ELEMENTS pe;
    PDH_RAW_COUNTER pdhRawValues;
    HQUERY      hQuery      = NULL;
    HCOUNTER    hCounter    = NULL;
    DWORD       dwType      = 0;

    // Open PDH query
    pdhResult = PdhOpenQuery( NULL, 0, &hQuery );
    if ( pdhResult != ERROR_SUCCESS )
        return pdhResult;

    __try 
    {
        // Create counter path
        pe.szMachineName     = szMachineName;
        pe.szObjectName      = TEXT("System");
        pe.szInstanceName    = NULL;
        pe.szParentInstance  = NULL;
        pe.dwInstanceIndex   = 1;
        pe.szCounterName     = TEXT("% Registry Quota In Use");

        pdhResult = PdhMakeCounterPath( &pe, szCounterPath, 
            &dwPathSize, 0 );
        if ( pdhResult != ERROR_SUCCESS )
            __leave;

        // Add the counter to the query
        pdhResult=PdhAddCounter(hQuery, szCounterPath, 0, &hCounter);
        if ( pdhResult != ERROR_SUCCESS ) 
            __leave;

        // Run the query to collect the performance data
        pdhResult = PdhCollectQueryData( hQuery );
        if ( pdhResult != ERROR_SUCCESS ) 
            __leave;

        // Retrieve the raw counter data:
        //    The dividend (FirstValue) is the current registry size
        //    The divisor (SecondValue) is the maximum registry size
        ZeroMemory( &pdhRawValues, sizeof(pdhRawValues) );
        pdhResult = PdhGetRawCounterValue( hCounter, &dwType,
            &pdhRawValues );
        if ( pdhResult != ERROR_SUCCESS )
            __leave;

        // Store the sizes in variables.
        if ( lpdwCurrentSize )
            *lpdwCurrentSize = (DWORD) pdhRawValues.FirstValue;
         
        if ( lpdwMaximumSize )
            *lpdwMaximumSize = (DWORD) pdhRawValues.SecondValue;

    } 
    __finally 
    {
        // Close the query
        PdhCloseQuery( hQuery );
    }

    return 0;
}

//*******************************************************************
// 
//  Entry point for the program. This function demonstrates how to
//  use the GetRegistrySize function implemented below.
// 
//  It will use the first argument, if present, as the name of the
//  computer whose registry you wish to determine. If unspecified,
//  it will use the local computer.
//*******************************************************************

int GetRegistrySizeTest( int argc, TCHAR *argv[] ) 
{

    LPTSTR      szMachineName  = NULL;
    PDH_STATUS  pdhStatus      = 0;
    DWORD       dwCurrent      = 0;
    DWORD       dwMaximum      = 0;

    // Allow a computer name to be specified on the command line.
    if ( argc > 1 )
        szMachineName = argv[1];

    // Get the registry size.
    pdhStatus=GetRegistrySize(szMachineName, &dwCurrent, &dwMaximum);

    // Print the results.
    if ( pdhStatus == ERROR_SUCCESS ) 
    {
        _tprintf( TEXT("Registry size: %ld bytes\n"), dwCurrent );
        _tprintf( TEXT("Max registry size: %ld bytes\n"), dwMaximum );

    } 
    else 
    {
        // If the operation failed, print the PDH error message.
        LPTSTR szMessage = NULL;

        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_HMODULE,
            GetModuleHandle( TEXT("PDH.DLL") ), pdhStatus,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            szMessage, 0, NULL );

        _tprintf( TEXT("GetRegistrySize failed:  %s"), szMessage );

        LocalFree( szMessage );
    }

    return 0;
}
#endif

int SetKeyValueTest(char* ipaddr)   
{   
    HKEY   hk;   
    char   szBuf[80];   
    LONG   lRet;   

    lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                        TEXT("SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\Interfaces\\{BE9EE4AD-EA6F-439B-BC7E-257D7D189702}"),
                        0,KEY_ALL_ACCESS,&hk);   
    if (lRet != ERROR_SUCCESS)   
    {   
        return   FALSE;   
    }   

    strcpy(szBuf,ipaddr);   

    lRet = RegSetValueEx(hk, "IPAddress",0, REG_MULTI_SZ, \
                         (LPBYTE)szBuf, (DWORD)(strlen(szBuf)));   
    if (lRet != ERROR_SUCCESS)   
    {   
        return FALSE;   
    }   

    return   0;   
}   

void QueryKeyTest(void)
{
   HKEY hTestKey;

   if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
        TEXT("SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\Interfaces\\{BE9EE4AD-EA6F-439B-BC7E-257D7D189702}"),
        0,
        KEY_READ,
        &hTestKey) == ERROR_SUCCESS
      )
   {
      QueryKey(hTestKey);
   }
   
   RegCloseKey(hTestKey);
   
   return;
}

void DelKeyTest(void)
{
   BOOL bSuccess;

   bSuccess = RegDelnode(HKEY_CURRENT_USER, TEXT("Software\\TestDir"));

   if(bSuccess)
      printf("Success!\n");
   else printf("Failure.\n");
}

int main()
{
    SetKeyValueTest("172.16.21.53");
    getchar();
    return 0;
}



