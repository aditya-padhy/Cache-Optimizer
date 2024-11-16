#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <string>

extern int totalTests;  // Declare the static variables
extern int passedTests;

class TestFramework {
public:
    static void assertEqual(const char* expected, const char* actual, const std::string& testName);
    static void assertEqual(int expected, int actual, const std::string& testName);
    static void assertTrue(bool condition, const std::string& testName);
    static void report();
};

#endif
