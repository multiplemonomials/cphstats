// string_manip.cpp: Contains the routines to do string manipulations

#include <cstring>
#include "string_manip.h"

/// Splits a string along a specific delimiter
vector<string> split(string const& instring, const char* delim) {
   vector<string> result;
   char* pch;
   pch = strtok((char*)instring.c_str(), delim);
   while (pch != NULL) {
      result.push_back( string(pch) );
      pch = strtok(NULL, delim);
   }

   return result;
}

vector<string> split(const char* instring, const char* delim) {
   return split(string(instring), delim);
}

vector<string> split(string const& instring, string const& delim) {
   return split(instring, delim.c_str());
}

vector<string> split(string const& instring) {
   return split(instring, " \n\r");
}
