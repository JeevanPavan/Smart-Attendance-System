#include "db_helper.h"
#include <sqlite3.h>
#include <iostream>

static const char* DB_PATH =
"E:/SmartAttendance/SmartAttendance/attendance.db";

void markAttendance(int studentId,
    const std::string& studentName,
    const std::string& timestamp)
{
    sqlite3* db = nullptr;
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        std::cerr << "❌  Can't open DB\n";
        return;
    }

    /* 1️⃣  Make sure the students table exists and holds the name.      */
    const char* createStudents =
        "CREATE TABLE IF NOT EXISTS students ("
        " id INTEGER PRIMARY KEY,"
        " name TEXT NOT NULL);";
    sqlite3_exec(db, createStudents, nullptr, nullptr, nullptr);

    const char* upsert =
        "INSERT OR REPLACE INTO students (id, name) VALUES (?, ?);";
    sqlite3_stmt* s1 = nullptr;
    if (sqlite3_prepare_v2(db, upsert, -1, &s1, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(s1, 1, studentId);
        sqlite3_bind_text(s1, 2, studentName.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(s1);
    }
    sqlite3_finalize(s1);

    /* 2️⃣  Insert the attendance row.                                    */
    const char* insertAtt =
        "INSERT INTO attendance (student_id, timestamp) VALUES (?, ?);";
    sqlite3_stmt* s2 = nullptr;
    if (sqlite3_prepare_v2(db, insertAtt, -1, &s2, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(s2, 1, studentId);
        sqlite3_bind_text(s2, 2, timestamp.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(s2);
    }
    sqlite3_finalize(s2);

    sqlite3_close(db);
}

int getTodaysCount()
{
    sqlite3* db = nullptr;
    sqlite3_stmt* stmt = nullptr;
    int count = 0;

    if (sqlite3_open(DB_PATH, &db) == SQLITE_OK) {
        const char* sql =
            "SELECT COUNT(DISTINCT student_id) "
            "FROM attendance "
            "WHERE DATE(timestamp)=DATE('now', 'localtime');";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK &&
            sqlite3_step(stmt) == SQLITE_ROW)
        {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return count;
}
