#include <boost/algorithm/string.hpp>
#include "CustomHeader.h"

CustomHeader::CustomHeader(char* header) 
{
    // Set modulus first
    char* bytes = new char[4];
    for (int i = 0; i < 4; i++)
        bytes[i] = header[i];

    _modulus = *reinterpret_cast<int*>(bytes);

    // Set extension
    char* ext = new char[4];
    for (int y = 0, i = 4; i < 8; i++, y++)
        ext[y] = header[i];

    _extension = string(ext);
    boost::algorithm::trim(_extension);

    // Set type
    _type = header[8];

    delete[] bytes;
    delete[] ext;
}

CustomHeader::CustomHeader() 
{
    // Initialize the variables
    return;
}

int CustomHeader::modulus() 
{
    return this->_modulus;
}

string CustomHeader::extension() 
{
    return this->_extension;
}

char CustomHeader::type() 
{
    return this->_type;
}

int CustomHeader::last_position() 
{
    return this->_last_pos;
}

void CustomHeader::last_position(int n) 
{
    this->_last_pos = n;
}
