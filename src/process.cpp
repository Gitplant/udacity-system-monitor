#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Temp
#include <iostream>
using std::cout;

// TODO: Return this process's ID
int Process::Pid() {
    return pid_; }

void Process::SetPid(int pid) {
    pid_ = pid;
}

// Set the system's uptime;
void Process::SetUptime(float uptime){
    uptime_ = uptime;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {

    vector<string> process_statusses = LinuxParser::ProcessStatusses(this->pid_);

    long Hz = sysconf(_SC_CLK_TCK);

    int stime = std::stoi(process_statusses[LinuxParser::ProcessStates::kSTime_]);
    int utime = std::stoi(process_statusses[LinuxParser::ProcessStates::kUTime_]);
    int total_time = utime + stime;
    int start_time = std::stoi(process_statusses[LinuxParser::ProcessStates::kStartTime_]);
    float elapsed_time = this->uptime_ - (static_cast<float>(start_time) / Hz);
    float cpu_usage = ((static_cast<float>(total_time) / Hz) / elapsed_time);

    return cpu_usage; }

// TODO: Return the command that generated this process
string Process::Command() { return string(); }

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(this->pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }