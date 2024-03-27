#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

//Temp
#include <iostream>
using std::cout;

/* Read a value from a stream who's lines are constructed as:
name value possible_more_values.*/
 string GetValueFromStream(std::ifstream& stream, string name){
  string line;
  string key, value;
  if (stream.is_open()) {
      while (std::getline(stream, line)) {
        // Temp
        // std::cout << line << "\n";
        std::istringstream linestream(line);
        linestream >> key >> value;
        if (key == name){
          return value;
        }
      }
      throw std::invalid_argument("Could not find key " + name + "in stream.\n");
  }

  return 0;
 }
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
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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
  string line;
  string key, value;
  long int memTotal, memFree;
  float memPercentage;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  memTotal = std::stoi(GetValueFromStream(stream, "MemTotal:"));
  memFree = std::stoi(GetValueFromStream(stream, "MemFree:"));
  memPercentage = 1 - (static_cast<float>(memFree) / memTotal);
  return memPercentage; }

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long up_time, idle_time;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time >> idle_time;
  }
  return up_time;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {

  string line;
  string key;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "cpu"){
        linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
        vector<string> cpu(10);
        cpu[LinuxParser::CPUStates::kUser_] = user;
        cpu[LinuxParser::CPUStates::kNice_] = nice;
        cpu[LinuxParser::CPUStates::kSystem_] = system;
        cpu[LinuxParser::CPUStates::kIdle_] = idle;
        cpu[LinuxParser::CPUStates::kIOwait_] = iowait;
        cpu[LinuxParser::CPUStates::kIRQ_] = irq;
        cpu[LinuxParser::CPUStates::kSoftIRQ_] = softirq;
        cpu[LinuxParser::CPUStates::kSteal_] = steal;
        cpu[LinuxParser::CPUStates::kGuest_] = guest;
        cpu[LinuxParser::CPUStates::kGuestNice_] = guest_nice;
        return cpu;
      }
    }
    throw std::invalid_argument("Could not find key cpu in stream.");
  }

  return {}; }

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string nr_processes_str = GetValueFromStream(stream, "processes");
  long int nr_processes = std::stoi(nr_processes_str);
  return nr_processes;
 }

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string nr_running_processes_str = GetValueFromStream(stream, "procs_running");
  long int nr_running_processes = std::stoi(nr_running_processes_str);
  return nr_running_processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string user = GetValueFromStream(stream, "Uid:");
  return user; }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
