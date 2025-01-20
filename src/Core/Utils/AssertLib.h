#ifndef ASSERT_LIBRARY_H
#define ASSERT_LIBRARY_H

#include <typeinfo>
#include <stdexcept>

/**
 * @namespace AssertLib
 * @brief A utility namespace providing assertion functionality for debugging and testing
 */

/**
 * @class AssertionError
 * @brief Custom exception class for handling assertion failures
 * @extends std::runtime_error
 */

/**
 * @brief Constructs an AssertionError with a specific error message
 * @param message The error message describing the assertion failure
 */

/**
 * @brief Creates a formatted error message for assertion failures
 * @param file The source file where the assertion failed
 * @param line The line number where the assertion failed
 * @param msg Additional message describing the failure
 * @return std::string Formatted error message
 */

/**
 * @def ASSERT(condition)
 * @brief Macro for checking if a condition is true
 * @param condition The boolean condition to evaluate
 * @throws AssertionError if the condition is false
 */

/**
 * @brief Compares two values for equality
 * @tparam T The type of values being compared
 * @param expected The expected value
 * @param actual The actual value to compare against
 * @param file The source file where the assertion is made
 * @param line The line number where the assertion is made
 * @throws AssertionError if the values are not equal
 */

/**
 * @def ASSERT_EQUALS(expected, actual)
 * @brief Macro for comparing two values for equality
 * @param expected The expected value
 * @param actual The actual value to compare against
 * @throws AssertionError if the values are not equal
 */

/**
 * @brief Verifies that a value is of the expected type
 * @tparam T The expected type
 * @tparam U The type of the value to check
 * @param value The value whose type is being checked
 * @param file The source file where the assertion is made
 * @param line The line number where the assertion is made
 * @throws AssertionError if the type doesn't match the expected type
 */

/**
 * @def ASSERT_TYPE(expectedType, value)
 * @brief Macro for checking if a value is of the expected type
 * @param expectedType The type to check against
 * @param value The value whose type is being checked
 * @throws AssertionError if the type doesn't match the expected type
 */
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
