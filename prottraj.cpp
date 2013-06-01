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
   std::vector<float> nprot(nres_, 0.0f);
   std::vector<int> transitions(nres_, 0);
   float totprot = 0.0f;

   // Start with the first point, but skip it in the iteration
   last_point_ = statelist_[start];
   for (int i = start + 1; i < nframes_; i++) {
      int j = 0;
      for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
         transitions[j] += (int) (rit->isProtonated(last_point_[j]) !=
                                  rit->isProtonated(statelist_[i][j]));
         float protadd = (float) (rit->isProtonated(statelist_[i][j]));
         nprot[j] += protadd;
         totprot += protadd;
         j++;
      }
      // Reassign the last point to the current one before incrementing
      last_point_ = statelist_[i];
   }

   // Take averages
   totprot /= (float) nframes_;
   for (int i = 0; i < nres_; i++)
      nprot[i] /= (float) nframes_;

   // Now do the printing
   fprintf(fd, "Solvent pH is %8.3f\n", pH_);
   int i = 0;
   for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
      float pKa = pH_ - log10( (1-nprot[i]) / nprot[i] );
      float offset = pKa - pH_;
      fprintf(fd, "%3s %-4d: Offset %6.3f  Pred %6.3f  Frac Prot %5.3f Transitions %9d\n",
              rit->getResname().c_str(), rit->getResnum(), offset, pKa,
              nprot[i], transitions[i]);
      i++;
   }
   fprintf(fd, "\nAverage total molecular protonation: %7.3f\n", totprot);

   return;
}

void ProtTraj::PrintCalcpka(FILE *fd) {
   PrintCalcpka(fd, 0);
}
