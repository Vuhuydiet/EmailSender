#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <tuple>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <stack>
#include <bitset>
#include <algorithm>
#include <cmath>
#include <stdio.h>
#include <numeric>
#include <typeinfo>
#include <sstream>
#include <iomanip>
#include <time.h>

// ----------------------------------------------------------------------------------------- //

#define _B   1LL
#define _KB  1'000LL
#define _MB  1'000'000LL
#define _GB  1'000'000'000LL

// ----------------------------------------------------------------------------------------- //

#define __ostream_target std::cerr

#define __MAX_OUTPUT_SIZE (500 * _KB)

// ----------------------------------------------------------------------------------------- //

class __Printer {
public:
    template <typename T>
    __Printer& operator<< (const T& __data) {
        if (_counter > _CAPACITY)
            return *this;
        clock_t start = clock();

        std::stringstream buffer;
        this->__print(buffer, __data);
        if (buffer.str().back() != '\n' && _space != -1)
            buffer << _space;
        uint64_t buffer_size = buffer.str().size() * sizeof(char);

        if (_counter + buffer_size <= _CAPACITY) {
            __ostream_target << buffer.str();
            _counter += buffer_size;
        }
        else {
            __ostream_target << "...\n";
            __ostream_target.flush();
            _counter = _CAPACITY + 1;
        }

        clock_t end = clock();
        _debug_time += end - start;
        return *this;
    }

    void _set_space(char space = ' ') { this->_space = space; }

    __Printer() {
        _start = _end = clock();
    }

    ~__Printer() {
        _end = clock();
        __ostream_target << "\n----------------------------------------------\n";
        __ostream_target << "[Time taken]: " << (_end - _start) << " ms\n";
        __ostream_target << "[Time taken without debugging]: " << (_end - _start - _debug_time) << " ms\n";
        __ostream_target.flush();
    }

private:
    template <typename T>
    void __print(std::ostream& out, const T& __data) {
        out << __data;
    }
    template <typename T, typename V>
    void __print(std::ostream& out, const std::pair<T, V>& p) {
        out << "[";
        this->__print(out, p.first);
        out << ", ";
        this->__print(out, p.second);
        out << "]";
    }
    template <typename T>
    void __print(std::ostream& out, const std::vector<T>& v) {
        out << "{ ";
        for (int i = 0; i < v.size(); i++) {
            if (i != 0)
                out << ", ";
            out << "(" << i << ": ";
            this->__print(out, v[i]);
            out << ")";
        }
        out << " }";
    }
    template <typename T>
    void __print(std::ostream& out, const std::set<T>& s) {
        out << "{ ";
        for (auto it = s.cbegin(); it != s.cend(); it++) {
            if (it != s.cbegin())
                out << ", ";
            this->__print(out, *it);
        }
        out << " }";
    }
    template <typename T>
    void __print(std::ostream& out, const std::unordered_set<T>& s) {
        out << "{ ";
        for (auto it = s.cbegin(); it != s.cend(); it++) {
            if (it != s.cbegin())
                out << ", ";
            this->__print(out, *it);
        }
        out << " }";
    }
    template <typename T>
    void __print(std::ostream& out, const std::multiset<T>& s) {
        out << "{ ";
        for (auto it = s.cbegin(); it != s.cend(); it++) {
            if (it != s.cbegin())
                out << ", ";
            this->__print(out, *it);
        }
        out << " }";
    }
    template <typename T, typename V>
    void __print(std::ostream& out, const std::map<T, V>& mp) {
        out << "{ ";
        for (auto it = mp.cbegin(); it != mp.cend(); it++) {
            if (it != mp.cbegin())
                out << ", ";
            this->__print(out, *it);
        }
        out << " }";
    }
    template <typename T, typename V>
    void __print(std::ostream& out, const std::unordered_map<T, V>& mp) {
        out << "{ ";
        for (auto it = mp.cbegin(); it != mp.cend(); it++) {
            if (it != mp.cbegin())
                out << ", ";
            this->__print(out, *it);
        }
        out << " }";
    }

private:
    const uint64_t _CAPACITY = __MAX_OUTPUT_SIZE;
    uint64_t _counter = 0;
    char _space = ' ';

    clock_t _start, _end;
    clock_t _debug_time = 0;

};
inline __Printer __printer;

// ------------------------------------------------------------------------------- //

template <typename T>
__Printer& __printArr(const T& a, int n, const char* name, int line) {
    __printer._set_space(-1);
    __printer << "[" << line << "] " << name << ": { ";
    for (int i = 0; i < n; i++) {
        if (i != 0)
            __printer << ", ";
        __printer << "(" << i << ": " << a[i] << ")";
    }
    __printer << " }\n";
    __printer._set_space();
    return __printer;
}

template <typename T>
inline __Printer& __printMat(const T& a, int n, int m, const char* name, int line) {
    __printer._set_space(-1);
    __printer << "[" << line << "] " << name << ": {\n";

    for (int i = 0; i < n; i++) {
        __printer << "   " << i << ": ";
        for (int j = 0; j < m; j++) {
            if (j != 0)
                __printer << ", ";
            __printer << "(" << j << ": " << a[i][j] << ")";
        }
        __printer << "\n";
    }

    __printer << "}\n";
    __printer._set_space();
    return __printer;
}

template <typename T>
inline __Printer& __print(const T& t, const char* name, int line) {
    __printer._set_space(-1);
    __printer << "[" << line << "] " << name << ": " << t;
    __printer._set_space();
    return __printer;
}

template <typename T>
inline __Printer& __println(const T& t, const char* name, int line) {
    __printer._set_space(-1);
    __print(t, name, line);
    __printer << "\n";
    __printer._set_space();
    return __printer;
}

inline __Printer& __ldb_helper(const char* file, int line) {
    __printer._set_space(-1);
    __printer << "[" << file << "] [" << line << "] ";
    __printer._set_space();
    return __printer;
}


// ------------- API -------------------------------------------------- //

#define __printArr(x, n)        __printArr(x, n, #x, __LINE__)
#define __printMat(x, n, m)     __printMat(x, n, m, #x, __LINE__)
#define __print(x)              __print(x, #x, __LINE__)
#define __println(x)            __println(x, #x, __LINE__)

#define __db                    __printer
#define __ldb                   __ldb_helper(__FILE__, __LINE__)
#define __flag                  __ldb << "[Flag!]\n"