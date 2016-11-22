#include <cstdlib>
#include <iostream>
#include <fstream>
#include <list>
#include "macro_def.h"

#ifdef CONF_ANLYZ
#include "anlyz_conf.h"

using namespace std;

char* conf_def[] = {
    "NULL",
    "IP",
    "MASK",
    "GW",
    "NS1",
    "NS2"
};

list<ADP_CONF_INFO_S*> adpConfInfoList;

int init_conf(int conf_type, char* filename)
{
    fstream fHandle;
    
    if (NULL == filename) 
        return -1;

    fHandle.open(filename, ios::in | ios::binary);

    if (!fHandle.good()) { //failed to open file with!
         fHandle.open(filename, ios::out | ios::binary);
         if (!fHandle.good()) return -1;
    }

    fHandle.close();

    return 0;
}

void finish_conf(int conf_type)
{
    switch (conf_type)
    {
        default:
            break;
            
        case ANALYZ_NETWORK:
            ADP_CONF_INFO_S* tmp = *adpConfInfoList.begin();
            
            while (tmp)
            {
                delete tmp;
                tmp = *adpConfInfoList.begin();
                adpConfInfoList.pop_front();
            }
    }
    
    return;
}

int InitConfType(int anlz_type, const char* value)
{
    if (NULL == value) 
        return -1;
    
    switch (anlz_type){
        case ANALYZ_NETWORK:
            int infoSize = sizeof(ADP_CONF_INFO_S);
            ADP_CONF_INFO_S* adpConInfo = (ADP_CONF_INFO_S*)new char[infoSize];
            if (NULL == adpConInfo) 
                return -1;

            memset((void*)adpConInfo, 0, sizeof(ADP_CONF_INFO_S));
            strcpy(adpConInfo->name, value);
            adpConfInfoList.push_back(adpConInfo);
            break;
    }
    
    return 0;
}

int stringAnalyse(const char* inStr, unsigned int len, char* outStr, int* outLen)
{
    
    return -1;
}

int confAnalyse(int anlz_type, const char* inStr, unsigned int len)
{
    int flag = 0;
    char value[VALUE_LEN];
    int retlen = VALUE_LEN;
    int ret = 0;
    
    flag = stringAnalyse(inStr, len, value, &retlen);

    switch (flag) {
        case UNDEF_TYPE:
            ret = InitConfType(anlz_type, inStr); 
            break;
            
        case IP_ADDRESS:
            
            break;
        case IP_MASK:
            break;
        case IP_GATEWAY:
            break;
        case IP_NS1:
            break;
        case IP_NS2:
            break;
        default:
            flag = -1;
            break;            
    }

    return ret;
}
/*
ios::in   = 0x01,      // rd mode, if not exists create file   
ios::out = 0x02,       // wr mode, if not exists create file
ios::ate   = 0x04,     // open file and make the file pf to end of file
ios::app　     = 0x08, //append data to end of file
ios::trunc　　 = 0x10, //cut the file 2 zero size, while open the file
ios::nocreate　= 0x20,   // open file,  without creating. report ERROR while Exists
ios::noreplace = 0x40,   // without creating. report ERROR while Exists
ios::binary　　= 0x80    // binary mode
*/

int fileAnalyse(const char* filename, const int anlyz_type)
{
    fstream fHandle;

    unsigned nLine = 1;
    char* lineBuf;
    char* tmp;
    char ch;
    char prevch;
    int  ret = 0;
    
    if (NULL == filename) 
        return -1;
    
    fHandle.open(filename, ios::in | ios::binary);

    if (!fHandle.good()) { //failed to open file !
        cout<<"Failed to open file: "<<filename<<endl;
        return OPEN_FILE_ERROR;
    }
    
    lineBuf = new char[LINE_BUF_LEN+1];

    if (NULL == lineBuf) {
        fHandle.close();
        return -1;
    }
    
    tmp = lineBuf;  
    while (ch=fHandle.get()){
        if (' ' == ch ) continue; //ignore space key
        
        *tmp=ch;
        if ('\n' == ch || '\r' == ch || EOF == ch){
           
            if (tmp-lineBuf > 0) { //Ignore the '\r\n'  and blank line
                *tmp = '\0';       //ignore ENTER key
                if ('#' == *(tmp-1)) *(--tmp) = '\0'; // begin with '#' is description
                /* analyse the string */
                if (tmp-lineBuf>0) {
                    #ifdef GLOBAL_DEBUG
                    cout<<"LINE "<<nLine<<": "<<lineBuf<<endl;
                    #endif
                    ret = confAnalyse(anlyz_type, lineBuf, tmp-lineBuf );   
                    if (ret < 0){ // analyse value error!
                       cout<<"Configure file ERROR at line:"<<nLine<<endl;
                       break;
                    }
                }
                
                tmp = lineBuf;
            }
            
            if (prevch != '\r') //windows ENTER is '\r\n'
               nLine++;
               
            prevch = ch;
            if (EOF == ch) 
                break;
            else
                continue;
        } 
        if (tmp-lineBuf>0 && '#' == *(tmp-1)) continue; // begin with '#' is description
        tmp++;
        
    }
    
    delete lineBuf;
    
    fHandle.close();
    
    return ret;
}

#endif

