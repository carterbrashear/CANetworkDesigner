#ifndef CAN_DATABASE_HPP
#define CAN_DATABASE_HPP
#include <sqlite3.h>

class Project {
    sqlite3* db;
public:
    Project();

    void create();
    void open();

    ~Project() = default;
};

#endif //CAN_DATABASE_HPP
