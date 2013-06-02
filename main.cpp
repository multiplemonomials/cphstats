#include <cstdio>
#include "cpin.h"
#include "cpout.h"
#include "cloptions.h"
#include "prottraj.h"
#include "test.h"
#include "types.h"
#include "utilities.h"

using namespace std;

int main(int argc, char**argv) {

   // Set up the command-line options and parse them
   CLOptions clopt = CLOptions(argc, argv);
   if (clopt.Parse())
      return 1;

   // Check the input
   if (clopt.CheckInput()) {
      fprintf(stderr, "Error: Input errors detected! See messages above.\n");
      return 1;
   }

// test_clopt(clopt);

   int nres = 0; // number of residues (for consistency tests)

   /* Set up the cpin and print some diagnostic information (only necessary if
    * we're not fixing REMD files
    */
   Cpin my_cpin;
   if (clopt.REMDPrefix().empty()) {
      if ( my_cpin.Parse(clopt.Cpin()) )
         return 1;
   
      nres = (int) my_cpin.getTrescnt();

      if (nres <= 0) {
         fprintf(stderr, "Error: Did not detect any residues in %s!\n",
                 clopt.Cpin().c_str());
         return 1;
      }
      printf("There are %d titratable residues defined in %s:\n",
             nres, my_cpin.getFilename().c_str());
      printf("They are:\n");
      for (Cpin::ResIterator it = my_cpin.begin(); it != my_cpin.end(); it++) {
         printf("\t%3s %-3d (%d states) [ ", it->getResname().c_str(),
                                          it->getResnum(), it->numStates());
         for (int j = 0; j < it->numStates(); j++) {
            if (it->isProtonated(j))
               printf("P ");
            else
               printf("D ");
         }
         printf("]\n");
         }
   } // if clopt.REMDPrefix().empty()

   // Set up the cpouts
   CpoutList cpouts;
   for (CLOptions::cpout_iterator it = clopt.begin(); it != clopt.end(); it++) {
      CpoutFile c = CpoutFile(*it);
      // Skip over invalid cpouts
      if (!c.Valid()) {
         fprintf(stderr, "Error: Cpout file %s is invalid! Skipping.\n", it->c_str());
         continue;
      }
      // For REMD fixing where a cpin is unnecessary, make sure all cpouts have
      // the same number of residues, so set nres to the first cpout's Nres
      if (nres <= 0) nres = c.Nres();
      // Skip over cpouts with a residue mismatch
      if (c.Nres() != nres) {
         fprintf(stderr, "Error: Cpout file %s has %d residues. I expected %d.  Skipping.\n",
                 it->c_str(), c.Nres(), my_cpin.getTrescnt());
         continue;
      }
      fprintf(stdout, "Added [[ %s ]] to cpout list.\n", it->c_str());
      cpouts.push_back(c);
   }

   printf("Analyzing %d cpouts.\n", (int)clopt.Cpouts().size());

   // Special-case REMD re-ordering
   if (!clopt.REMDPrefix().empty())
      return sort_remd_files(cpouts, clopt.REMDPrefix(), clopt.Overwrite());

   ProtTraj stats = ProtTraj(&my_cpin, cpouts[0].pH(), cpouts[0].GetRecord());
   for (cpout_iterator it = cpouts.begin(); it != cpouts.end(); it++) {
      stats.LoadCpout(*it);
   }

   // Do the normal calcpka-style output if requested
   if (clopt.Calcpka()) {
      if (clopt.Output().empty())
         stats.PrintCalcpka(stdout);
      else {
         FILE *fd = fopen(clopt.Output().c_str(), "w");
         stats.PrintCalcpka(fd);
      }
   }

   // Do chunk analysis
   if (clopt.ChunkWindow() > 0)
      stats.PrintChunks(clopt.ChunkWindow(), clopt.ChunkOutput(),
                        clopt.PrintProtonated() && !clopt.pKa(), clopt.pKa());
   
   // Do cumulative analysis
   if (clopt.doCumulative())
      stats.PrintCumulative(clopt.CumulativeOutput(), clopt.Interval(),
                        clopt.PrintProtonated() && !clopt.pKa(), clopt.pKa());

   // Do running averages
   if (clopt.RunningAvgWindow() > 0)
      stats.PrintRunningAvg(clopt.RunningAvgWindow(), clopt.Interval(), 
                        clopt.RunningAvgOutput(),
                        clopt.PrintProtonated() && !clopt.pKa(), clopt.pKa());

   printf("All done!\n");

   return 0;
}
