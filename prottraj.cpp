// Set up vectors of all of the protonation states for each snapshot

#include <cmath> // log10
#include "prottraj.h"
#include "constants.h"

ProtTraj::ProtTraj(Cpin* cpin, float pH, Record const& recin) :
cpin_(NULL),
nres_(0),
pH_(0.0f),
nframes_(0)
{
   cpin_ = cpin;
   nres_ = cpin->getTrescnt();
   pH_ = pH;

   // Set the first point
   for (int i = 0; i < nres_; i++)
      last_point_.push_back(0);

   for (std::vector<RecordPoint>::const_iterator it = recin.points.begin();
            it != recin.points.end(); it++)
      last_point_[it->residue] = it->state;

   statelist_.push_back(last_point_);
}

/// Loads a full cpout file point by point
void ProtTraj::LoadCpout(CpoutFile cpout) {
   while (!cpout.Done())
      AddPoint(cpout.GetRecord());
}

/// Loads a single frame into the trajectory
void ProtTraj::AddPoint(Record const& recin) {
   for (std::vector<RecordPoint>::const_iterator it = recin.points.begin();
            it != recin.points.end(); it++)
      last_point_[it->residue] = it->state;
   statelist_.push_back(last_point_);
   nframes_++;
}

/// Prints the calcpka-style output
void ProtTraj::PrintCalcpka(FILE *fd, const int start) {

   // First calculate the statistics
   std::vector<long long int> nprot(nres_, 0ll);
   std::vector<int> transitions(nres_, 0);
   long long int totprot = 0ll;

   // Start with the first point, but skip it in the iteration
   last_point_ = statelist_[start];
   for (int i = start + 1; i < nframes_; i++) {
      int j = 0;
      for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
         transitions[j] += (int) (rit->isProtonated(last_point_[j]) !=
                                  rit->isProtonated(statelist_[i][j]));
         long long int protadd = (long long int) (rit->isProtonated(statelist_[i][j]));
         nprot[j] += protadd;
         totprot += (long long int) rit->numProtons(statelist_[i][j]);
         j++;
      }
      // Reassign the last point to the current one before incrementing
      last_point_ = statelist_[i];
   }

   // Now do the printing
   fprintf(fd, "Solvent pH is %8.3f\n", pH_);
   int i = 0;
   for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
      double pKa = pH_ - log10( (double) (nframes_-nprot[i]) / (double) nprot[i] );
      float offset = (float) pKa - pH_;
      fprintf(fd, "%3s %-4d: Offset %6.3f  Pred %6.3lf  Frac Prot %5.3lf Transitions %9d\n",
              rit->getResname().c_str(), rit->getResnum(), offset, (float) pKa,
              (double) nprot[i] / (double) nframes_, transitions[i]);
      i++;
   }
   fprintf(fd, "\nAverage total molecular protonation: %7.3lf\n",
               (double) totprot / (double) nframes_);

   return;
}

void ProtTraj::PrintCalcpka(FILE *fd) {
   PrintCalcpka(fd, 0);
}
