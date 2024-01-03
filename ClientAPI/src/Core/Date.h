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

    // format: "dd/mm/yyyy hh:mm:ss"
    explicit Date(const std::string& date, const std::string& format = "%d/%d/%d %d:%d:%d") {
		int ret = sscanf(date.c_str(), format.c_str(), &day, &month, &year, &hour, &min, &sec);
	}

    // format: "dd/mm/yyyy - hh:mm:ss"
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