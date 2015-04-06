//
// Created by tracymac on 4/3/15.
//

#include "time_utils.h"

std::string& time_utils::TimeUtil::UTC2Local(const time_t *utc) {
    //return e.g. "2015-04-02 00:06:08"
    char buf[128];
    tm *ltm = localtime(utc);
    ltm->tm_year += 1900;

    if (++ltm->tm_hour >= 24) {
        ltm->tm_hour -= 24;
        ltm->tm_mday ++;
    }

    ltm->tm_mon++;

    sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d", ltm->tm_year, ltm->tm_mon, ltm->tm_mday,
           ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

    return *(new std::string(buf));
}