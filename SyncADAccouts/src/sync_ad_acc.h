#ifndef SYNC_AD_ACC_H
#define SYNC_AD_ACC_H

#include <iostream>
#include <list>

using namespace std;

class SyncAdAcc
{
    private:
        string m_server;
        string m_username;
        string m_password;
        string m_filename;
        
        void initlogfile(char* filename);
        
    public:
        SyncAdAcc(string adserver, string username, string password);
        void Log(string info, string funcname, int errno);
        int getusers(list<string> users);
        int dosharefords(string path, list<string> users);     
};


#endif
