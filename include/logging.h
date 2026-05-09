#ifndef LOGGING_H
#define LOGGING_H
#include <string>
#include "cpu.h"
using namespace std;

void log_info(const string &message);
void log_error(const string &message);
void log_warning(const string &message);
void log_trace(const CpuState& state);
void print_performance_summary();

#endif // LOGGING_H