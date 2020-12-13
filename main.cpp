#include <iostream>
#include "main.h"
#include "Algorithm.h"

using namespace std;

typedef enum {
    HIDE_STRING = 1,
    HIDE_FILE = 3,
    RETRIEVE = 2
} hide_mode;

int main(int argc, char* argv[])
{
    string msg;
    string input_path;
    string file_ext, input_ext;

    ifstream bin_file_stream;
    streampos bin_file_size;
    vector<char> msg_buf;

    hide_mode mode = HIDE_STRING; // hide

    // TODO: Use boost program options instead of this yuck
    if (argc < 2) {
        cout << "Where are my the parameters mate?" << endl;
        cout << "To hide a string:  ./HideMeIn [input_file] \"'string message'\" (Single quotation inside double quotation)" << endl;
        cout << "To hide a file:    ./HideMeIn [input_file] [file_to_hide]" << endl;
        cout << endl;
        cout << "To retrieve something already hidden:   ./HideMeIn [file_with_hidden_data] -f" << endl;
        return 0;
    } else if (argc == 2) {
        /**
        * When only the song path is passed.
        * Using the default message
        * @param: song
        * */

        cout << "No message to hide was specified. Using a default string message..." << endl;
        input_path = argv[1];
        msg = "Default Message KEKLIFE";
        //return 0;
    } else if (argc == 3){
        /**
        * When only the song path and the (message or flag) are passed.
        * In case mode is passsed, find hidden message
        * @param: song, message/flag
        * */

        input_path = argv[1];
        if (string(argv[2]) == "-f" || string(argv[2]) == "--find") {
            // Find the hidden message mode
            mode = RETRIEVE;
        } else {
            // User wants to hide data then
            // Decide whether is a string or a file
            if (string(argv[2])[0] == 39 && string(argv[2]).back() == 39) {
                // Quotation marks detected. Hide string message
                mode = HIDE_STRING;
                msg = string(argv[2]);

                // Get input file extension
                input_ext = get_file_extension(string(argv[1]));
            } else {
                // Take it as a file path
                mode = HIDE_FILE;

                // Get files extension
                file_ext = get_file_extension(string(argv[2]));
                input_ext = get_file_extension(string(argv[1]));

                bin_file_stream.open(string(argv[2]), std::ios::binary );
                bin_file_size = bin_file_stream.tellg();
                msg_buf.reserve(bin_file_size); // Reserve the amount of the file size of memory to the vector
            }
        }
    }
    else{
        cout << "Too much argumments maybe? " << endl;

        cout << "To hide a string:  ./HideMeIn [input_file] \"'string message'\" (Single quotation inside double quotation)" << endl;
        cout << "To hide a file:    ./HideMeIn [input_file] [file_to_hide]" << endl;
        cout << endl;
        cout << "To retrieve something already hidden:   ./HideMeIn [file_with_hidden_data] -f" << endl;
        return 0;
    }

    cout << "Doing it boss! " << endl;

    // Get input file
    ifstream input(input_path, std::ios::binary);
    if (!input.is_open()) {
        cout << "Unable to open the file given" << endl;
        return 0;
    }

    // Copy the song into a buffer and closing it because we don't want to touch that
    vector<char> buffer(
                    (istreambuf_iterator<char>(input)),
                    (istreambuf_iterator<char>()));

    if (mode == HIDE_FILE) {
        /**
         * If we are going to hide a binary file:
         * Assign the binary file into a vector to play with him later and close the file.
         */

        msg_buf.assign(
                (istreambuf_iterator<char>(bin_file_stream)),
                (istreambuf_iterator<char>()));

        bin_file_stream.close();
    }

    input.close();
    
    switch (mode) {
        case HIDE_STRING: {
            int status = play_with_wave_buf(buffer, msg, input_ext);
            if (status == SUCCESS)
                cout << "Hiding process has finished successfully.\nCleaning memory..." << endl;
            else if (status == ERROR)
                cout << "Something failed.\nCleaning memory..." << endl;
            break;
        }
        case HIDE_FILE: {
            int status = play_with_wave_buf(buffer, msg_buf, file_ext, input_ext);
            if (status == SUCCESS)
                cout << "Hiding process has finished successfully.\nCleaning memory..." << endl;
            else if (status == ERROR)
                cout << "Something failed.\nCleaning memory..." << endl;
            break;
        }
        case RETRIEVE: {
            int status = find_hidden_message(buffer);
            if (status == SUCCESS)
                cout << "Recovering process has finished successfully.\nCleaning memory..." << endl;
            else if (status == ERROR)
                cout << "Something failed.\nCleaning memory..." << endl;
            break;
        }
        default:
            break;
    }

    // Force remove the buffer from memory
    vector<char>().swap(buffer);
    vector<char>().swap(msg_buf);
    return 0;
}

/**
* This will return the extension even on strings like "a.b.c.d.e.s.mp3"
* If it cannot find the extension it will return "".
*
* @param fileName: The entire file name
* */
string get_file_extension(const string& fname)
{
    if(fname.find_last_of(".") != std::string::npos)
        return fname.substr(fname.find_last_of(".")+1);
    return "";
}
