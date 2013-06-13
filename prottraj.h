#ifndef PROTTRAJ_H
#define PROTTRAJ_H
#include <vector>

#include "cpin.h"
#include "cpout.h"

class ProtTraj {

   public:
      // Constructors
      ProtTraj(Cpin*, float, Record const&);

      // Destructor
//    ~ProtTraj();

      // Adds a point to the trajectory
      void AddPoint(Record const&);

      // Loads a full cpout
      void LoadCpout(CpoutFile);

      // Prints the calcpka-style output
      void PrintCalcpka(FILE*);
      void PrintCalcpka(FILE*, const int);

      // Prints the desired values for each residue for chunks of the simulation
      // of given size (in time steps)
      void PrintChunks(const int, std::string const&, const bool, const bool);

      // Prints the cumulative averages for each residue during the course of
      // the simulation
      void PrintCumulative(std::string const&, const int,
                           const bool, const bool);

      // Prints the running averages for each residue during the course of the
      // simulation
      void PrintRunningAvg(const int, const int, std::string const&,
                           const bool, const bool);

      // Prints a summary of the population of each residue in each state
      void PrintProtPop(std::string const&);

   private:
      // New type: protonation state vector
      typedef std::vector<int> ProtVector;
      typedef ProtVector::const_iterator ProtVectorIterator;

      // Cpin file
      Cpin *cpin_;

      // Store the last point loaded
      ProtVector last_point_;

      // Store every snapshot that was saved
      std::vector<ProtVector> statelist_;

      // Store the number of residues we have
      int nres_;

      // Solvent pH
      float pH_;

      // Number of frames
      long long int nframes_;

      // Monte carlo time step
      int time_step_;

      typedef struct {
         std::vector<long long int> state_cnt;
         std::vector<int> prot_cnt;
         int nstates;
      } StateCount;
      
};
#endif /* PROTTRAJ_H */
