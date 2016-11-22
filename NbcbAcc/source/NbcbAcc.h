#ifndef __NBCB_ACC__
#define __NBCB_ACC__

#include <iostream>
#include <string>
#include <list>

using namespace std;

#define MAX_VALUE_LENGTH 255
#define DEFAULT_FATAL_FILE "fatal.csv"
#define DEFAULT_SOFT_KEY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
#define NBCB_ACC_FULL_REG_KEY "SOFTWARE\\NbcbAcc"
#define NBCB_ACC_REG_KEY "NbcbAcc"
#define SOFTWARE_REG_KEY "SOFTWARE"

#define DEFAULT_ACC_NAME "fatal"
#define DEFAULT_NAME "NbcbAcc"
#define DEFAULT_VER  "1.0"

#define CHECK_DEFAULT 0
#define CHECK_VERSION 1

#define PRINT_ALL_INSTALL_SOFT    0x0001
#define PRINT_DEFINED_FATAL_SOFT  0x0002
#define PRINT_FATAL_SOFT          0x0004

#define OUT_TYPE_LOG              0x0001
#define OUT_TYPE_CONSOLE          0x0002

#define LOGFILE      "C:/WINDOWS/Temp/NbcbAccInfo.html"


typedef struct {
        string DisplayName;       //software name
        string DisplayVersion;    //software version
        string Publisher;         
        string InstallLocation;
        string UninstallString;  
} UninstallInfo_S;

void ToLowCase(string &str);

class TNbcbAcc
{
    private:
        list<UninstallInfo_S> m_InstalledSoftList;
        list<UninstallInfo_S> m_InstalledFatalSoftList;
        list<UninstallInfo_S> m_DefinedFatalsoftList;
                
        int GetFatalsoftInfo(const char* filename);
        int GetUninstallInfo(void);        
        int SetRegValue(const char* name, const char *value);
        int GetRegValue(const char* name, char* value, DWORD *valuelen);         
        void AddtoFatalList(UninstallInfo_S &info);
        void InitLogFile(const char* filename);
        void WriteLogFile(const char* filename, const char* data, unsigned int datalen);
        void BuildSoftInfoHtml(const char* filename, string title, list<UninstallInfo_S> softwarelist, unsigned int outtype); 
        void ReleaseList(list<UninstallInfo_S> softlist);
        
    public: 
        TNbcbAcc(void);
        ~TNbcbAcc(void);   
        void GetAllSoftInfo(unsigned int flag, unsigned int outtype);
        int CheckSoftware(int flag);
};

#endif
