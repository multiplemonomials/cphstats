#ifndef CLOPTIONS_H
#define CLOPTIONS_H

#define VERSION_STR "0.1a"

#include <cstring>
#include <string>
#include <vector>

using namespace std;

class CLOptions {
   public:
      enum RetType {OK=0, HELP, VERSION, ERR, INSUFARGS};

      CLOptions(int, char**);

      /// This command finalizes the parsing
      int Parse();

      /// This prints out the help message
      void Help();

      /// This prints out the version string
      void Version();

      /// Checks that all input is valid
      int CheckInput();

      /// These functions are getters for the various options
      int Verbose()              { return verbose_;   }
      vector<string> Cpouts()    { return cpouts_;    }
      string Cpin()              { return cpin_;      }
      string Output()            { return output_;    }
      bool Calcpka()             { return do_calcpka_;}
      string CumulativeOutput()  { return cumout_;    }
      bool doCumulative()        { return cumulative_;}
      string RunningAvgOutput()  { return runavgout_; }
      string Prog()              { return prog_;      }
      string ChunkOutput()       { return chunkout_;  }
      bool Overwrite()           { return overwrite_; }
      int RunningAvgWindow()     { return runavgwin_; }
      int ChunkWindow()          { return chunksize_; }
      string REMDPrefix()        { return reorder_prefix_; }

      // Provide an iterator over the cpouts
      typedef vector<string>::const_iterator cpout_iterator;
      cpout_iterator begin()     { return cpouts_.begin(); }
      cpout_iterator end()       { return cpouts_.end();   }

   private:
      /// Name of the input cpin file
      string cpin_;
      /// Name of the input cpout files
      vector<string> cpouts_;
      /// Name of the program
      string prog_;
      /// Status of the parsing
      RetType parse_return_;
      /// Verbosity level
      int verbose_;
      /// Calcpka-style output
      string output_;
      /// running average output file
      string runavgout_;
      /// File for cumulative output
      string cumout_;
      /// File for chunk output file
      string chunkout_;
      /// Do we write a calcpka-style output?
      bool do_calcpka_;
      /// Do I do a cumulative avg of prot. frac.?
      bool cumulative_;
      /// Running average window size
      int runavgwin_;
      /// Chunk window size
      int chunksize_;
      /// prefix for REMD file reordering
      string reorder_prefix_;
      /// Do we allow file overwriting?
      bool overwrite_;

};

#endif /* CLOPTIONS_H */
