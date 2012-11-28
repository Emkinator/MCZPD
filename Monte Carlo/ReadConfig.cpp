#include "ReadConfig.h"
#include <fstream>

using namespace MC;

ConfigClass::ConfigClass(const char* fileName)
{
    fName = fileName;
}

std::string ConfigClass::GetValue(std::string fieldName)
{
    std::ifstream file;
    file.open(fName.c_str());
    std::string line;
    std::string output;
    std::size_t pos;
    while(file.good())
    {
        getline(file, line);
        if(line[0] == '/')
            continue;
        pos = line.find(fieldName.c_str());
        if(pos==0)
        {
            pos = line.find("=");
            output = line.substr(pos+2,line.size()-(pos+1));
            break;
        }

    }
    file.close();
    return output;
}
