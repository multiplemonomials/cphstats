#include <cstdio>
#include "cpin.h"
#include "cpout.h"
#include "cloptions.h"
#include "test.h"

using namespace std;

int main(int argc, char**argv) {


   // Set up the command-line options and parse them
   CLOptions clopt = CLOptions(argc, argv);
   if (clopt.Parse())
      return 1;

   test_clopt(clopt);

   Cpin my_cpin = Cpin(clopt.Cpin());
   
   std::vector<TitratableResidue> residues = my_cpin.getResidues();

   printf("There are %d titratable residues!\n", (int)residues.size());
   printf("They are:\n");
   for (int i = 0; i < my_cpin.getTrescnt(); i++) {
      printf("\t%s %d (%d states) [ ", residues[i].getResname().c_str(),
             residues[i].getResnum(), residues[i].numStates());
      for (int j = 0; j < residues[i].numStates(); j++) {
         if (residues[i].isProtonated(j))
            printf("P ");
         else
            printf("D ");
      }
      printf("]\n");
   }

   vector<CpoutFile> cpouts;
   for (CLOptions::cpout_iterator it = clopt.begin(); it != clopt.end(); it++) {
      fprintf(stdout, "Found cpout file %s\n", it->c_str());
      cpouts.push_back(CpoutFile(*it));
      printf("Done loading %s\n", it->c_str());
   }

   printf("Found %d cpouts.\n", (int)clopt.Cpouts().size());

   printf("All done!\n");

   return 0;
}
