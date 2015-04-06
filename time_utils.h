//
// Created by tracymac on 4/3/15.
//

#ifndef CNP_ATM_TIME_UTILS_H
#define CNP_ATM_TIME_UTILS_H

#include <string>
#include <ctime>

namespace time_utils {
    class TimeUtil {
    public:
        static std::string &UTC2Local(const time_t *utc);
    };
}

#endif //CNP_ATM_TIME_UTILS_H
