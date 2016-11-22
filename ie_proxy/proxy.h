#ifndef __PROXY__
#define __PROXY__

#ifdef __CR_PAC__
#define PAC_FILENAME "c:/proxy.pac"
#define FUNC_HEAD "function FindProxyForURL(url,host) \
                    \r\n{ \
                    \r\n    if (isPlainHostName(host)) return \"DIRECT\";\r\n"
#define FUNC_END  "    else return \"DIRECT\";\r\n}\r\n"
#define LINE_BUF_LEN 128
#define RT_DIRECT "return \"DIRECT\";"
#define RT_PROXY  "return \"PROXY 172.16.21.71:80\";"


typedef enum {
    IP_UNDEFINED = 0,
    IP_ADDR,
    IP_DOMAIN
};

typedef struct st_proxy_host {
    unsigned int type;
    char* ip_addr;
    char* mask;
}proxy_host_st;


extern proxy_host_st do_proxy[];
int buildPac(void);
#endif

#define AUTO_PROXY_KEY "Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Internet Settings"
int enableProxy(void);

#endif

    
