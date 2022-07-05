#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {

float totalutil;
long activejiffies = LinuxParser::ActiveJiffies();
long jiffies = LinuxParser::Jiffies();

totalutil = float(activejiffies) / float(jiffies);

return totalutil;

}