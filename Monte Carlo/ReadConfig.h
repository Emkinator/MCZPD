#ifndef READCONFIG_H_INCLUDED
#define READCONFIG_H_INCLUDED
#include <fstream>
#include <string>
#include <stdlib.h>

using namespace std;

class ConfigClass
{
    private:
        string fName;
    public:
        template<class T>
        void GetValue(T& value, string fieldName, int layer = 0, int index = -1);
        string ReadValue(string fieldName, int layer = 0, int index = -1);
        ConfigClass(const char* fileName);

};

template<class T>
void ConfigClass::GetValue(T& value, string fieldName, int layer, int index)
{
    double temp = (double)value;
    string ret = ReadValue(fieldName, layer, index);
    if(ret != "") {
        temp = atof(ret.c_str());
    }
    value = (T)temp;
}


#endif // READCONFIG_H_INCLUDED
