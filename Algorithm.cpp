#include <iostream>
#include "Algorithm.h"

using namespace std;

#define DEF_MODULE 64 // The bigger the module, the more spread(?) the message
#define WAV_HEADER 44
#define MY_HEADER_MODULE 64
/**
* My custom header with the attributes of the file to retireve:
* 4 bytes for modulus value
* 4 for file extension
* 1 byte for the type of message to find (text or binary)
* */
#define MY_HEADER 9

// TODO: Fix file extension

status play_with_wave_buf(vector<char>& buffer, string& msg, string& input_ext)
{
    char* modulus_bytes = new char[4] {0}; // Max number of modulus in bytes
    char* custom_header = new char[MY_HEADER] {0}; // Custom header
    string file_extension = "";

    string end = "@<;;";
    msg += end;

    // How many times the buffer is bigger than the message
    long modulus = ((buffer.size() - WAV_HEADER) / (msg.size() + MY_HEADER));

    cout << "Spreading level: " << modulus << endl;

    // Verify if it is safe to hide the message. Must me at must half the size of the space avaible
    if(modulus <= 3) {
        cout << "The message might be too big for the audio file" << endl;
        return ERROR;
    }

    create_header(modulus, modulus_bytes, custom_header, file_extension, false);

    int n = 0;
    unsigned pos = 0;

    // Write my custom header first (Spread)
    for (vector<char>::iterator it = buffer.begin() + WAV_HEADER; it != buffer.end(); ++it) {
        if (n % MY_HEADER_MODULE == 0) {
            *it = custom_header[pos];
            pos++;
            //cout << "Header -> " << *it << endl; // Uncomment this to see the message being written

            if (pos == MY_HEADER) {
                cout << "Header written" << endl;
                break;
            }
        }
        
        n++;
    }

    // Delete arrays we are not using it anymore
    modulus_bytes = new char[1]; // Relocate memory
    delete[] modulus_bytes;
    delete[] custom_header;

    int j = 0;
    pos = 0;
    // Since the actual data of the wav starts at byte 44. Everything above is just header things that we don't care atm
    for (vector<char>::iterator it = buffer.begin() + WAV_HEADER + n + MY_HEADER_MODULE; 
         it != buffer.end(); ++it) {
        if (j % modulus == 0) {
            *it = msg.at(pos);
            pos++;
            //cout << *it << endl; // Uncomment this to see the message being written

            if (pos >= msg.size())
                break;
        }
        
        j++;
    }

    if (pos < msg.size())
        cerr << "Whole file was not written" << endl;

    return output_stegged_data(buffer, input_ext);
}

status play_with_wave_buf(vector< char >& buffer, vector< char >& file_msg, string& file_extension, string& input_ext)
{
    char* modulus_bytes = new char[4] {0}; // Max number of modulus in bytes
    char* custom_header = new char[MY_HEADER] {0}; // Custom header

    if (buffer.size()/4 <= file_msg.size()) {
        cout << "The message might be too big for the audio file" << endl;
        return ERROR;
    }

    string end = "@<;;";
    
    file_msg.push_back('@');
    file_msg.push_back('<');
    file_msg.push_back(';');
    file_msg.push_back(';');

    // How many times the buffer is bigger than the message
    // buffer.size() - (HEADER SIZE = 44 bytes) - (My own tags to de hidden file = 3 bytes)
    long modulus = ((buffer.size() - WAV_HEADER) / (file_msg.size() + MY_HEADER));

    cout << "Spreading level: " << modulus << endl;

    // Verify if it is safe to hide the message. Must me at must half the size of the space avaible
    if(modulus <= 3) {
        cout << "The message might be to big for the audio file" << endl;
        return ERROR;
    }

    create_header(modulus, modulus_bytes, custom_header, file_extension, true);

    int n = 0;
    unsigned pos = 0;

    // Write my custom header first (Spread)
    for (vector<char>::iterator it = buffer.begin() + WAV_HEADER; it != buffer.end(); ++it) {
        if (n % MY_HEADER_MODULE == 0) {
            *it = custom_header[pos];
            pos++;
            //cout << "Header -> " << *it << endl; // Uncomment this to see the message being written

            if (pos == MY_HEADER) {
                cout << "Header written" << endl;
                break;
            }
        }
        n++;
    }

    // Delete arrays we are not using it anymore
    modulus_bytes = new char[1]; // Relocate memory
    delete[] modulus_bytes;
    delete[] custom_header;

    int j = 0;
    pos = 0;
    for (vector<char>::iterator it = buffer.begin() + WAV_HEADER + n + MY_HEADER_MODULE;
         it != buffer.end(); ++it)
    {
        if (j % modulus == 0) {
            *it = file_msg.at(pos);
            pos++;
            //cout << *it << endl; // Uncomment this to see the message being written

            if (pos >= file_msg.size())
                break;
        }
        j++;
    }

    if (pos < file_msg.size())
        cerr << "Whole file was not written" << endl;

    return output_stegged_data(buffer, input_ext);
}

/**
* Look for the custom header and define what type of message is in it
*
* @param buffer: File loaded into a buffer
* */
status find_hidden_message(vector<char>& buffer)
{
    char* custom_header_raw = new char[MY_HEADER] {0}; // Custom header

    int n = 0;
    int pos = 0;
    cout << "Looking for the hidden message..." << endl;
    // Since the actual data of the wav starts at byte 44 we start from it. Everything above is just header things that we don't care atm
    for (vector<char>::iterator it = buffer.begin() + WAV_HEADER; it != buffer.end(); ++it) {
        if (n % MY_HEADER_MODULE == 0) {
            custom_header_raw[pos] = *it;
            //cout << "Custom Header: " << customHeader[pos] << endl; // Uncomment this if you want to see the header being read
            pos++;
            if (pos == MY_HEADER)
            {
                //cout << "Header has been read " << endl;
                break;
            }
        }
        n++;
    }

    CustomHeader custom_header(custom_header_raw);

    // Clean memory
    delete[] custom_header_raw;

    if (custom_header.type() == 'b') {
        cout << "File detected. Retrieving it..." << endl;
        custom_header.last_position(n);
        return find_hidden_file_in_wave(buffer, custom_header);
    } else if (custom_header.type() == 't'){
        cout << "String detected. Retrieving it..." << endl;
        custom_header.last_position(n);
        return find_hidden_text_in_wave(buffer, custom_header);
    } else {
        // If it hits here it's because there was no message found in the file
        cout << "Failed to detect a hidden file." << endl;
        cout << "No custom header was found." << endl;
        return ERROR;
    }
}

status find_hidden_text_in_wave(vector<char>& buffer, CustomHeader& custom_header)
{
    string msg_text;

    int modulus = custom_header.modulus();
    int last_pos = custom_header.last_position();

    int n = 0;
    int pos = 0;
    vector<char>::iterator temp_it;
    // Since the actual data of the wav starts at byte 44. Everything above is just header things that we don't care atm
    for (vector<char>::iterator it = buffer.begin() + WAV_HEADER + last_pos + MY_HEADER_MODULE;
         it != buffer.end(); ++it)
    {
        if (n % modulus == 0) {
            if (*it == '@') {
                // Setting the iterator to the next possible position
                temp_it = buffer.begin() + n + 44 + MY_HEADER_MODULE + last_pos + modulus;

                if (*temp_it == '<') {
                    // Setting the iterator to the next possible position
                    temp_it = buffer.begin() + n + 44 + MY_HEADER_MODULE + last_pos + (2 * modulus);

                    if (*temp_it == ';') {
                        // Setting the iterator to next possible flag
                        temp_it = buffer.begin() + n + 44 + MY_HEADER_MODULE + last_pos + (3 * modulus);

                        if (*temp_it == ';') {
                            // End of message reached
                            cout << "Message recovered size: " << pos << " bytes" << endl;

                            // Output text
                            cout << "Message: " << msg_text.c_str() << endl;
                            return SUCCESS;
                        }
                    }
                }
            }

            msg_text[pos] = *it;
            //cout << msgText[pos] << endl; // Uncomment this if yuo want to see the characters being read
            pos++;

        }
        n++;
    }

    // If it hits here it's because there was no message found in the file
    cout << "No message found :(" << endl;
    return ERROR;

}

status find_hidden_file_in_wave(vector<char>& buffer, CustomHeader& custom_header)
{
    vector<char> msg_buf;

    int modulus = custom_header.modulus();
    int last_pos = custom_header.last_position();

    int n = 0;
    int pos = 0;
    vector<char>::iterator temp_it;
    
    // Since the actual data of the wav starts at byte 44. Everything above is just header things that we don't care atm
    for (vector<char>::iterator it = buffer.begin() + + WAV_HEADER + last_pos + MY_HEADER_MODULE;
         it != buffer.end(); ++it)
    {
        if (n % modulus == 0) {
            if (*it == '@') {
                //Setting the iterator to the next possible position
                temp_it = buffer.begin() + n + 44 + MY_HEADER_MODULE + last_pos + modulus;

                if (*temp_it == '<') {
                    //Setting the iterator to the next possible position
                    temp_it = buffer.begin() + n + 44 + MY_HEADER_MODULE + last_pos + (2* modulus);

                    if (*temp_it == ';') {
                        // Setting the iterator to next possible flag
                        temp_it = buffer.begin() + n + 44 + MY_HEADER_MODULE + last_pos + (3 * modulus);

                        if (*temp_it == ';'){
                            // End of message reached
                            cout << "Message recovered size: " << pos << " bytes" << endl;
                            return output_file(msg_buf, custom_header);
                        }
                    }
                }
            }

            msg_buf.push_back(*it);
            //cout << "Data recovered: " << msgBuffer[pos] << endl; // Uncomment this if you want to see the characters being read
            pos++;

        }
        n++;
    }

    // If it hits here it's because there was no message found in the file
    cout << "Could not find the end tags of the hidden file :(" << endl;
    return ERROR;
}

status write_message_from_end(vector<char>& buffer, string msg)
{
    // Verify if it is safe to hide the message in the buffer
    if ((buffer.size() / 4) < msg.size()) {
        cout << "The message might be to big for the audio file" << endl;
        return ERROR;
    }

    // Inverse iterator runs backwards
    int n = 0;
    unsigned pos = 0;
    for (vector<char>::reverse_iterator i = buffer.rbegin(); i != buffer.rend(); ++i) {
        if (n%1000 == 0) {
            *i = msg.at(pos);
            pos++;
            cout << *i << endl;

            if (pos >= msg.size())
                break;
        }
        
        n++;
    }

    return SUCCESS;
}

status output_stegged_data(vector<char>& buffer, string& file_extension)
{
    string fname;
    
    if (file_extension == "")
        fname = "output";
    else
        fname = "output." + file_extension;

    ofstream output(fname, ios::binary);
    output.write((const char*)&buffer[0], buffer.size());
    output.close();
    cout << "File has been saved as: " << fname << endl;

    return SUCCESS;
}

status output_file(vector<char>& buffer, CustomHeader& custom_header)
{
    string fname;
    if (custom_header.extension() == "")
        fname = "output";
    else
        fname = "output." + custom_header.extension();

    ofstream output(fname, ios::binary);
    output.write((const char*)&buffer[0], buffer.size());
    output.close();
    cout << "File has been saved as: " << fname << endl;

    return SUCCESS;
}

void create_header(long& modulus, char* modulus_bytes, char* custom_header, string& file_extension, bool is_file)
{
    /**
    * Create custom header (9 bytes):
    * Modulus value: 4 bytes
    * File extension: 4 bytes
    * Message type (text or file): 1 byte
    * */

    // If modulus is lesser than the max int value of 4 bytes assign it.
    // Otherwise, set a flag to use DEF_MODULE constant
    if (modulus <= 42946729)
        modulus_bytes = reinterpret_cast<char*>(&modulus);
    else
        modulus_bytes[0] = DEF_MODULE;

    unsigned y, i;
    // Assign modulus to header
    for (i = 0; i < 4; i++)
        custom_header[i] = modulus_bytes[i];

    // Assign the file extension to recover later
    for(y = 0, i = 4; i < 8; i++) {
        // Dinamically fill the 4 bytes of the file extension even if its less than 4 characters
        if(file_extension.size() > y )
            custom_header[i] = file_extension.at(y);
        else
            custom_header[i] = ' ';
        y++;
    }

    // Assign the type of the message
    if (is_file)
        custom_header[8] = 'b';
    else
        custom_header[8] = 't';
}
