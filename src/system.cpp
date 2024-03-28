#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

//Temp
#include <iostream>
using std::cout;

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {

    vector<int> pids = LinuxParser::Pids();
    processes_.clear();
    for (int pid: pids){
        Process process = Process();
        process.SetPid(pid);
        process.SetUptime(LinuxParser::UpTime());
        process.SetCpuUtilization();
        processes_.push_back(process);
    }

    // Sort the processes by cpu utilization
    std::sort(processes_.begin(), processes_.end());

    return processes_; }

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running

long int System::UpTime() { return  static_cast<long int>(LinuxParser::UpTime()); }