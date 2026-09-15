#ifndef CAN_DATABASE_HPP
#define CAN_DATABASE_HPP
#include <sqlite3.h>

class CanDataBase {
    sqlite3* db;
public:
    CanDataBase();

    void create();
    void open();

    ~CanDataBase() = default;
};

#endif //CAN_DATABASE_HPP
