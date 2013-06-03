#ifndef CLOPTIONS_H
#define CLOPTIONS_H

#define VERSION_STR "0.5"

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
      std::vector<std::string> Cpouts() const { return cpouts_;         }
      int Verbose() const                     { return verbose_;        }
      std::string Cpin() const                { return cpin_;           }
      std::string Output() const              { return output_;         }
      bool Calcpka() const                    { return do_calcpka_;     }
      std::string CumulativeOutput() const    { return cumout_;         }
      bool doCumulative() const               { return cumulative_;     }
      std::string RunningAvgOutput() const    { return runavgout_;      }
      std::string Prog() const                { return prog_;           }
      std::string ChunkOutput() const         { return chunkout_;       }
      bool Overwrite() const                  { return overwrite_;      }
      int RunningAvgWindow() const            { return runavgwin_;      }
      int ChunkWindow() const                 { return chunksize_;      }
      int Interval() const                    { return interval_;       }
      bool PrintProtonated() const            { return protonated_;     }
      bool PrintDeprotonated() const          { return !protonated_;    }
      std::string REMDPrefix() const          { return reorder_prefix_; }
      float TimeStep() const                  { return time_step_;      }
      bool pKa() const                        { return pKa_;            }

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
      /// Time step used in the simulation
      float time_step_;
      /// Do we put predicted pKas for time series?
      bool pKa_;

};

#endif /* CLOPTIONS_H */
