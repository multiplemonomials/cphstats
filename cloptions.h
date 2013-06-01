#ifndef CLOPTIONS_H
#define CLOPTIONS_H

#define VERSION_STR "0.1a"

#include <cstring>
#include <string>
#include <vector>

class CLOptions {
   public:
      enum RetType {OK=0, HELP, VERSION, ERR, INSUFARGS};

      CLOptions(int, char**);

      /// This command finalizes the parsing
      int Parse();

      /// This prints out the help message
      void Help();

      /// This prints out the usage statement
      void Usage();

      /// This prints out the version string
      void Version();

      /// Checks that all input is valid
      int CheckInput();

      /// These functions are getters for the various options
      std::vector<std::string> Cpouts(){ return cpouts_;         }
      int Verbose()                    { return verbose_;        }
      std::string Cpin()               { return cpin_;           }
      std::string Output()             { return output_;         }
      bool Calcpka()                   { return do_calcpka_;     }
      std::string CumulativeOutput()   { return cumout_;         }
      bool doCumulative()              { return cumulative_;     }
      std::string RunningAvgOutput()   { return runavgout_;      }
      std::string Prog()               { return prog_;           }
      std::string ChunkOutput()        { return chunkout_;       }
      bool Overwrite()                 { return overwrite_;      }
      int RunningAvgWindow()           { return runavgwin_;      }
      int ChunkWindow()                { return chunksize_;      }
      int Interval()                   { return interval_;       }
      bool PrintProtonated()           { return protonated_;     }
      bool PrintDeprotonated()         { return !protonated_;    }
      std::string REMDPrefix()         { return reorder_prefix_; }

      // Provide an iterator over the cpouts
      typedef std::vector<std::string>::const_iterator cpout_iterator;
      cpout_iterator begin()     { return cpouts_.begin(); }
      cpout_iterator end()       { return cpouts_.end();   }

   private:
      /// Name of the input cpin file
      std::string cpin_;
      /// Name of the input cpout files
      std::vector<std::string> cpouts_;
      /// Name of the program
      std::string prog_;
      /// Status of the parsing
      RetType parse_return_;
      /// Verbosity level
      int verbose_;
      /// Calcpka-style output
      std::string output_;
      /// running average output file
      std::string runavgout_;
      /// File for cumulative output
      std::string cumout_;
      /// File for chunk output file
      std::string chunkout_;
      /// Do we write a calcpka-style output?
      bool do_calcpka_;
      /// Do I do a cumulative avg of prot. frac.?
      bool cumulative_;
      /// Running average window size
      int runavgwin_;
      /// Chunk window size
      int chunksize_;
      /// prefix for REMD file reordering
      std::string reorder_prefix_;
      /// Do we allow file overwriting?
      bool overwrite_;
      /// Frequency that we print time series data
      int interval_;
      /// Do we print out protonated statistics?
      bool protonated_;

};

#endif /* CLOPTIONS_H */
