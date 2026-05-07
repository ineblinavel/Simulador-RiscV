#ifndef LOGGING_H
#define LOGGING_H
#include <string>
using namespace std;

void log_info(const string &message);
void log_error(const string &message);
void log_warning(const string &message);

#endif // LOGGING_H