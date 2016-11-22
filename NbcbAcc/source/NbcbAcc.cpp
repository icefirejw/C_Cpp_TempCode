// NbcbAcc.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <windows.h>
#include "NbcbAcc.h"

int TNbcbAcc::GetFatalsoftInfo(const char* filename)
{
    fstream  infile;
    string msg;
    char tmpbuff[1024];
    
    if (NULL == filename)
        return -1;
    
    infile.open(filename, ios::in | ios::binary);
    if (!infile.good())
        return -2;
    
    if (!infile.is_open())
        return -3;
    
    while (infile.good() && !infile.eof()) {
        memset(tmpbuff,0,1024);
        infile.getline(tmpbuff, 1024);   
        
        msg = tmpbuff;
        //cout << tmpbuff <<endl;
        
        if (msg.length() < 4) //softname and version at least 4 charactors, such as a,1\n
            continue;
            
        UninstallInfo_S &tmp = *new UninstallInfo_S;
        char* p = NULL;
        
        for (p = &tmpbuff[0]; *p!='\0' && p-tmpbuff<1024; p++){
            if (*p == ',' || *p == ';') {
                *p = '\0';                
                tmp.DisplayName += tmpbuff;
                tmp.DisplayVersion += (p+1);
                tmp.Publisher += "&nbsp;";
                tmp.InstallLocation += "&nbsp;";         
                tmp.UninstallString += "&nbsp;"; 
            }
            if (*p == '\n' || *p == '\r') {
                *p = '\0';
            }
        }
        m_DefinedFatalsoftList.push_back(tmp);
    }
    infile.close();
    
    return 0;
}

void TNbcbAcc::InitLogFile(const char* filename)
{
    fstream fHd;
    
    if (NULL == filename)
        return;
    
    fHd.open(filename, ios::out | ios::binary | ios::trunc);
    if (!fHd.good())
        cout << "Create file " << filename << "Error!!" << endl;
    
    fHd.close();
    return;
}

void TNbcbAcc::WriteLogFile(const char* filename, const char* data, unsigned int datalen)
{
    fstream fHd;
    if (NULL == filename)
        return;
    
    fHd.open(filename, ios::binary | ios::app | ios::out);
    if (!fHd.good())
        cout << "WriteLogFile open file " << filename << "Error!!" << endl;
    
    //cout << data;
    fHd.write(data, datalen);
    fHd.flush(); 
    fHd.close();
    return;
}
void TNbcbAcc::BuildSoftInfoHtml(const char* filename, string title, list<UninstallInfo_S> softwarelist, unsigned int outtype)
{
        list<UninstallInfo_S>::iterator it;        
                       
        string tmpbuf = "<div align='center' bgcolor='#33ccee'><h1>" + title + "</h1></div>\r\n";
        tmpbuf += "<table width='90%' cellspacing='0' cellpadding='0' border='1'>\r\n";
        tmpbuf += "<thead><th>DisplayName</th><th>Version</th><th>Publisher</th><th>Location</th><th>Uninstall</th></thead>\r\n";
        for (it=softwarelist.begin(); it!=softwarelist.end(); it++) {
    			tmpbuf += "<tr><td>" + it->DisplayName + "</td>";
                tmpbuf += "<td>" + it->DisplayVersion + "</td>";
                tmpbuf += "<td>" + it->Publisher + "</td>";
                tmpbuf += "<td>" + it->InstallLocation + "</td>";
                tmpbuf += "<td>" + it->UninstallString + "</td></tr>\r\n";
        }
        
        tmpbuf += "</table>";
        
        if ((OUT_TYPE_CONSOLE & outtype) > 0) //print to console 
            cout << tmpbuf << endl;            
        if ((OUT_TYPE_LOG & outtype) > 0)  //write log file
            WriteLogFile(filename, (const char*)tmpbuf.c_str(), (unsigned int)tmpbuf.length());  
}

void TNbcbAcc::GetAllSoftInfo(unsigned int flag, unsigned int outtype)
{
    string tmpbuf; 
    
    if ((OUT_TYPE_LOG & outtype) > 0)  //write log file
    {
        tmpbuf = "<html><head><title>NbcbAccInfo</title></head>\r\n<body>\r\n";
        WriteLogFile(LOGFILE, (const char*)tmpbuf.c_str(), (unsigned int)tmpbuf.length()); 
    } 
     
    if ((flag & PRINT_FATAL_SOFT) > 0) {
        BuildSoftInfoHtml(LOGFILE, "安装的非法软件", m_InstalledFatalSoftList, outtype); 
    }
        
    if ((flag & PRINT_DEFINED_FATAL_SOFT) > 0) {
        if (m_DefinedFatalsoftList.empty())
            GetFatalsoftInfo(DEFAULT_FATAL_FILE);
        BuildSoftInfoHtml(LOGFILE, "不能安装的软件", m_DefinedFatalsoftList, outtype); 
    }
    
    if ((flag & (PRINT_ALL_INSTALL_SOFT)) > 0) {
        if (m_InstalledSoftList.empty())
            GetUninstallInfo();
        BuildSoftInfoHtml(LOGFILE, "已安装的软件", m_InstalledSoftList, outtype); 
    }
    
    if ((OUT_TYPE_LOG & outtype) > 0)  //write log file
    {
        tmpbuf = "</body></html>\r\n";
        WriteLogFile(LOGFILE, (const char*)tmpbuf.c_str(), (unsigned int)tmpbuf.length()); 
    } 
}

int TNbcbAcc::GetUninstallInfo()
{
    HKEY hKey;
    string szMainkey = DEFAULT_SOFT_KEY;
    DWORD subNumbers = 0;
    DWORD MaxKeyNameLen = 0;
	
    int ret;
    
    ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		(LPCSTR)szMainkey.c_str(),
        0,
        KEY_READ,
        &hKey);        
    if( ret != ERROR_SUCCESS){
		cout << "RegOpenKeyEx return error: " << ret <<endl;
		return -1;
	}
    
    ret = RegQueryInfoKey(hKey,NULL,NULL,NULL,&subNumbers,&MaxKeyNameLen,NULL,NULL,NULL,NULL,NULL,NULL);
    if (ret != ERROR_SUCCESS) {
		cout << "RegQueryInfoKey return error: " << ret <<endl;
		RegCloseKey(hKey);
		return -2;        
    }
    
    if (subNumbers > 0)
    {
        TCHAR* szKeyname = new TCHAR[MaxKeyNameLen+1];
		if (NULL == szKeyname) {
			RegCloseKey(hKey);
			return -3;    
		}
		
		int keyCount = 0; //debug
        for (DWORD i=0; i<subNumbers; i++)
        {
            DWORD keySize = MaxKeyNameLen+1;  //must be the max key name length
            
            ret = RegEnumKeyEx(hKey,i,(LPSTR)szKeyname,&keySize,NULL,NULL,NULL,NULL);
            if (ret != ERROR_SUCCESS) {                
                cout << "RegEnumKeyEx return error: " << ret <<endl;
		        continue;
            }
            
            if ('K' == szKeyname[0] && 'B' == szKeyname[1])
                continue;
                
            string subKeyName = szMainkey + "\\" + szKeyname;
			//cout << subKeyName <<endl;
            
            HKEY hSubKey;
            ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                    (LPCSTR)subKeyName.c_str(),
                    0,
                    KEY_READ,
                    &hSubKey); 
            if (ret != ERROR_SUCCESS) {
        		cout << "RegOpenKeyEx subkey return error: " << ret <<endl;
        		continue;        
            }
            
            UninstallInfo_S &tempSoftInfo = *new UninstallInfo_S;
            
			DWORD ValueDataLen = MAX_VALUE_LENGTH*sizeof(TCHAR);
			char szValueData[MAX_VALUE_LENGTH*sizeof(TCHAR)];

            ret = RegQueryValueEx(hSubKey, "DisplayName", NULL, NULL, (LPBYTE)&szValueData[0], &ValueDataLen);
            if (ret != ERROR_SUCCESS || ValueDataLen == 0){
                continue;
            }
			tempSoftInfo.DisplayName += (TCHAR*)szValueData;

            ValueDataLen = MAX_VALUE_LENGTH + 1;
            ret = RegQueryValueEx(hSubKey, "DisplayVersion", NULL, NULL, (LPBYTE)&szValueData[0], &ValueDataLen);
            if (ret == ERROR_SUCCESS && ValueDataLen > 0)
                tempSoftInfo.DisplayVersion += (TCHAR*)szValueData;
            else
                tempSoftInfo.DisplayVersion += "&nbsp;";
            
            ValueDataLen = MAX_VALUE_LENGTH + 1;
            ret = RegQueryValueEx(hSubKey, "Publisher", NULL, NULL, (LPBYTE)&szValueData[0], &ValueDataLen);
            if (ret == ERROR_SUCCESS && ValueDataLen > 0)
                tempSoftInfo.Publisher += (TCHAR*)szValueData;
            else
                tempSoftInfo.Publisher += "&nbsp;";
            
            ValueDataLen = MAX_VALUE_LENGTH + 1;
            ret = RegQueryValueEx(hSubKey, "InstallLocation", NULL, NULL, (LPBYTE)&szValueData[0], &ValueDataLen);
            if (ret == ERROR_SUCCESS && ValueDataLen > 0)
                tempSoftInfo.InstallLocation += (TCHAR*)szValueData; 
            else
                tempSoftInfo.InstallLocation += "&nbsp;";
                
            ValueDataLen = MAX_VALUE_LENGTH + 1;
            ret = RegQueryValueEx(hSubKey, "UninstallString", NULL, NULL,(LPBYTE)&szValueData[0], &ValueDataLen);
            if (ret == ERROR_SUCCESS && ValueDataLen > 0)
                tempSoftInfo.UninstallString += (TCHAR*)szValueData; 
            else 
                tempSoftInfo.UninstallString += "&nbsp;";
                
            m_InstalledSoftList.push_back(tempSoftInfo);
			keyCount++;//debug
            RegCloseKey(hSubKey);
        }
        //cout << "all key count: " << keyCount <<endl;
        delete szKeyname;
        
    }
    
    RegCloseKey(hKey);
    return 0;
}

int TNbcbAcc::SetRegValue(const char* name, const char* value)
{    
    HKEY hKey;
    HKEY hNbcbAccKey;
    DWORD dw;
    int ret;
    
    ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,	(LPCSTR)SOFTWARE_REG_KEY,
        0,  KEY_READ | KEY_WRITE,   &hKey);       
    if (ret != ERROR_SUCCESS)
        return -1;
           
    ret = RegCreateKeyEx(hKey, NBCB_ACC_REG_KEY, 0, REG_NONE, REG_OPTION_NON_VOLATILE, 
                        KEY_READ | KEY_WRITE, NULL, &hNbcbAccKey, &dw);                        
    RegCloseKey(hKey); //close the software key
    if (ret != ERROR_SUCCESS)
        return -2;
    
    ret = RegSetValueEx(hNbcbAccKey, name, 0, REG_SZ, (BYTE*)value, strlen(value));
    RegCloseKey(hNbcbAccKey);
    if (ret != ERROR_SUCCESS)
        return -3;
    
    return 0;
}

int TNbcbAcc::GetRegValue(const char* name, char* value, DWORD *valuelen)
{
    HKEY hNbcbAccKey;
    int ret;
    
    ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,	(LPCSTR)NBCB_ACC_FULL_REG_KEY,
        0,  KEY_READ | KEY_WRITE,   &hNbcbAccKey);       
        
    if (ret != ERROR_SUCCESS)
        return -1;
        
    ret = RegQueryValueEx(hNbcbAccKey, name, NULL, NULL, (LPBYTE)value, valuelen);
    RegCloseKey(hNbcbAccKey);
    
    if (ret != ERROR_SUCCESS)
        return -2;
    
    return 0;
}


int TNbcbAcc::CheckSoftware(int flag)
{
    list<UninstallInfo_S>::iterator it;
    list<UninstallInfo_S>::iterator fit;
    bool isFatal = false;
    int  ret;
    
    if (m_DefinedFatalsoftList.empty()) {
        ret = GetFatalsoftInfo(DEFAULT_FATAL_FILE);
        if (ret != 0) {
            cout << "GetFatalsoftInfo Error:" << ret << endl;
            return -1;
        }
    }
    
    if (m_InstalledSoftList.empty()) {
        ret = GetUninstallInfo();
        if (ret != 0) {
            cout << "GetUninstallInfo Error:" << ret << endl;
            return -1;            
        }        
    }
        
    for (it=m_InstalledSoftList.begin(); it!=m_InstalledSoftList.end(); it++) {
        string inssoft = it->DisplayName;
        ToLowCase(inssoft);
        string inssoftver = it->DisplayVersion;
        ToLowCase(inssoftver);
        
        for (fit=m_DefinedFatalsoftList.begin(); fit!=m_DefinedFatalsoftList.end(); fit++) {
            string fatalsoft = fit->DisplayName;
            ToLowCase(fatalsoft);
            if (inssoft.find(fatalsoft) != string::npos) {
                if (CHECK_VERSION == flag) {
                    string fatalsoftver = fit->DisplayVersion;
                    ToLowCase(fatalsoftver);
                    if (inssoftver.find(fatalsoftver) != string::npos) {
                        isFatal = true;  
                        AddtoFatalList(*it);
                    }
                } else {
                    isFatal = true;
                    AddtoFatalList(*it);
                }          
            } 
        }
    }
    
    if (isFatal) {
        SetRegValue(DEFAULT_ACC_NAME, "1");
        /*
        string msg = "发现安装非法软件,请及时卸载,具体查看以下文件\r\n\r\n";
        msg +=  LOGFILE; 
        msg += "\r\n"; 
        MessageBox(NULL, msg.c_str() , "ERROR",MB_OK);        
        */
    }        
    else {
        SetRegValue(DEFAULT_ACC_NAME, "0");
        //cout << "no fatal software found" << endl;
    }   
}

void TNbcbAcc::AddtoFatalList(UninstallInfo_S &info)
{
    m_InstalledFatalSoftList.push_back(info);
}

void TNbcbAcc::ReleaseList(list<UninstallInfo_S> softlist)
{
    list<UninstallInfo_S>::iterator usit;
    list<UninstallInfo_S>::iterator usittmp;
    
    usit = softlist.begin(); 
    while(usit != softlist.end()) {
        usit->DisplayName.~string();
        usit->DisplayVersion.~string();
        usit->Publisher.~string();
        usit->InstallLocation.~string();
        usit->UninstallString.~string();
        usittmp = usit;
        delete &usittmp;
        usit++;
    }
}

TNbcbAcc::TNbcbAcc(void)
{
    int ret;
    DWORD dw = 32;
    char tmp[32] ;
    
    ret = GetRegValue(DEFAULT_ACC_NAME, &tmp[0], &dw);
    if (ret < 0)
    {
        SetRegValue(DEFAULT_ACC_NAME, "0");
    }
    SetRegValue("DisplayName", DEFAULT_NAME);
    SetRegValue("DisplayVersion", DEFAULT_VER);
    
    InitLogFile(LOGFILE);   
}

TNbcbAcc::~TNbcbAcc(void)
{
    ReleaseList(m_InstalledSoftList);
    ReleaseList(m_InstalledFatalSoftList);
    ReleaseList(m_DefinedFatalsoftList);
}

void ToLowCase(string &str)
{
    for (int i=0; i<str.length(); i++)
        if ((str[i]>='A') && (str[i]<='Z'))
            str[i] += ('a' - 'A');
}

int main(int argc, char* argv[])
{
    class TNbcbAcc nbcbacc;
    int ret;
    
    nbcbacc.CheckSoftware(CHECK_DEFAULT);
    nbcbacc.GetAllSoftInfo(PRINT_DEFINED_FATAL_SOFT|PRINT_FATAL_SOFT|PRINT_ALL_INSTALL_SOFT, OUT_TYPE_LOG);
	return 0;
}

