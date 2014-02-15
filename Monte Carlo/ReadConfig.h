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
        void GetValue(int& value, int layer, string fieldName, int index = -1);
        void GetValue(long long int& value, int layer, string fieldName, int index = -1);
        void GetValue(double& value, int layer, string fieldName, int index = -1);
        ConfigClass(const char* fileName);

};


#endif // READCONFIG_H_INCLUDED
