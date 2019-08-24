#pragma once
#include <string>
#include <vector>
#include <xutility>
#include <stdexcept>
#include <iostream>

using namespace std;
extern vector<pair<string, void(*)()>> test_cases;

class AssertionError : public exception {
public:
    AssertionError(const string& msg) : exception(msg.c_str()) {};
};

inline void expect(bool b, const string& msg) {
    if (!b) throw AssertionError(msg);
}

#define TEST_CASE(name) void name(); static auto t_##name = test_cases.insert(test_cases.end(), pair<string, void(*)()>(#name, name)); void name()