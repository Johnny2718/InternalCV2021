#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>
#include "serial/include/serial/serial.h"
using namespace std;

int main(){
    string port("/dev/ttyl");
    serial::Serial my_serial(port);

    if(!my_serial.isOpen()){
        cout << "Not opened" << endl;
        return 0;
    }

    my_serial.write("1");
    return 0;
}