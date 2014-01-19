#ifndef READCONFIG_H_INCLUDED
#define READCONFIG_H_INCLUDED
#include <fstream>
#include <string>

using namespace std;

class ConfigClass
{
    private:
        string fName;
    public:
        string GetValue(int layer, string fieldName, int index = -1);
        ConfigClass(const char* fileName);

};


#endif // READCONFIG_H_INCLUDED
