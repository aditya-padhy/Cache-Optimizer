#include "test.h"  // Include the header to use the declarations
#include <cstring>

// Define the static variables **only once** in test.cpp
int totalTests = 0;
int passedTests = 0;

void TestFramework::assertEqual(const char* expected, const char* actual, const std::string& testName) {
    if (strcmp(expected, actual) == 0) {
        std::cout << testName << " PASSED\n";
    } else {
        std::cout << testName << " FAILED: Expected \"" << expected << "\", but got \"" << actual << "\"\n";
    }
}

void TestFramework::assertEqual(int expected, int actual, const std::string& testName) {
    if (expected == actual) {
        std::cout << testName << " PASSED\n";
    } else {
        std::cout << testName << " FAILED: Expected " << expected << ", but got " << actual << "\n";
    }
}

void TestFramework::assertTrue(bool condition, const std::string& testName) {
    totalTests++;
    if (condition) {
        passedTests++;
        std::cout << "[PASSED] " << testName << std::endl;
    } else {
        std::cout << "[FAILED] " << testName << ": Condition was false" << std::endl;
    }
}

void TestFramework::report() {
    std::cout << "\nTotal Tests: " << totalTests << ", Passed: " << passedTests << std::endl;
    if (totalTests == passedTests) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << "Some tests failed!" << std::endl;
    }
}
