#ifndef STRING_MANIP_H
#define STRING_MANIP_H
// Routines for string manipulation

#include <string>
#include <vector>

std::vector<std::string> split(char*, const char*);
std::vector<std::string> split(std::string const&, const char*);
std::vector<std::string> split(std::string const&, std::string const&);
std::vector<std::string> split(std::string const&);
std::vector<std::string> split(char*);

std::string strip(std::string const&);
std::string strip(char*);

std::string upper(std::string const&);
std::string upper(const char*);

#endif
