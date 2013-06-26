// prottraj.cpp: Set up vectors of all of the protonation states for each snapshot

#include <cmath> // log10
#include "prottraj.h"
#include "constants.h"

// Some simple binary sorting macros
#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a > b ? b : a)

ProtTraj::ProtTraj(Cpin* cpin, float pH, Record const& recin) :
cpin_(NULL),
nres_(0),
pH_(0.0f),
nframes_(0),
time_step_(0)
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
   if (time_step_ == 0) time_step_ = cpout.StepSize();
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

/// Prints the calcpka-style output for the entire simulation
void ProtTraj::PrintCalcpka(FILE *fd) {
   PrintCalcpka(fd, 0);
}

/// Print a time series of the desired properties in 'chunks' of the simulation
void ProtTraj::PrintChunks(const int window, std::string const& fname,
                           const bool print_prot, const bool print_pka) {

   // Open up the file and write a header
   FILE *fp = fopen(fname.c_str(), "w");
   fprintf(fp, "#Time step ");
   for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
      char buf[LINEBUF];
      sprintf(buf, "%s %d", rit->getResname().c_str(), rit->getResnum());
      fprintf(fp, "%8s ", buf);
   }
   fprintf(fp, "Total Frac. Prot.\n");

   int interval = window / time_step_;
   int first = 0;
   while (first + interval <= nframes_) {
      std::vector<long long int> nprot(nres_, 0ll);
      long long int totprot = 0ll;
      for (int i = first; i < first + interval; i++) {
         int j = 0;
         for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
            long long int protadd = (long long int) (rit->isProtonated(statelist_[i][j]));
            nprot[j] += protadd;
            totprot += (long long int) rit->numProtons(statelist_[i][j]);
            j++;
         }
      }
      // Now write the information
      int j = 0;
      fprintf(fp, "%10i ", time_step_*(first + interval / 2));
      for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
         double fracprot = (double)nprot[j] / (double)interval;
         if (print_prot) {
            // We want the fraction protonated
            fprintf(fp, "%8.5lf ", fracprot);
        }else if (print_pka) {
            // We want the pKa
            double pKa = pH_ - log10( (1.0 - fracprot) / fracprot );
            fprintf(fp, "%8.4lf ", pKa);
        }else {
            // We want the fraction deprotonated
            fprintf(fp, "%8.5lf ", 1.0-fracprot);
         }
         j++;
      }
      fprintf(fp, "%17.6f\n", (double)totprot / (double)interval);
      first += interval;
   }

   // Now that I'm here (and done), close the file
   fclose(fp);
   return;
}

/// Prints a cumulative running average time series of the desired property
void ProtTraj::PrintCumulative(std::string const& fname, const int interval,
                               const bool print_prot, const bool print_pka) {
   
   // Open up the file and write a header
   FILE *fp = fopen(fname.c_str(), "w");
   fprintf(fp, "#Time step ");
   for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
      char buf[LINEBUF];
      sprintf(buf, "%s %d", rit->getResname().c_str(), rit->getResnum());
      fprintf(fp, "%8s ", buf);
   }
   fprintf(fp, "Total Frac. Prot.\n");

   // Now go through the trajectory
   std::vector<long long int> nprot(nres_, 0ll);
   long long int totprot = 0ll;
   int c = 0; // simple counter
   for (int i = 0; i < nframes_; i++) {
      
      { // scope this
      int j = 0;
      for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
         long long int protadd = (long long int) (rit->isProtonated(statelist_[i][j]));
         nprot[j] += protadd;
         totprot += (long long int) rit->numProtons(statelist_[i][j]);
         j++;
      }
      }

      if (c * time_step_ >= interval) {
         c = 0;
         // Now we print a point
         int j = 0;
         fprintf(fp, "%10i ", i*time_step_);
         for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
            double fracprot = (double)nprot[j] / (double)(i+1);
            if (print_prot) {
               // We want the fraction protonated
               fprintf(fp, "%8.5lf ", fracprot);
           }else if (print_pka) {
               // We want the pKa
               double pKa = pH_ - log10( (1.0 - fracprot) / fracprot );
               fprintf(fp, "%8.4lf ", pKa);
           }else {
               // We want the fraction deprotonated
               fprintf(fp, "%8.5lf ", 1.0-fracprot);
            }
            j++;
         }
         fprintf(fp, "%17.6f\n", (double)totprot / (double)(i+1));
      }
      c++; // heh
   }

   fclose(fp);
   return;
}

/// Print the rolling/running average time series with a given window
void ProtTraj::PrintRunningAvg(const int window, const int interval,
                               std::string const& fname, const bool print_prot,
                               const bool print_pka) {
   const int halfwin = window / (2 * time_step_);

   // Open the file for writing and print a header
   FILE *fp = fopen(fname.c_str(), "w");
   fprintf(fp, "#Time step ");
   for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
      char buf[LINEBUF];
      sprintf(buf, "%s %d", rit->getResname().c_str(), rit->getResnum());
      fprintf(fp, "%8s ", buf);
   }
   fprintf(fp, "Total Frac. Prot.\n");

   for (int i = 0; i < nframes_; i+= interval/time_step_) {
      std::vector<long long int> nprot(nres_, 0ll);
      long long int totprot = 0ll;
      // Loop over all frames we should include here
      for (int j = MAX(0, i-halfwin); j < MIN(nframes_, i+halfwin); j++) {
         // Loop over every residue
         int k = 0;
         for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
            long long int protadd = (long long int) (rit->isProtonated(statelist_[j][k]));
            nprot[k] += protadd;
            totprot += (long long int) rit->numProtons(statelist_[j][k]);
            k++;
         }
      }
      // Print this frame to the output file
      int j = 0;
      int n = MIN(nframes_, i+halfwin) - MAX(0, i-halfwin);
      fprintf(fp, "%10i ", time_step_*i);
      for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
         double fracprot = (double)nprot[j] / (double)n;
         if (print_prot) {
            // We want the fraction protonated
            fprintf(fp, "%8.5lf ", fracprot);
        }else if (print_pka) {
            // We want the pKa
            double pKa = pH_ - log10( (1.0 - fracprot) / fracprot );
            fprintf(fp, "%8.4lf ", pKa);
        }else {
            // We want the fraction deprotonated
            fprintf(fp, "%8.5lf ", 1.0-fracprot);
         }
         j++;
     }
     fprintf(fp, "%17.6f\n", (double)totprot / (double)n);
   }

   fclose(fp);
   return;
}

/** Print the population fraction of the entire simulation for each state and
  * each residue
  */
void ProtTraj::PrintProtPop(std::string const& fname) {
   // Set up the table of populations, initializing everything to zero
   std::vector<StateCount> pop_counts;
   // Now loop through every state and every residue, building pop_counts
   for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++) {
      StateCount newstate;
      newstate.nstates = 0;
      for (int i = 0; i < rit->numStates(); i++) {
         newstate.state_cnt.push_back(0ll);
         newstate.prot_cnt.push_back(rit->numProtons(i));
         newstate.nstates++;
      }
      pop_counts.push_back(newstate);
   }

   // Now loop through the entire trajectory, building the population list
   for (int i = 0; i < nframes_; i++) {
      ProtVector curst = statelist_[i];
      for (int j = 0; j < nres_; j++)
         pop_counts[j].state_cnt[ statelist_[i][j] ] += 1ll;
   }

   // Now print out the population counts
   FILE *fp = fopen(fname.c_str(), "w");
   if (fp == NULL) {
      fprintf(stderr, "Error: Could not open [ %s ] for writing!\n", fname.c_str());
      return;
   }

   // Get the max number of states
   int maxst = cpin_->getResidues()[0].numStates();
   for (Cpin::ResIterator rit = cpin_->begin(); rit != cpin_->end(); rit++)
      maxst = MAX(maxst, rit->numStates());

   // Write the header
   fprintf(fp, "%17s ", "Residue Number");
   for (int i = 0; i < maxst; i++)
      fprintf(fp, "%9s%3d ", "State", i);
   fprintf(fp, "\n------------------");
   for (int i = 0; i < maxst; i++)
      fprintf(fp, "-------------");
   fprintf(fp, "\n");

   // Now loop through every residue and print out the populations
   for (int i = 0; i < nres_; i++) {
      char hdr[20];
      sprintf(hdr, "Residue: %s %d", cpin_->getResidues()[i].getResname().c_str(),
                             cpin_->getResidues()[i].getResnum());
      fprintf(fp, "%-17s ", hdr);
      for (int j = 0; j < pop_counts[i].nstates; j++) {
         double pop = (double) pop_counts[i].state_cnt[j] / (double) nframes_;
         fprintf(fp, "%8.6lf (%1i) ", pop, pop_counts[i].prot_cnt[j]);
      }
      fprintf(fp, "\n");
   }

   fclose(fp);
}

/// Prints out the conditional probabilities
void ProtTraj::PrintCondProb(std::string const& fname,
                             std::vector<ConditionalProb> const& condprobs) {
   // Determine how large the field has to be to print out all of the
   // conditional probabilities
   size_t max_size = condprobs[0].str().size() + (size_t) 1;
   for (std::vector<ConditionalProb>::const_iterator it = condprobs.begin();
        it != condprobs.end(); it++) {
      max_size = MAX(max_size, it->str().size() + (size_t) 1);
   }
   max_size = MAX(max_size, 25);

   char fmt[10];
   sprintf(fmt, "%%%ds", (int) max_size);
   // Open the file, then loop through all conditional probabilities, calculate
   // the conditional probability, then print it
   FILE *fp = fopen(fname.c_str(), "w");
   fprintf(fp, fmt, "Conditional Probabilities");
   fprintf(fp, " %10s\n", "Fraction");
   for (std::vector<ConditionalProb>::const_iterator it = condprobs.begin();
        it != condprobs.end(); it++) {
      // Now loop through the whole trajectory
      long long int counted = 0;
      for (int i = 0; i < nframes_; i++)
         counted += (long long int) it->SatisfiedBy(statelist_[i]);

      fprintf(fp, fmt, it->str().c_str());
      double frac = (double) counted / (double) nframes_;
      fprintf(fp, " %10.6f\n", frac);
   }
   fclose(fp);
}
