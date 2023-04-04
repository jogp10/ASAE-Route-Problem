#include "Time.h"

void Time::addTime(int milliseconds, int seconds, int minutes, int hours, int days) {
    this->milliseconds += milliseconds;
    this->seconds += seconds;
    this->minutes += minutes;
    this->hours += hours;
    this->days += days;

    if (this->milliseconds >= 1000)
    {
        this->seconds += this->milliseconds / 1000;
        this->milliseconds = this->milliseconds % 1000;
    }
    if (this->seconds >= 60)
    {
        this->minutes += this->seconds / 60;
        this->seconds = this->seconds % 60;
    }

    if (this->minutes >= 60)
    {
        this->hours += this->minutes / 60;
        this->minutes = this->minutes % 60;
    }

    if (this->hours >= 24)
    {
        this->days += this->hours / 24;
        this->hours = this->hours % 24;
    }
}

void Time::setTime(int milliseconds, int seconds, int minutes, int hours, int days) {
    this->milliseconds = milliseconds;
    this->seconds = seconds;
    this->minutes = minutes;
    this->hours = hours;
    this->days = days;
}

void Time::addTime(Time time) {
    this->addTime(time.milliseconds, time.seconds, time.minutes, time.hours, time.days);
}

bool Time::operator<(const Time &rhs) const {
    if (days < rhs.days) return true;
    if (rhs.days < days) return false;
    if (hours < rhs.hours) return true;
    if (rhs.hours < hours) return false;
    if (minutes < rhs.minutes) return true;
    if (rhs.minutes < minutes) return false;
    if (seconds < rhs.seconds) return true;
    if (rhs.seconds < seconds) return false;
    return milliseconds < rhs.milliseconds;
}

void Time::subTime(Time time) {
    this->subTime(time.milliseconds, time.seconds, time.minutes, time.hours, time.days);
}

void Time::subTime(int milliseconds, int seconds, int minutes, int hours, int days) {
    Time aux = {milliseconds, seconds, minutes, hours, days};
    float aux_seconds = aux.toSeconds();
    float this_seconds = this->toSeconds();

    this_seconds -= aux_seconds;

    this->days = (int) this_seconds / 86400;
    this_seconds -= this->days * 86400;
    this->hours = (int) this_seconds / 3600;
    this_seconds -= this->hours * 3600;
    this->minutes = (int) this_seconds / 60;
    this_seconds -= this->minutes * 60;
    this->seconds = (int) this_seconds;
    this_seconds -= this->seconds;
    this->milliseconds = (int) (this_seconds * 1000);
}

void Time::toNextHour() {
    this->hours++;
    if (this->hours >= 24) {
        this->hours = 0;
        this->days++;
    }
    this->minutes = 0;
    this->seconds = 0;
    this->milliseconds = 0;
}

float Time::toSeconds() {
    return (float) this->milliseconds / 1000 + this->seconds + this->minutes * 60 + this->hours * 3600 + this->days * 86400;
}

std::string Time::toString(Time t) {
    std::string time;
    t.addTime(0, 0, 0, 0, 0);
    if (t.days > 0) {
        time += std::to_string(t.days) + "d ";
    }
    if (t.hours > 0) {
        time += std::to_string(t.hours) + "h ";
    }
    if (t.minutes > 0) {
        time += std::to_string(t.minutes) + "m ";
    }
    if (t.seconds > 0) {
        time += std::to_string(t.seconds) + "s ";
    }
    if (t.milliseconds > 0) {
        time += std::to_string(t.milliseconds) + "ms ";
    }
    if (!time.empty()) {
        return time;
    }
    return "0ms";
}
