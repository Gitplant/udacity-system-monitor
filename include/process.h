#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();
  void SetPid(int pid);
  void SetUptime(float uptime);
  std::string User();
  std::string Command();
  void SetCpuUtilization();
  float GetCpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

  // DONE: Declare any necessary private members
 private:
  int pid_;
  float uptime_;
  float cpu_usage_;
};

#endif