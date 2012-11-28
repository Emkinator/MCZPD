#ifndef READCONFIG_H_INCLUDED
#define READCONFIG_H_INCLUDED
#include <fstream>
#include <string>
namespace MC
{
    class ConfigClass
    {
        private:
            std::string fName;
        public:
            std::string GetValue(std::string fieldName);
            ConfigClass(const char* fileName);

    };
}


#endif // READCONFIG_H_INCLUDED
