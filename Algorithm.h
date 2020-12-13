#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "CustomHeader.h"

using std::string;
using std::vector;

typedef enum {
    SUCCESS,
    ERROR
} status;

void create_header(long int& modulus, char* modulus_bytes, char* custom_header, string& file_extension, bool is_file);
status output_file(vector< char >& buffer, CustomHeader& custom_header);
status output_stegged_data(vector< char >& buffer, string& fileExtension);
status write_message_from_end(vector< char >& buffer, string msg);
status play_with_wave_buf(vector<char>& buffer, string& msg, string& input_ext);             // Hide a string
status play_with_wave_buf(vector<char>& buffer, vector< char >& file_msg, string& file_extension, string& input_ext);  // Hide a binary file
status find_hidden_text_in_wave(vector<char>& buffer, CustomHeader& customHeader);
status find_hidden_file_in_wave(vector<char>& buffer, CustomHeader& custom_header);
status find_hidden_message(vector<char>& buffer);

