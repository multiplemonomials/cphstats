#include <cstdio>
#include "cpin.h"

using namespace std;

int main(int argc, char**argv) {

   if (argc < 2) {
      printf("Usage: cphutil <cpin>\n");
      return 0;
   }

   Cpin my_cpin = Cpin(argv[1]);

   printf("There are %d titratable residues!\n", my_cpin.getTrescnt());
   printf("They are:\n");
   vector<string> rnames = my_cpin.getResnames();
   for (int i = 0; i < my_cpin.getTrescnt(); i++)
      printf("\t%s\n", rnames[i].c_str());
   printf("All done!\n");

   return 0;
}
