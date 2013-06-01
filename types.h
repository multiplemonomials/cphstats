#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <map>

#include "cpout.h"

// Define some custom types
typedef std::vector<CpoutFile> CpoutList;
typedef CpoutList::iterator cpout_iterator;

typedef std::map<float, FILE*> RemdMap;

#endif
