#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // DONE: See src/processor.cpp

  // DONE: Declare any necessary private members
 private:
    int prev_total_ = 0;
    int prev_idle_ = 0;
};

#endif