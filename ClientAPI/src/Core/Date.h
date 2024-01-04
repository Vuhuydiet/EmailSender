#pragma once
#include <ctime>
#include <unordered_map>

int MonthToInt(const std::string& monthName);
std::string IntToMonth(int monthNumber);

int WDayToInt(const std::string& dayOfWeekName);
std::string IntToWDay(int dayOfWeekNumber);


struct Date {
    int day, month, year;
    int hour, min, sec;
    int dayOfWeek;
    int timezoneOffset;

public:
    Date();

    // format: "%a, %d %b %Y %H:%M:%S %z"
    explicit Date(const std::string& date);

    bool SameDay(const Date& other) const;

    bool operator<(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator<=(const Date& other) const;

};