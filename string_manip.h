#ifndef STRING_MANIP_H
#define STRING_MANIP_H
// Routines for string manipulation

#include <string>
#include <vector>

using namespace std;

vector<string> split(char*, const char*);
vector<string> split(string const&, const char*);
vector<string> split(string const&, string const&);
vector<string> split(string const&);
vector<string> split(char*);

string strip(string const&);
string strip(char*);

#endif
