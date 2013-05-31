#ifndef CPOUT_H
#define CPOUT_H

#include <cstdio>
// To support compression
#include "zlib.h"

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
      
      // See if this cpout file is valid
      bool Valid() { return valid_; }
      bool Done()  { return !valid_ || done_; }

      // Destructor
      ~CpoutFile();

      // Get the next record
      Record GetRecord();

   private:
      // Auto-dispatch
      int Gets(char* c, int i) { if (type_ == ASCII) return AsciiGets(c, i);
                                 if (type_ == GZIP) return GzGets(c, i); }

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

      bool valid_; // Is this a valid file?
      bool done_;  // Is this file done reading?
      float orig_ph_; // pH on the opening full record
      float step_size_; // Monte carlo step size
};

#endif /* CPOUT_H */
