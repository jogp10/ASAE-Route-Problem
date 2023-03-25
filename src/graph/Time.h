//
// Created by john on 22-03-2023.
//

#ifndef ASAEINSPECTIONROUTE_TIME_H
#define ASAEINSPECTIONROUTE_TIME_H

#include <utility>
#include <random>
#include <queue>
#include <cmath>
#include <climits>
#include <map>
#include <iostream>
#include <list>
#include <vector>
#include <matplot/matplot.h>

class Time {

public:
    int milliseconds;
    int seconds;
    int minutes;
    int hours;
    int days;

    /**
     * Add time
     * @param milliseconds
     * @param seconds
     * @param minutes
     * @param hours
     * @param days
     */
    void addTime(int milliseconds = 0, int seconds = 0, int minutes = 0, int hours = 0, int days = 0);

    /**
     * Set time
     * @param milliseconds
     * @param seconds
     * @param minutes
     * @param hours
     * @param days
     */
    void setTime(int milliseconds = 0, int seconds = 0, int minutes = 0, int hours = 0, int days = 0);

    /**
     * Add time
     * @param time
     */
    void addTime(Time time);

    /**
     * Compare two times
     * @param rhs
     * @return
     */
    bool operator<(const Time &rhs) const;

    /**
     * Subtract time
     * @param time
     */
    void subTime(Time time);

    /**
     * Subtract time
     * @param milliseconds
     * @param seconds
     * @param minutes
     * @param hours
     * @param days
     */
    void subTime(int milliseconds = 0, int seconds = 0, int minutes = 0, int hours = 0, int days = 0);

    /**
     * Forward time to next hour
     */
    void toNextHour();

    /**
     * Roll back time to previous hour
     */
    void toPreviousHour();

    /**
     * Convert time to seconds
     * @return
     */
    float toSeconds();
};


#endif //ASAEINSPECTIONROUTE_TIME_H
