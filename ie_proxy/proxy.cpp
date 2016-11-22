#include <cstdlib>
#include <iostream>
#include <fstream>
#include <windows.h>

#include "proxy.h"

using namespace std;

#if __CR_PAC__
proxy_host_st do_proxy[] = { \
    {IP_ADDR,"9.99.0.0","255.255.0.0"}, \
    {IP_ADDR,"9.96.0.0","255.255.0.0"}, \
    {IP_ADDR,"9.24.0.0","255.255.0.0"}, \
    {IP_ADDR,"8.99.0.0","255.255.0.0"}, \
};

proxy_host_st dont_proxy[] = { \
};


char* first_if(int *flag)
{
    /*
    if (0 == *flag) {
        *flag = 1;
        return "    if";        
    }
    else
    */
        return "    else if";
}

int build_line(proxy_host_st host_st, char* rt_str, int *flag, char* tmp)
{
    switch(host_st.type){
        case IP_ADDR:
            sprintf(tmp,"%s (isInNet(host, \"%s\", \"%s\")) %s\r\n", \
                first_if(flag), host_st.ip_addr, host_st.mask, rt_str);            
            break;
        case IP_DOMAIN:
            sprintf(tmp,"%s ((dnsDomainIs(host,\"%s\")) %s\r\n", \
                first_if(flag), host_st.ip_addr, rt_str);
            break;
        default:
            break;
    }
    
    return 0;
}
    
int buildPac(void)
{
    int count;
    int i;
    char* tmp;
    fstream fHandle;
    int pac_init = 0;
    
    fHandle.open(PAC_FILENAME, ios::out | ios::binary);

    if (!fHandle.good()) { //failed to open file !
        cout<<"Failed to open file: "<<PAC_FILENAME<<endl;
        return -1;
    }
    

    //write the function head
    fHandle.write(FUNC_HEAD, strlen(FUNC_HEAD));
    
    tmp = new char[LINE_BUF_LEN+1];

    if (NULL == tmp) {
        fHandle.close();
        return -2;
    }

    //the url neednt proxy
    count = sizeof(dont_proxy)/sizeof(proxy_host_st);

    for (i=0; i<count; i++){
        build_line(dont_proxy[i], RT_DIRECT, &pac_init, tmp);
        fHandle.write(tmp, strlen(tmp));
    }

    //the url do need proxy
    count = sizeof(do_proxy)/sizeof(proxy_host_st);

    for (i=0; i<count; i++){
        build_line(do_proxy[i], RT_PROXY, &pac_init, tmp);
        fHandle.write(tmp, strlen(tmp));
    }
    
    delete tmp;

    //write the end of function
    fHandle.write(FUNC_END, strlen(FUNC_END));
        
    fHandle.close();

    return 0;
    
}
#endif

int enableProxy(void)
{
    HKEY hKey;
    char proxy_str[128];
    
    //[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Internet Settings]
    //[HKEY_USERS\S-1-5-21-1177238915-1647877149-2147093213-500\Software\Microsoft\Windows\CurrentVersion\Internet Settings]

    /* open the regedit */    
    if(RegOpenKeyEx(HKEY_CURRENT_USER, AUTO_PROXY_KEY,
                    0, KEY_WRITE, &hKey) != ERROR_SUCCESS)   
        return -1;

    /* set the value */
    //sprintf(proxy_str, "file://%s", PAC_FILENAME);
    
    sprintf(proxy_str, "http://proxy.nbcb.cn/proxy.pac");
    
    RegSetValueEx(hKey, "AutoConfigURL", 0, REG_SZ, (unsigned char*)proxy_str, strlen(proxy_str));

    RegCloseKey(hKey);
    
    return 0;
}


