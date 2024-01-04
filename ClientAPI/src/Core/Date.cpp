#include "pch.h"
#include "Date.h"


int MonthToInt(const std::string& monthName) {
    static const std::map<std::string, int> monthNumberMap = {
        { "Jan", 1 }, { "Feb", 2 }, { "Mar", 3 }, { "Apr", 4 },
        { "May", 5 }, { "Jun", 6 }, { "Jul", 7 }, { "Aug", 8 },
        { "Sep", 9 }, { "Oct", 10 }, { "Nov", 11 }, { "Dec", 12 }
    };

    auto it = monthNumberMap.find(monthName);
    return (it != monthNumberMap.end() ? it->second : -1);
}

std::string IntToMonth(int monthNumber) {
    static const std::vector<std::string> months = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
        "Sep", "Oct","Nov", "Dec"
    };

    return months[monthNumber];
}

int WDayToInt(const std::string& dayOfWeekName) {
    static const std::map<std::string, int> dayOfWeekNumberMap = {
        {"Sun", 6}, {"Mon", 0}, {"Tue", 1}, {"Wed", 2},
        {"Thu", 3}, {"Fri", 4}, {"Sat", 5}
    };

    auto it = dayOfWeekNumberMap.find(dayOfWeekName);
    return (it != dayOfWeekNumberMap.end()) ? it->second : -1;
}

std::string IntToWDay(int dayOfWeekNumber) {
    static const std::vector<std::string> days = {
        "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
    };

    return days[dayOfWeekNumber];
}

Date::Date() {
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);

    dayOfWeek = localTime->tm_wday;
    day = localTime->tm_mday;
    month = localTime->tm_mon + 1;
    year = localTime->tm_year + 1900;
    hour = localTime->tm_hour;
    min = localTime->tm_min;
    sec = localTime->tm_sec;

    std::stringstream ss; ss << std::put_time(localTime, "%z");
    timezoneOffset = std::stoi(ss.str()) / 100;
}

Date::Date(const std::string& date) {
    char wday[10] = { '\0' }, mmonth[10] = { '\0' };
    int mday, myear, h, m, s, offset;
    int num_of_token = sscanf(date.c_str(), "%[^,], %d %[^ ] %d %d:%d:%d %d", wday, &mday, mmonth, &myear, &h, &m, &s, &offset);
    if (num_of_token != 8) {
        __ERROR("Invalid date!");
        *this = Date();
        return;
    }

    dayOfWeek = WDayToInt(wday);
    day = mday;
    month = MonthToInt(mmonth);
    year = myear;
    hour = h;
    min = m;
    sec = s;
    timezoneOffset = offset / 100;
}

bool Date::SameDay(const Date& other) const {
    return day == other.day && month == other.month && year == other.year && timezoneOffset == other.timezoneOffset;
}


bool Date::operator<(const Date& other) const {
    if (year != other.year)
        return year < other.year;
    if (month != other.month)
        return month < other.month;
    if (day != other.day)
        return day < other.day;
    if (hour != other.hour)
        return hour < other.hour;
    if (min != other.min)
        return min < other.min;
    return sec < other.sec;
}

bool Date::operator>(const Date& other) const {
    if (year != other.year)
        return year > other.year;
    if (month != other.month)
        return month > other.month;
    if (day != other.day)
        return day > other.day;
    if (hour != other.hour)
        return hour > other.hour;
    if (min != other.min)
        return min > other.min;
    return sec > other.sec;
}

bool Date::operator==(const Date& other) const {
    return year == other.year && month == other.month && day == other.day && hour == other.hour && min == other.min && sec == other.sec;
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

bool Date::operator<=(const Date& other) const {
    return *this < other || *this == other;
}
