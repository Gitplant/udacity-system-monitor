#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    long int hh = seconds / 60 / 60;
    long int mm = (seconds - hh * 60 * 60) / 60;
    long int ss = seconds - hh * 60 * 60 - mm * 60;
    string elapsed_time = Format::LeadingZeros(hh) + ":" + Format::LeadingZeros(mm) + ":" + Format::LeadingZeros(ss);
    return elapsed_time; }

string Format::LeadingZeros(long int number){
    std::string str_number = std::to_string(number);
    std::string formattedText;
    if (str_number.length() < 2) {
        formattedText = std::string(2 - str_number.length(), '0') + str_number;
    }
    else {
        formattedText = str_number;
    }
    return formattedText;
}