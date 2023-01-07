#ifndef __GANDALF_EXCEPTION_H
#define __GANDALF_EXCEPTION_H

#include <exception>

namespace gandalf {
    class Exception: public std::exception {
    public:
        Exception(const char* message): message_(message) {}
        const char* what() const noexcept override { return message_; }

    private:
        const char* message_;
    };

    class InvalidArgument: public Exception {
    public:
        InvalidArgument(const char* message): Exception(message) {}
    };
} // namespace gandalf

#endif