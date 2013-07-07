#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
/// Defines the exceptions used in this program

#include <stdexcept>

class InvalidInteger : public std::runtime_error {
   public:
      InvalidInteger(std::string const& s) :
         std::runtime_error(s) {}
};

class InvalidDecimal : public std::runtime_error {
   public:
      InvalidDecimal(std::string const& s) :
         std::runtime_error(s) {}
};

#endif /* EXCEPTIONS_H */
