#include "ReadConfigs.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <sstream>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

using namespace std;

ConfigClass::ConfigClass(const char* fileName)
{
    fName = fileName;
}

string ConfigClass::GetValue(int layer, string fieldName, int index)
{
    ifstream file;
    file.open(fName.c_str());
    string line;
    string output;
    string label = SSTR(layer);
    size_t pos;
    size_t comment;
    bool found = false;
    bool labels_matter = false;
    bool in_array = false;
    bool right_array = false;
    int array_index = 0;
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
        else if(line[0] == '{') {
            in_array = true;
        }
        else if(line[0] == '}') {
            if(right_array) break;
            in_array = false;
        }

        if(in_array) {
            if(right_array) {
                if(index == array_index) {
                    output = line.substr(1, line.size()-1);
                    break;
                }
                array_index++;
            }
        }
        else if(found || !labels_matter) {
            pos = line.find((fieldName + " ").c_str());
            if(pos == 0) {
                if(index > -1)
                    right_array = true;
                else {
                    pos = line.find("=");
                    output = line.substr(pos+2, line.size()-(pos+1));
                    break;
                }
            }
        }
    }
    file.close();
    return output;
}
