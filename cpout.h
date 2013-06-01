#ifndef CPOUT_H
#define CPOUT_H

#include <cstdio>
#include <vector>
#include <string>

// To support compression
#include "zlib.h"

#include "constants.h"

typedef struct {
   int residue;
   int state;
} RecordPoint;

typedef struct {
   std::vector<RecordPoint> points;
   float pH;
   bool full;
} Record;

class CpoutFile {
   public:
      enum FileType {ASCII=0, BZIP, GZIP};

      // Constructors
      CpoutFile(std::string const&);
      CpoutFile(const char*);
      
      // Getters
      bool Valid()   { return valid_;           }
      bool Done()    { return !valid_ || done_; }
      int Nres()     { return nres_;            }
      int StepSize() { return step_size_;       }
      float pH()     { return orig_ph_;         }

      std::string Filename() { return filename_; }

      // Destructor
//    ~CpoutFile();

      // Get the next record
      Record GetRecord();

   private:
      // Auto-dispatch
      int Gets(char* c, int i) { if (type_ == ASCII) return AsciiGets(c, i);
                                 if (type_ == GZIP) return GzGets(c, i); 
                                 return 1;}
      void Rewind() { if (type_ == ASCII) fseek(fp_, 0, SEEK_SET);
                      if (type_ == GZIP) gzseek(gzfp_, 0, SEEK_SET); }

      void Close() { if (type_ == ASCII) fclose(fp_);
                     if (type_ == GZIP) gzclose(gzfp_); }
      // Real methods
      int GzGets(char*, int);
      int AsciiGets(char*, int);

      // File objects
      FILE *fp_;
      gzFile gzfp_;

      // File type (ASCII? Gzip?)
      FileType type_;

      std::string filename_; // Original file name
      bool valid_; // Is this a valid file?
      bool done_;  // Is this file done reading?
      float orig_ph_; // pH on the opening full record
      int step_size_; // Monte carlo step size
      int nres_;      // Number of residues defined in this cpout
};

#endif /* CPOUT_H */
