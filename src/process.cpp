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

// DONE: Return this process's ID
int Process::Pid() {
    return pid_; }

void Process::SetPid(int pid) {
    pid_ = pid;
}

// Set the system's uptime;
void Process::SetUptime(float uptime){
    uptime_ = uptime;
}

void Process::SetCpuUtilization() {

    vector<string> process_statusses = LinuxParser::ProcessStatusses(this->pid_);

    long long stime = std::stoll(process_statusses[LinuxParser::ProcessStates::kSTime_]);
    long long utime = std::stoll(process_statusses[LinuxParser::ProcessStates::kUTime_]);
    long long total_time = utime + stime;
    long long start_time = std::stoll(process_statusses[LinuxParser::ProcessStates::kStartTime_]);
    float elapsed_time = this->uptime_ - (static_cast<float>(start_time) / LinuxParser::clock_frequency);
    this->cpu_usage_ = ((static_cast<float>(total_time) / LinuxParser::clock_frequency) / elapsed_time);
    }

// DONE: Return this process's CPU utilization
float Process::GetCpuUtilization(){ return this->cpu_usage_; };

// DONE: Return the command that generated this process
string Process::Command() {

    // Cut the command off if it is longer than 50 characters
    string full_command = LinuxParser::Command(this->pid_);
    int command_length = full_command.length();
    int max_length = 50;
    if (command_length > max_length){
        int nr_dots = std::min(3, command_length - max_length);
        std::string dots(nr_dots, '.');
        string command = full_command.substr(0, max_length - nr_dots) + dots;
        return command;
        }
    return full_command;
    }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(this->pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(this->pid_); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(this->pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
        return a.cpu_usage_ < cpu_usage_; }