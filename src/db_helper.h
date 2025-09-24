#ifndef DB_HELPER_H
#define DB_HELPER_H

#include <string>

void markAttendance(int studentId,
    const std::string& studentName,
    const std::string& timestamp);
int  getTodaysCount();

#endif
