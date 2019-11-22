#include "process.h"
#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int& pid)
        : m_pid(pid) {
}

// DONE: Return this process's ID
int Process::Pid() { return m_pid; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const {
    const float tck = sysconf(_SC_CLK_TCK);
    const float tot = static_cast<float>(LinuxParser::ActiveJiffies(m_pid)) / tck;
    const auto up = LinuxParser::UpTime();
    const auto pUp = LinuxParser::UpTime(m_pid);
    return tot / static_cast<float>(up - pUp);
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(m_pid); }

// DONE: Return this process's memory utilization
string Process::Ram() {
    string s = LinuxParser::Ram(m_pid);
    return s;
}

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(m_pid); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(m_pid); }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    return (CpuUtilization() > a.CpuUtilization());
}
