#include <cstdlib>
#include <iostream>

#include "proxy.h"

using namespace std;



int main(int argc, char *argv[])
{
    int err_no;

    /*
    err_no = buildPac();
    if (0 != err_no) {
        cout<<"build pac file error:"<<err_no<<endl;
        system("PAUSE");
        return -1;
    }
    */
    if (0 != enableProxy()) {
        cout<<"enable proxy error"<<endl;
        system("PAUSE");
    }
    return EXIT_SUCCESS;
}



