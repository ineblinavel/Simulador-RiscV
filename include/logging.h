#ifndef LOGGING_H
#define LOGGING_H
#include <string>
#include "cpu.h"

void log_info(const std::string &message);
void log_error(const std::string &message);
void log_warning(const std::string &message);
void log_trace(const CpuState& state);
void print_performance_summary();

#endif // LOGGING_H