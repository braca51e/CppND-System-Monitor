#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
  string linux_, version_, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> linux_ >> version_ >> kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {

  string const search_str{"Mem"};
  float memTotal, memFree, memAvailable, memFloat;
  string key;
  std::vector<float> mems;
  string line;
  std::ifstream filestream(kProcDirectory+kMeminfoFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> memFloat;
      auto it = std::search(key.begin(), key.end(), search_str.begin(), search_str.end());
      if(it == key.end()){
        break;
      }
      mems.emplace_back(memFloat);
    }
  }
  memAvailable = mems.back();
  mems.pop_back();
  memFree = mems.back();
  mems.pop_back();
  memTotal = mems.back();
  mems.pop_back();
  
  return (memTotal - memFree)/memTotal;
}

// TODO: Read and return the system uptime
long int LinuxParser::UpTime() { 
  long int uptime;
  string line;
  std::ifstream filestream(kProcDirectory+kUptimeFilename);
  if(filestream.is_open()){
    if(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> uptime;
    }
  }
  return uptime*0.01;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 

  std::vector<long> jiffiess;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  filestream.ignore(5, ' '); //skip cpu
  for(long jiffy; filestream >> jiffy; jiffiess.emplace_back(jiffy));

  return std::accumulate(jiffiess.begin(), jiffiess.end(), 0.0);

}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long totalJiffies = Jiffies();
  long idleJiffies = IdleJiffies();
  
  return totalJiffies - idleJiffies; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {

  std::vector<long> jiffiess;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  filestream.ignore(5, ' '); //skip cpu
  for(long jiffy; filestream >> jiffy; jiffiess.emplace_back(jiffy));
  return jiffiess.at(3); 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  
  std::vector<string> utils;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  filestream.ignore(5, ' '); //skip cpu
  for(string util; filestream >> util; utils.emplace_back(util));
  return utils;

}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int processes;
  string const search_str{"processes"};
  std::ifstream filestream(kProcDirectory+kStatFilename);
  string line;
  string tmp;
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> tmp >> processes;
      auto it = std::search(tmp.begin(), tmp.end(), search_str.begin(), search_str.end());
      if(it != tmp.end()){
        break;
      }
    }
  }
  return processes; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int running;
  string const search_str{"procs_running"};
  std::ifstream filestream(kProcDirectory+kStatFilename);
  string line;
  string tmp;
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> tmp >> running;
      auto it = std::search(tmp.begin(), tmp.end(), search_str.begin(), search_str.end());
      if(it != tmp.end()){
        break;
      }
    }
  }
  return running;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { 
  std::ifstream filestream(kProcDirectory+std::to_string(pid)+kCmdlineFilename);
  string line;
  if(filestream.is_open()){
    while(std::getline(filestream, line));
  }

  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  std::ifstream filestream(kProcDirectory+std::to_string(pid)+kStatusFilename);
  string line;
  float size = 0;
  string const memstr("VmSize:");
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      auto it = std::search(line.begin(), line.end(), memstr.begin(), memstr.end());
      if(it != line.end()){
        std::cout << line << "\n";
        linestream.ignore(7, ' ');
        linestream >> size;
        std::cout << size << "\n";
        size *= 0.001;
        break;
      }
    }
  }

  return std::to_string(size); 

}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  std::ifstream filestream(kProcDirectory+std::to_string(pid)+kStatusFilename);
  string line, uid;
  string const uidstr("Uid:");
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      auto it = std::search(line.begin(), line.end(), uidstr.begin(), uidstr.end());
      if(it != line.end()){
        linestream.ignore(5, ' ');
        linestream >> uid;
        break;
      }
    }
  }

  return uid; 

}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  std::ifstream filestream(kPasswordPath);
  std::string uidstr = LinuxParser::Uid(pid);
  string line;
  string user;
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      auto it = std::search(line.begin(), line.end(), uidstr.begin(), uidstr.end());
      if(it != line.end()){
         linestream >> user;
         break;
      }
    }
  }
  
  return user; 
  
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }