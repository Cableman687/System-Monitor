#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

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
  string os, kernel, version;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
string line, key;
float value, memtotalfloat, memfreefloat;


std::ifstream stream(kProcDirectory + kMeminfoFilename);

if(stream.is_open()){
  
  while(std::getline(stream,line)){
    std::istringstream linestream(line);
    while(linestream >> key >> value){
      if(key == "MemTotal:"){
        memtotalfloat = value;
      }
      if(key == "MemFree:"){
        memfreefloat = value;
        break;
      }
    }
  }
}
return (memtotalfloat - memfreefloat)/memtotalfloat;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {

long uptime;
  std::string line; 
  std::ifstream filestream(kProcDirectory + kUptimeFilename);

  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long numjiffies = 0;
  std::vector<std::string> cpuUtil = LinuxParser::CpuUtilization();
  for (int i = kUser_; i <= kSteal_; i++) {
    numjiffies = numjiffies + std::stol(cpuUtil[i]);
  }
  return numjiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]){
  std::string pidstring = '/' + std::to_string(pid);
  std::string pid_s = std::to_string(pid);
  vector<long> values;

  std::string key,first,second;
  std::string line;
  long value;
  std::ifstream stream(kProcDirectory + pidstring + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key >> first >> second;
      if(key ==  pid_s){
        while(linestream >> value){
        values.push_back(value);
      }
    }
  }

}
if (values.size()>14){
  long utime = values[10];
  long stime = values[11];
  long cutime = values[12];
  long cstime = values[13];

  long active = utime + stime + cutime +cstime;

  return active;
}
else{
  return 0;
}
}


// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{
       
  long idlecomponent = 0;
  std::vector<std::string> cpuUtil = LinuxParser::CpuUtilization();
  for (int i = kIdle_; i <= kIOwait_; i++) {
    idlecomponent = idlecomponent + std::stol(cpuUtil[i]);
  }
  return idlecomponent;

}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
return Jiffies() - IdleJiffies();
}

// Read and return the CPU Utlization for the System
vector<string> LinuxParser::CpuUtilization() 
{ 
  string line,key, value;
  vector<string> cpuUtil;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      while(linestream >> key){
        if(key == "cpu"){
          for(int i = 0; i<10; i++){
            linestream >> value;
            cpuUtil.push_back(value);
          }
        }
      }
    }
  }
  return cpuUtil;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line,key;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key=="processes"){
          return value;
        }        
      }
    } 
  }
  return 0.0;
}



// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line,key;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key=="procs_running"){
          return value;
        }        
      }
    } 
  }
  return 0;  
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
string command,line;
std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
if(stream.is_open()){
  std::getline(stream,line);
  std::istringstream linestream(line);
  linestream >> command;
}
return command;


}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line,ram ,value,key;
  
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key;
      if(key == "VmSize:"){
        linestream >> value;
        float memory = stof(value)/1000.0;
        std::stringstream sstream;
        sstream << std::fixed << std::setprecision(2) << memory;
        return sstream.str();
      }
    }
  }
  return string();
  }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  
string line,key,value;

std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
if(stream.is_open()){
  while(std::getline(stream,line)){
    std::istringstream linestream(line);
    while(linestream >> key >> value){
      if(key == "Uid:"){
        return value;
        break;
      }
    }
  }
}
return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
string line,user,password,uid;

std::ifstream stream(kPasswordPath);
if(stream.is_open()){
  while(std::getline(stream,line)){
    std::replace(line.begin(),line.end(),':',' ');
    std::istringstream linestream(line);
    while(linestream>>user>>password>>uid){
      if(uid == LinuxParser::Uid(pid)){
        return user;
        
      }
      
    }
  }
  
}
return string();

}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  float timer = sysconf(_SC_CLK_TCK);

  string line, value;
  
  vector<string> timestates;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);

    for(int i = 0; i <21; i++){
      linestream >> value;
    }
    linestream >> value;
    return stol(value) / timer;
  }
return long();

}


