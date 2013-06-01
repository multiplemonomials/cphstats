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
      int nframes_;
};
#endif /* PROTTRAJ_H */
