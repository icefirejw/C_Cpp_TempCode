#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <iphlpapi.h>

#include "macro_def.h"
#include "mod_ip.h"
#ifdef CONF_ANLYZ
#include "anlyz_conf.h"
#endif

using namespace std;

int main(int argc, char *argv[])
{
    ADP_INFO_LIST plOldInfoList;
    int ret;
    
    plOldInfoList.pHead = NULL;
    plOldInfoList.pTail = NULL;

    ret = GetLocalAdapterInfo(&plOldInfoList);
    if (ret < 0) return -1;
    
    PrintLocalAdpInfo(&plOldInfoList);
    SetDNS(&plOldInfoList);

	#ifdef GLOBAL_DEBUG
	system("PAUSE");;
	#endif
	
    return EXIT_SUCCESS;
}


