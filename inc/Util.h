#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <sstream>
#include <stdexcept>
#include <string>

#include <stdio.h>

// allows for super class referencing without
// having to care about the type name.
#define DECLARE_SUPER_CLASS(super_class) \
	private: typedef super_class __super;

// for convenient and verbose output of throw_if
// exeption messages.
#ifndef YOLO
#define SOURCE_LOCATION \
	__FILE__, "@", std::to_string (__LINE__), ": "
#define THROW_IF(premise, ...) \
	blurryroots::util::throw_if (premise, SOURCE_LOCATION, ##__VA_ARGS__)
#else
#define TROW_IF(premise, ...) \
	if (false) {}
#endif

// for logging debug and warning messages
#ifdef DEBUG
#define DEBUG_SOURCE_LOCATION \
	std::string (__FILE__) + "@" + std::to_string (__LINE__)

#define LOG_PREFIX "LOG"
#define DEBUG_LOG(msg) \
	blurryroots::util::log (LOG_PREFIX, DEBUG_SOURCE_LOCATION, "%s", msg)
#define DEBUG_LOG(msg, ...) \
	blurryroots::util::log (LOG_PREFIX, DEBUG_SOURCE_LOCATION, msg, ##__VA_ARGS__)

#define WARNING_PREFIX "WARNING: "
#define DEBUG_WARN(msg) \
	blurryroots::util::log (WARNING_PREFIX, DEBUG_SOURCE_LOCATION, "%s", msg)
#define DEBUG_WARN(msg, ...) \
	blurryroots::util::log (WARNING_PREFIX, DEBUG_SOURCE_LOCATION, msg, ##__VA_ARGS__)
#else
#define LOG(msg) \
	if (false) {}
#define LOG(msg, ...) \
	if (false) {}
#endif

namespace blurryroots { namespace util {

// if given promise is true, throw a runtime_error
template<class... TArgs> static void
throw_if (bool premise, TArgs... args) {
	// if the error condition is not met, abort.
	if (! premise) {
		return;
	}

	// unpack all further arguments.
	std::vector<std::string> arguments {
		args...
	};

	// collect all arguments into a stringstream.
	std::stringstream ss;
	for (auto &a : arguments) {
		ss << a;
	}
	ss << std::endl;

	// throw runtime error, with collected string as msg.
	throw std::runtime_error (ss.str ());
}

// logging
template<class... TArgs> static void
log (std::string prefix, std::string location, std::string msg, TArgs... args) {
	// collect all arguments into a stringstream.
	std::stringstream ss;
	ss << prefix << " (" << location << "): " << msg << std::endl;
	std::string format_string = ss.str ();

	const char *format_cstring = format_string.c_str ();
	printf (format_cstring, args...);
}

}}

#endif
