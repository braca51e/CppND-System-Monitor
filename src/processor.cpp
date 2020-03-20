#include <vector>
#include <fstream>
#include <iostream>
#include <numeric>

#include "processor.h"
#include "linux_parser.h"
#include "unistd.h"


// TODO: Return the aggregate CPU utilization
std::vector<float> getCPUTimes_(){
  std::vector<float> times;
  std::ifstream filestream(LinuxParser::kProcDirectory+LinuxParser::kStatFilename);
  filestream.ignore(5, ' '); //skip cpu
  for(float time; filestream >> time; times.emplace_back(time));
  return times;
}

void updateCPUTimes_(float &idletime, float &totaltime){
  std::vector<float> cputimes = getCPUTimes_();
  idletime = cputimes.at(3);
  totaltime = std::accumulate(cputimes.begin(), cputimes.end(), 0.0);
}

float Processor::Utilization() {
    unsigned int wait = 2; 
    float totalutil = 0;
    float previdletime = 0, prevtotaltime = 0;
    float idletime = 0, totaltime = 0;
    float idletimedelta, totaltimedelta;
    while(wait > 0) {
        updateCPUTimes_(idletime, totaltime);
        idletimedelta = idletime - previdletime;
        totaltimedelta = totaltime - prevtotaltime;
        totalutil = (1.0 - idletimedelta / totaltimedelta);
        previdletime = idletime;
        prevtotaltime = totaltime;
        wait -= 1;
        sleep(wait);
    }

    return totalutil;
}
