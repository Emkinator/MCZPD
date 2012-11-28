#ifndef WRITETOCVS_H_INCLUDED
#define WRITETOCVS_H_INCLUDED
#include <fstream>
#include <string>
//A .cvs file consists of elements seperated by commas
//A single row makes a single row in excel
//An endl makes a new row
namespace MC
{
    template< typename t_data >
    void WriteCVS(t_data *a, const char* fileName, int nrows, int ncols)
    {
        std::ofstream file;
        std::string fullName(fileName);
        fullName += ".cvs";
        file.open(fullName.c_str());

        for(int row = 0; row < nrows; row++)
        {
            for(int col = 0; col < ncols; col++)
            {
                file << *a;
                if(col < ncols-1)
                    file << ", ";
                a++;

            }
            file << std::endl;
        }
        file.close();
    }
}

#endif // WRITETOCVS_H_INCLUDED
