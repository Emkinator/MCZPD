#include "ReadConfig.h"
#include <fstream>
#include <string.h>
#include <sstream>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

using namespace MC;

ConfigClass::ConfigClass(const char* fileName)
{
    fName = fileName;
}

std::string ConfigClass::GetValue(int layer, std::string fieldName)
{
    std::ifstream file;
    file.open(fName.c_str());
    std::string line;
    std::string output;
    std::string label = SSTR(layer);
    std::size_t pos;
    std::size_t comment;
    bool found = false;
    bool labels_matter = false;
    while(file.good()) {
        getline(file, line);

        comment = line.find("/");
        if(comment > 0)
            line = line.substr(0, comment - 1);
        else if(comment == 0)
            continue;

        if(line[0] == '[') {
            pos = line.find(']');
            if(pos > 1) {
                if(found) //passed over the needed label already
                    break;
                labels_matter = true;
                output = line.substr(1, pos - 1);
                if(strcmp(label.c_str(), output.c_str()) == 0) //found label
                    found = true;
            }
        }
        if(found || !labels_matter) {
            pos = line.find(fieldName.c_str());
            if(pos == 0) {
                pos = line.find("=");
                output = line.substr(pos+2, line.size()-(pos+1));
                break;
            }
        }

    }
    file.close();
    return output;
}
