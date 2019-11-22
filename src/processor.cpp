#include "processor.h"
#include <fstream>
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
    long total = LinuxParser::Jiffies();
    long active = LinuxParser::ActiveJiffies();
    float cpu = 0.0;
    if (total != m_total) {
        cpu = static_cast<float>(active - m_active) /
              static_cast<float>(total - m_total);
    }
    m_active = active;
    m_total = total;
    return cpu;
}
