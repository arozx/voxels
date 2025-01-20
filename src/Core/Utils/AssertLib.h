#ifndef ASSERT_LIBRARY_H
#define ASSERT_LIBRARY_H

#include <iostream>
#include <typeinfo>
#include <string>
#include <stdexcept>
#include <sstream>

namespace AssertLib {

    // Custom exception for failed assertions
    class AssertionError : public std::runtime_error {
    public:
        explicit AssertionError(const std::string& message)
            : std::runtime_error(message) {}
    };

    // Helper function to create error messages
    inline std::string createMessage(const std::string& file, int line, const std::string& msg) {
        std::ostringstream oss;
        oss << "Assertion failed in " << file << ":" << line << ". " << msg;
        return oss.str();
    }

    // Macro for asserting boolean conditions
    #define ASSERT(condition) \
        if (!(condition)) { \
            throw AssertLib::AssertionError(AssertLib::createMessage(__FILE__, __LINE__, "Condition failed: " #condition)); \
        }

    // Template function to assert variable values
    template <typename T>
    void assertEquals(const T& expected, const T& actual, const std::string& file, int line) {
        if (expected != actual) {
            std::ostringstream oss;
            oss << "Expected: " << expected << ", but got: " << actual;
            throw AssertionError(createMessage(file, line, oss.str()));
        }
    }

    // Macro for assertEquals
    #define ASSERT_EQUALS(expected, actual) \
        AssertLib::assertEquals(expected, actual, __FILE__, __LINE__)

    // Template function to assert variable types
    template <typename T, typename U>
    void assertType(const U& value, const std::string& file, int line) {
        if (typeid(value) != typeid(T)) {
            std::ostringstream oss;
            oss << "Expected type: " << typeid(T).name() << ", but got type: " << typeid(value).name();
            throw AssertionError(createMessage(file, line, oss.str()));
        }
    }

    // Macro for assertType
    #define ASSERT_TYPE(expectedType, value) \
        AssertLib::assertType<expectedType>(value, __FILE__, __LINE__)

} // namespace AssertLib

#endif // ASSERT_LIBRARY_H
