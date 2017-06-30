#ifndef VTZERO_EXCEPTION_HPP
#define VTZERO_EXCEPTION_HPP

/**
 * @file exception.hpp
 *
 * @brief Contains the exceptions used in the vtzero library.
 */

#include <exception>

namespace vtzero {

/**
 * All exceptions explicitly thrown by the functions of the vtzero library
 * derive from this exception.
 */
struct exception : public std::exception {
    /// Returns the explanatory string.
    const char* what() const noexcept override { return "vtzero exception"; }
};

/**
 * This exception is thrown when vector tile encoding isn't valid according to
 * the vector tile specification.
 */
struct format_exception : public exception {
    const char* what() const noexcept override { return "invalid vector tile"; }
};

/**
 * This exception is thrown when a geometry encoding isn't valid according to
 * the vector tile specification.
 */
struct geometry_exception : public format_exception {
    const char* what() const noexcept override { return "invalid vector tile geometry"; }
};

/**
 * This exception is thrown when a tag value is accessed using the wrong type.
 */
struct type_exception : public exception {
    const char* what() const noexcept override { return "wrong tag value type"; }
};

/**
 * This exception is thrown when an unknown version number is found in the
 * layer.
 */
struct version_exception : public exception {
    const char* what() const noexcept override { return "unknown vector tile version"; }
};

} // namespace vtzero

#endif // VTZERO_EXCEPTION_HPP
