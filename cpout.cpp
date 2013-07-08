// cpout.cpp: Includes code to deal with and parse cpout files

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "cpout.h"
#include "exceptions.h"
#include "string_manip.h"

using namespace std;

CpoutFile::CpoutFile(string const& fname) :
fp_(NULL),
type_(ASCII),
valid_(true),
done_(false),
orig_ph_(0.0f),
step_size_(0),
nres_(0)
{
   
   filename_ = fname;
   if (fname.find_last_of(".") != string::npos) {
      // Get the suffix
      string sfx = fname.substr(fname.find_last_of('.'));
      if (sfx == string(".bz2")) {
         type_ = BZIP;
         cerr << "Error: BZIP2 compression not supported!" << endl;
         valid_ = false;
     }else if (sfx == string(".gz"))
         type_ = GZIP;
   }

   // Open up the appropriate file (ASCII or gzip)
   if (type_ == ASCII) {
      fp_ = fopen(fname.c_str(), "r");
      if (fp_ == NULL) {
         cerr << "Failed opening " << fname << " for reading." << endl;
         valid_ = false;
      }
         
  }else if (type_ == GZIP) {
      gzfp_ = gzopen(fname.c_str(), "r");
      if (gzfp_ == NULL) {
         cerr << "Failed opening gzip file " << fname << endl;
         valid_ = false;
      }
   }
   // Parse out the first (full) record to determine some information
   char buf[LINEBUF+1];
   if (valid_ && Gets(buf, LINEBUF)) {
      cerr << "Could not read from " << fname << endl;
      Close();
      valid_ = false;
  }else if (valid_) {
      if (sscanf(buf, "Solvent pH: %f\n", &orig_ph_) == 1) {
         Gets(buf, LINEBUF);
         if (sscanf(buf, "Monte Carlo step size: %d\n", &step_size_) != 1) {
            cerr << "Did not recognize the format of cpout " << fname << "." << endl;
            valid_ = false;
            Close();
         }
         Gets(buf, LINEBUF); // Time step:
         Gets(buf, LINEBUF); // Time
         // Get the starting time
         if (valid_ && sscanf(buf, "Time: %f\n", &start_time_) != 1) {
            cerr << "Did not recognize format of cpout " << fname << "." << endl;
            valid_ = false;
            Close();
         }

         if (valid_) {
            // I'm satisfied it's a valid cpout here; now come the residues
            Gets(buf, LINEBUF);
            int res;
            int state;
            float pH;
            nres_ = 0;
            while (sscanf(buf, "Residue %d State: %d pH: %f\n", &res, &state, &pH) >= 2) {
               nres_++;
               Gets(buf, LINEBUF);
            }
            Rewind();
         }
     }else {
         cerr << "Did not recognize format of cpout " << fname << "." << endl;
         Close();
         valid_ = false;
      }
   }
}

CpoutFile::CpoutFile(const char* fname) {
   CpoutFile(string(fname));
}

int CpoutFile::GzGets(char* str, int num) {
   if (gzgets(gzfp_, str, num) == NULL)
      return 1;
   return 0;
}

int CpoutFile::AsciiGets(char* str, int num) {
   if (fgets(str, num, fp_) == NULL) {
      return 1;
   }
   return 0;
}

Record CpoutFile::GetRecord() {
   char buf[LINEBUF+1];

   // Create an empty record for return
   Record empty_record;
   empty_record.pH = 0.0f; empty_record.full = false;
   empty_record.time_step = 0; empty_record.time = 0.0f;

   if (Gets(buf, LINEBUF)) {
      done_ = true;
      return empty_record;
   }
   Record result;
   float pH;
   int res;
   int state;
   result.full = false;
   if (sscanf(buf, "Solvent pH: %f\n", &pH) == 1) {
      result.full = true;
      result.pH = pH;
      Gets(buf, LINEBUF); // Monte Carlo step size
      Gets(buf, LINEBUF); // Time step:
      sscanf(buf, "Time step: %d\n", &result.time_step);
      Gets(buf, LINEBUF); // Time:
      sscanf(buf, "Time: %f\n", &result.time);
      Gets(buf, LINEBUF); // Residue
      while (sscanf(buf, "Residue %d State: %d pH: %f\n", &res, &state, &pH) >= 2) {
         RecordPoint pt;
         pt.state = state; pt.residue = res;
         result.points.push_back(pt);
         if (Gets(buf, LINEBUF)) {
            done_ = true;
            Close();
            return result;
         }
      }
      return result;
  }else {
      int s = sscanf(buf, "Residue %d State: %d pH: %f\n", &res, &state, &pH);
      RecordPoint pt;
      pt.state = state; pt.residue = res;
      result.points.push_back(pt);
      if (Gets(buf, LINEBUF)) {
         done_ = true;
         Close();
         return result;
      }
      // If this is a REMD run, assign the pH
      if (s == 3)
         result.pH = pH;
      // Get more residues from a multi-site move
      while (sscanf(buf, "Residue %d State: %d pH: %f\n", &res, &state, &pH) >= 2) {
         RecordPoint opt;
         opt.state = state; opt.residue = res;
         result.points.push_back(opt);
         if (Gets(buf, LINEBUF)) {
            done_ = true;
            Close();
            return result;
         }
      }
      return result;
   }

   throw InternalError("Cpout parsing error: should not be here");

   // Will never get here, but need this to eliminate compiler warnings
   return empty_record;
}
