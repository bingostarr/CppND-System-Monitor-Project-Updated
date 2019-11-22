#include "linux_parser.h"
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, vstr, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> vstr >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
    string key, line;
    float memTotal = 1;
    float memFree = 0;
    float memBuffer = 0;
    std::ifstream filestream(kProcDirectory + kMeminfoFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            string value;
            linestream >> key >> value;
            if ("MemTotal:" == key) {
                memTotal = std::stof(value);
            }
            if ("MemFree:" == key) {
                memFree = std::stof(value);
            }
            if ("Buffers:" == key) {
                memBuffer = std::stof(value);
            }
        }
    }
    return 1 - (memFree / (memTotal - memBuffer));
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
    std::ifstream filestream(kProcDirectory + kUptimeFilename);
    if (filestream.is_open()) {
        string line;
        std::getline(filestream, line);
        float uptime, idletime;
        std::istringstream linestream(line);
        linestream >> uptime >> idletime;
        long up = static_cast<long>(uptime);
        return up;
    }
    return 0;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
    return UpTime() * sysconf(_SC_CLK_TCK);
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
    long jiffies = 0;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
    if (filestream.is_open()) {
        string line;
        std::getline(filestream, line);
        std::istringstream linestream(line);
        vector<string> values;
        string s;
        while (linestream >> s) {
            values.emplace_back(s);
        }
        if (values.size() > 21) {
                jiffies = std::stol(values[13]) +
                          std::stol(values[14]) +
                          std::stol(values[15]) +
                          std::stol(values[16]);
        }
    }
    return jiffies;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
    long jiffies = 0;
    vector<string> cpu = LinuxParser::CpuUtilization();
    for (int i = kUser_; i <= kGuestNice_; i++) {
        jiffies += std::stol(cpu[i]);
    }
    jiffies -= std::stol(cpu[kIdle_]) + std::stol(cpu[kIOwait_]);
    return jiffies;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
    vector<string> cpu = LinuxParser::CpuUtilization();
    return std::stol(cpu[kIdle_]) + std::stol(cpu[kIOwait_]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
    vector<string> values;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        string key, line;
        getline(filestream, line);
        std::istringstream linestream(line);
        string value;
        while (linestream >> value) {
            if ("cpu" != value) {
                values.emplace_back(value);
            }
        }
    }
    return values;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        string key, line;
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            int value;
            linestream >> key >> value;
            if ("processes" == key) {
                return value;
            }
        }
    }
    return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        string key, line;
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            int value;
            linestream >> key >> value;
            if ("procs_running" == key) {
                return value;
            }
        }
    }
    return 0;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
    if (filestream.is_open()) {
        string line;
        std::getline(filestream, line);
        return line;
    }
    return string();
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (filestream.is_open()) {
        string key, line;
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            string value;
            linestream >> key >> value;
            if ("VmSize:" == key) {
                return std::to_string(std::stol(value) / 1024);
            }
        }
    }
    return string();
}

// DONE: Read and return the user ID associated with a Linux UID
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (filestream.is_open()) {
        string key, line;
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            string key, value;
            while (linestream >> key >> value) {
                if ("Uid:" == key) {
                    return value;
                }
            }
        }
    }
    return string();
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
    std::ifstream filestream(kPasswordPath);
    if (filestream.is_open()) {
        string line;
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            string name, x, value;
            while (linestream >> name >> x >> value) {
                if (value == Uid(pid)) {
                    return name;
                }
            }
        }
    }
    return string();
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (filestream.is_open()) {
        string line;
        while (std::getline(filestream, line)) {
            int count = 1;
            string value;
            std::istringstream linestream(line);
            while (linestream >> value) {
                if (22 == count) {
                    long up = std::stol(value)/sysconf(_SC_CLK_TCK);
                    return up;
                }
                count++;
            }
        }
    }
    return 0;
}
