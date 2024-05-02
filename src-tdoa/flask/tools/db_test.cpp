// g++ -o db_test db_test.cpp -lsqlite3
// This script inserts a new coordinate location into the db every 10th of a second.

#include <sqlite3.h>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

// connect to db
sqlite3* create_connection(const char* db_file) {
    sqlite3* db;
    int rc = sqlite3_open(db_file, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

// insert a new coordinate into coordinates table
bool insert_coordinate(sqlite3* db, int x, int y) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO coordinates(x, y) VALUES(?,?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, x);
    sqlite3_bind_int(stmt, 2, y);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

int main() {
    const char* database = "../cid.db";

    // connect to db
    sqlite3* db = create_connection(database);
    if (!db) return -1;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 750);  // temp range for box on HTML page

    // insert coordinate into table every 10th of a second
    while (true) {
        int x = distrib(gen);
        int y = distrib(gen);

        if (!insert_coordinate(db, x, y)) {
            std::cerr << "Insert failed" << std::endl;
            break;
        }

        std::cout << "Inserted coordinates (" << x << ", " << y << ")" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    sqlite3_close(db);
    return 0;
}
