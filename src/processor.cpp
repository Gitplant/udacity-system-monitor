#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
    vector<string> cpu_line = LinuxParser::CpuUtilization();

    long long user = std::stoll(cpu_line[LinuxParser::CPUStates::kUser_]);
    long long nice = std::stoll(cpu_line[LinuxParser::CPUStates::kNice_]);
    long long system = std::stoll(cpu_line[LinuxParser::CPUStates::kSystem_]);
    long long idle = std::stoll(cpu_line[LinuxParser::CPUStates::kIdle_]);
    long long iowait = std::stoll(cpu_line[LinuxParser::CPUStates::kIOwait_]);
    long long irq = std::stoll(cpu_line[LinuxParser::CPUStates::kIRQ_]);
    long long softirq = std::stoll(cpu_line[LinuxParser::CPUStates::kSoftIRQ_]);
    long long steal = std::stoll(cpu_line[LinuxParser::CPUStates::kSteal_]);

    // Current numbers:
    long long Idle = idle + iowait;
    long long NonIdle = user + nice + system + irq + softirq + steal;
    long long Total = Idle + NonIdle;

    // Percentage:
    long long totald = Total - this->prev_total_;
    long long idled = Idle - this->prev_idle_;
    float cpu_percentage = 0.0;
    if (totald != 0){
        cpu_percentage = (totald - idled) / static_cast<float>(totald);
    }

    // Set previous attributes for next iteration:
    this->prev_total_ = Idle + NonIdle;
    this->prev_idle_ = Idle;

    return cpu_percentage; }