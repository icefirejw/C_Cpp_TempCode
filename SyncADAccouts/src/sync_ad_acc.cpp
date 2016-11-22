#include <fstream>
#include <activeds.h>
#include "sync_ad_acc.h"

SyncAdAcc::SyncAdAcc(string adserver, string username, string password)
{
    m_server = "LDAP://" + adserver;
    m_username = username;
    m_password = password;
}

void SyncAdAcc::initlogfile(char* prefix)
{
    fstream fHd;
        
    if (NULL == prefix)
        return;
    
    m_filename = prefix;
    m_filename += "log"; //date??
    
    fHd.open(m_filename.c_str(), ios::out | ios::binary | ios::trunc);
    if (!fHd.good())
        cout << "Create file " << m_filename << "Error!!" << endl;
    
    fHd.close();
    return;

}

void SyncAdAcc::Log(string info, string funcname, int errno)
{
    fstream fHd;
    string tmpbuf;
    
    fHd.open(m_filename.c_str(), ios::binary | ios::app | ios::out);
    if (!fHd.good())
        cout << "WriteLogFile open file " << m_filename << "Error!!" << endl;
    
    if (0 == errno)
        tmpbuf = info;
    else
    {
        char tmp[32];
        sprintf(tmp, "%d", errno);
        
        tmpbuf = info + ", " + funcname + ", ERROR: ";
        tmpbuf.append(tmp);        
    }
    
    //cout << data;
    fHd.write(tmpbuf.c_str(), tmpbuf.size());
    fHd.flush(); 
    fHd.close();
    return;
}

int SyncAdAcc::getusers(list<string> users);
{
    HRESULT hr;
    IDirectorySearch *pSearch;
    
    // Inititalization    
    CoInitialize(NULL);
    
    // Bind to the base search object    
    hr = ADsOpenObject(CComBSTR(m_server),  //??
                        CComBSTR(m_username),  //??
                        CComBSTR(m_password),  //??
                        ADS_SECURE_AUTHENTICATION,
                        IID_IDirectorySearch,
                        (void**)&pSearch);
    
    
    ADS_SEARCHPREF_INFO prefInfo[1];
    prefInfo[0].dwSearchPref = ADS_SEARCHPREF_SEARCH_SCOPE;
    prefInfo[0].vValue.dwType = ADSTYPE_INTEGER;
    prefInfo[0].vValue.Integer = 100;
    hr = pSearch->SetSearchPreference( prefInfo, 1);
    if (FAILED(hr))
    {
        pSearch->Release();
        //return hr;
    }
    // Prepare for attributes to be returned
    
    LPWSTR pszSearchFilter = L"(&(objectClass=user)(objectCategory=person))";
    //LPWSTR pszAttr[] = {L"sAMAccountName"};
    LPWSTR pszAttr[] = {L"name"};
    ADS_SEARCH_HANDLE hSearch;
    
    LPOLESTR pszColumn = NULL;
    int iCount = 0;
    DWORD x = 0;
    
    DWORD dwCount= sizeof(pszAttr)/sizeof(LPWSTR);
    
    // Search
    hr = pSearch->ExecuteSearch(
        pszSearchFilter,
        pszAttr,
        dwCount,
        &hSearch
        );
    // Enumerate the search result
    
    ADS_SEARCH_COLUMN col;
    
    if ( SUCCEEDED(hr) )
    {   
    // Call IDirectorySearch::GetNextRow() to retrieve the next row of data
    hr = pSearch->GetFirstRow(hSearch);
    if (SUCCEEDED(hr))
    {
    while( hr != S_ADS_NOMORE_ROWS )
    {
    //Keep track of count.
    iCount++;
    // loop through the array of passed column names,
    // print the data for each column
    
    while( pSearch->GetNextColumnName( hSearch, &pszColumn ) != S_ADS_NOMORE_COLUMNS )
    {
    hr = pSearch->GetColumn( hSearch, pszColumn, &col );
    if ( SUCCEEDED(hr) )
    {
    for (x = 0; x< col.dwNumValues; x++)
    wprintf(L"%s: %s  ",col.pszAttrName,col.pADsValues[x].CaseIgnoreString);
    }
    
    pSearch->FreeColumn( &col );
    }
    FreeADsMem( pszColumn );
    //Get the next row
    hr = pSearch->GetNextRow( hSearch);
    wprintf(L"\n");
    }
    }
    }
    
    CoUninitialize();
}
