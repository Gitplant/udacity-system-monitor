#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

//Temp
#include <iostream>
using std::cout;


// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
    vector<string> cpu_line = LinuxParser::CpuUtilization();

    int user = std::stoi(cpu_line[LinuxParser::CPUStates::kUser_]);
    int nice = std::stoi(cpu_line[LinuxParser::CPUStates::kNice_]);
    int system = std::stoi(cpu_line[LinuxParser::CPUStates::kSystem_]);
    int idle = std::stoi(cpu_line[LinuxParser::CPUStates::kIdle_]);
    int iowait = std::stoi(cpu_line[LinuxParser::CPUStates::kIOwait_]);
    int irq = std::stoi(cpu_line[LinuxParser::CPUStates::kIRQ_]);
    int softirq = std::stoi(cpu_line[LinuxParser::CPUStates::kSoftIRQ_]);
    int steal = std::stoi(cpu_line[LinuxParser::CPUStates::kSteal_]);

    // Current numbers:
    int Idle = idle + iowait;
    int NonIdle = user + nice + system + irq + softirq + steal;
    int Total = Idle + NonIdle;

    // Percentage:
    int totald = Total - this->prev_total_;
    int idled = Idle - this->prev_idle_;
    float cpu_percentage = 0.0;
    if (totald != 0){
        cpu_percentage = (totald - idled) / static_cast<float>(totald);
    }

    // Set previous attributes for next iteration:
    int PrevNonIdle = user + nice + system + irq + softirq + steal;
    this->prev_total_ = Idle + PrevNonIdle;
    this->prev_idle_ = Idle;

    return cpu_percentage; }