#pragma once
#include <ctime>
struct Date {
    int day, month, year;
    int hour, min, sec;

    Date() {
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);
        
        day = localTime->tm_mday;
        month = localTime->tm_mon + 1;
        year = localTime->tm_year + 1900;
        hour = localTime->tm_hour;
        min = localTime->tm_min;
        sec = localTime->tm_sec;
    }
};