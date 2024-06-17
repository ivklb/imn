
#include "common.hpp"

int get_unique_id() {
    static int id = 0;
    return id++;
}
