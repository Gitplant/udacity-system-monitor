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

    //TODO (personal): Figure out if this can be done with enums.
    int user = std::stoi(cpu_line[0]);
    int nice = std::stoi(cpu_line[1]);
    int system = std::stoi(cpu_line[2]);
    int idle = std::stoi(cpu_line[3]);
    int iowait = std::stoi(cpu_line[4]);
    int irq = std::stoi(cpu_line[5]);
    int softirq = std::stoi(cpu_line[6]);
    int steal = std::stoi(cpu_line[7]);
    // int guest = std::stoi(cpu_line[8]);
    // int guest_nice = std::stoi(cpu_line[9]);

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