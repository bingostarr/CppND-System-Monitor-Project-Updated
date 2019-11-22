#include "format.h"
#include <string>

using std::string;

static auto constexpr SECS = 3600;
static auto constexpr MINS = 60;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    string outStr = string();
    long hours = seconds/SECS;
    string hh = std::to_string(hours);
    if (hours < 10) hh = "0" + hh;
    seconds %= SECS;
    long mins = seconds/MINS;
    string mm = std::to_string(mins);
    if (mins < 10) mm = "0" + mm;
    seconds %= MINS;
    string ss = std::to_string(seconds);
    if (seconds < 10) ss = "0" + ss;
    outStr = hh + ":" + mm + ":" + ss + "\n";
    return outStr;
}
