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

// Constants
long int LinuxParser::clock_frequency = sysconf(_SC_CLK_TCK);

/* Read a value from a stream who's lines are constructed as:
name value possible_more_values.*/
string GetValueFromStream(std::ifstream& stream, string name){
  string line;
  string key, value;
  if (stream.is_open()) {
      while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        linestream >> key >> value;
        if (key == name){
        // Temp
        // std::cout << line << "\n";
        // std::cout << "key = " << key << "\n";
        // std::cout << "value = " << value << "\n";
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
float LinuxParser::UpTime() {
  string line;
  float up_time, idle_time;
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

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {

  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line, command;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> command;
      return command;
    }
  }

  return string(); }

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  long int ram = std::stoi(GetValueFromStream(stream, "VmSize:")) / 1000;
  string ram_str = to_string(ram);

  return ram_str; }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string user_id = GetValueFromStream(stream, "Uid:");

  // Get user name from user id
  string line, user, x, key;
  std::ifstream stream2(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream2, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> key;
      if (key == user_id){
        return user;
      }
    }
    throw std::invalid_argument("Could not find user_id " + user_id + "in stream.\n");
  }


  return string(); }

// Read Process Process statusses for cpu utilization
vector<string> LinuxParser::ProcessStatusses(int pid) {

  string line;
  std::vector<std::string> process_status;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    char delimeter = ' ';
    while (std::getline(stream, line)) {
      process_status = LinuxParser::StringToVector(line, delimeter);
    }
  }

  return process_status; }

vector<string> LinuxParser::StringToVector(std::string line, char delimeter){

  std::istringstream linestream(line);
  std::string token;
  std::vector<std::string> output;
  while (std::getline(linestream, token, delimeter)) {
      output.push_back(token);
  }

  return output;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {

  long uptime;
  string line;
  vector<string> stat;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    char delimeter = ' ';
    while (std::getline(stream, line)) {
      stat = LinuxParser::StringToVector(line, delimeter);
    }
  }
  uptime = std::stol(stat[LinuxParser::ProcessStates::kStartTime_]) / LinuxParser::clock_frequency;

  return uptime;
}
