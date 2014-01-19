#ifndef READCONFIG_H_INCLUDED
#define READCONFIG_H_INCLUDED
#include <fstream>
#include <string>

class ConfigClass
{
    private:
        std::string fName;
    public:
        std::string GetValue(int layer, std::string fieldName, int index = -1);
        ConfigClass(const char* fileName);

};


#endif // READCONFIG_H_INCLUDED
