#pragma once
#include <ctime>
#include <unordered_map>

struct Date {
    int day = 0, month = 0, year = 0;
    int hour = 0, min = 0, sec = 0;
    int dayOfWeek = 0;
    std::string dayOfWeeks, days, months, years, hours, mins, secs;
    std::string timezoneOffset;
    
    int monthToNumber(const std::string& monthName) const {
        static std::map<std::string, int> monthNumberMap = {
            {"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4},
            {"May", 5}, {"Jun", 6}, {"Jul", 7}, {"Aug", 8},
            {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}
        };

        auto it = monthNumberMap.find(monthName);
        return (it != monthNumberMap.end()) ? it->second : 0;
    }

    int dayOfWeekToNumber(const std::string& dayOfWeekName) const {
        static std::map<std::string, int> dayOfWeekNumberMap = {
            {"Sun", 6}, {"Mon", 0}, {"Tue", 1}, {"Wed", 2},
            {"Thu", 3}, {"Fri", 4}, {"Sat", 5}
        };

        auto it = dayOfWeekNumberMap.find(dayOfWeekName);
        return (it != dayOfWeekNumberMap.end()) ? it->second : -1;  // -1 for unknown day of week
    }

    Date() {
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);
        dayOfWeek = localTime->tm_wday;
        day = localTime->tm_mday;
        month = localTime->tm_mon + 1;
        year = localTime->tm_year + 1900;
        hour = localTime->tm_hour;
        min = localTime->tm_min;
        sec = localTime->tm_sec;
        year = localTime->tm_year;
        char buffer[10];

    }

    explicit Date(const std::string& date, const std::string& format = "%a, %d %b %Y %H:%M:%S %z") {
        int ret = sscanf(date.c_str(), format.c_str(), dayOfWeeks, &day, &months, &year, &hour, &min, &sec, &timezoneOffset);
    }


    


    std::string ConcatString(const std::string& format = "%a, %d %b %04d %02d:%02d:%02d") const {
        return dayOfWeeks + " " + days + " " + months + " " + std::to_string(year) + " " +
            std::to_string(hour) + ":" + std::to_string(min) + ":" + std::to_string(sec);
    }

    /*explicit Date(const std::string& date, const std::string& format = "%d/%d/%d %d:%d:%d") {
        int ret = sscanf(date.c_str(), format.c_str(), &day, &month, &year, &hour, &min, &sec);
    }*/

    std::string ToString(const std::string& format = "%02d/%02d/%04d - %02d:%02d:%02d") const {
        char buffer[40];
        sprintf(buffer, format.c_str(), day, month, year, hour, min, sec);
        return std::string(buffer);
    }

    bool operator<(const Date& other) const {
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

    bool operator>(const Date& other) const {
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

    bool operator==(const Date& other) const {
		return year == other.year && month == other.month && day == other.day && hour == other.hour && min == other.min && sec == other.sec;
	}

    bool operator!=(const Date& other) const {
		return !(*this == other);
	}

    bool operator<=(const Date& other) const {
        return *this < other || *this == other;
    }
};