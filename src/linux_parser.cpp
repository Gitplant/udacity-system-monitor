#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Constants
long int LinuxParser::clock_frequency = sysconf(_SC_CLK_TCK);

/* Read a value from a stream who's lines are constructed as:
name value possibly_more_values.*/
string LinuxParser::GetValueFromStream(std::ifstream& stream, string name){

  string line;
  string key, value;
  if (stream.is_open()) {
      while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        linestream >> key >> value;
        if (key == name){
          return value;
        }
      }
  }
  stream.close();

  return string();
 }

 // Convert a string line to a vector of strings
vector<string> LinuxParser::StringToVector(std::string line, char delimeter){

  std::istringstream linestream(line);
  std::string token;
  std::vector<std::string> output;
  while (std::getline(linestream, token, delimeter)) {
      output.push_back(token);
  }

  return output;
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
  filestream.close();
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
  stream.close();
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
  long long memTotal, memFree;
  float memPercentage;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  string memTotal_str = LinuxParser::GetValueFromStream(stream, "MemTotal:");
  string memFree_str = LinuxParser::GetValueFromStream(stream, "MemFree:");
  if (memTotal_str == string() || memFree_str == string()){
    return -1.0;
  }
  memFree = std::stoll(memFree_str);
  memTotal = std::stoll(memTotal_str);
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
  stream.close();

  return up_time;
}

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
  }
  stream.close();

  return {}; }

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string nr_processes_str = LinuxParser::GetValueFromStream(stream, "processes");
  long long nr_processes;
  if (nr_processes_str != string()){
    nr_processes = std::stoll(nr_processes_str);
  }
  else{
    nr_processes = -1;
  }
  return nr_processes;
 }

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string nr_running_processes_str = LinuxParser::GetValueFromStream(stream, "procs_running");
  long long nr_running_processes;
  if (nr_running_processes_str != string()){
    nr_running_processes = std::stoll(nr_running_processes_str);
  }
  else{
    nr_running_processes = -1;
  }
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
  stream.close();

  return string(); }

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {

  string ram_str;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string ram_output = LinuxParser::GetValueFromStream(stream, "VmSize:");
  if (ram_output != string()){
    long long ram = std::stoll(ram_output) / 1000;
    ram_str = to_string(ram);
  }
  else {
    ram_str = string();
  }
  return ram_str; }

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string user_id = LinuxParser::GetValueFromStream(stream, "Uid:");

  return user_id; }

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {

  string user_id = LinuxParser::Uid(pid);
  if (user_id != string()){
    // Get user name from user id
    string line, user, x, key;
    std::ifstream stream(kPasswordPath);
    if (stream.is_open()) {
      while (std::getline(stream, line)) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> user >> x >> key;
        if (key == user_id){
          return user;
        }
      }
    }
    stream.close();
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
  stream.close();

  return process_status; }

//Convert to kernel version to a vector of ints
std::vector<int> LinuxParser::KernelToInt(string kernel){
  std::vector<int> splitted_kernel;

  std::string temp;
  for (char c : kernel){
    if (std::isdigit(c)){
      temp += c;
    }
    else{
      if (!temp.empty()){
        splitted_kernel.push_back(std::stoi(temp));
        temp.clear();
      }
    }
  }
  if (!temp.empty()){
        splitted_kernel.push_back(std::stoi(temp));
      }
  return splitted_kernel;
}

//Compare if the version of kernel1 >= kernel2
bool LinuxParser::CompareKernelVersions(string kernel1, string kernel2){

  std::vector<int> splitted_kernel1 = LinuxParser::KernelToInt(kernel1);
  std::vector<int> splitted_kernel2 = LinuxParser::KernelToInt(kernel2);

  int minimum_length = std::min(splitted_kernel1.size(), splitted_kernel2.size());

  for(int i=0 ; i<minimum_length ; i++){
    if (splitted_kernel1[i] > splitted_kernel2[i]){
      return true;
    }
    else if (splitted_kernel1[i] < splitted_kernel2[i]){
      return false;
    }
  }

  return true;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {

  long uptime, starttime;
  string line;
  vector<string> stat;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    char delimeter = ' ';
    while (std::getline(stream, line)) {
      stat = LinuxParser::StringToVector(line, delimeter);
    }
  }
  stream.close();

  starttime = std::stol(stat[LinuxParser::ProcessStates::kStartTime_]);

  string kernel = LinuxParser::Kernel();

  bool new_kernel = LinuxParser::CompareKernelVersions(kernel, "2.6");
  if (new_kernel){
    uptime = starttime / LinuxParser::clock_frequency;
  }
  else{
    uptime = 0;
  }

  return uptime;

}
