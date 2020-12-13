#pragma once

#include <string>

using std::string;

class CustomHeader {
    int _modulus;
    string _extension;
    char _type;
    int _last_pos = 0;
public:
    CustomHeader(char* header);
    CustomHeader();
    int modulus();
    string extension();
    char type();
    int last_position();
    void last_position(int n);
};

