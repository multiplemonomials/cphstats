// utilities.cpp: Miscellaneous, useful utilities

#include <cstring>
#include <fstream>
#include "utilities.h"

/// Test if a file exists
bool fexists(std::string const& fname) {
   std::ifstream f(fname.c_str());
   return f;
}

int sort_remd_files(CpoutList cpouts, std::string const& prefix, 
                    const bool overwrite) {
   RemdMap filemap;
   // Make sure none of the files we're about to write already exist, then
   // create a new file object and add it to the map, paired with the pH
   std::vector<std::string> suffixes;
   for (cpout_iterator it = cpouts.begin(); it != cpouts.end(); it++) {
      char buf[128];
      sprintf(buf, ".pH_%.2f", it->pH());
      std::string sfx = std::string(buf);
      for (std::vector<std::string>::const_iterator itt = suffixes.begin();
               itt != suffixes.end(); itt++) {
         if (*itt == sfx) {
            fprintf(stderr, "Error: Same pH (%.2f) found twice!\n", it->pH());
            return 1;
         }
      }
      if (!overwrite && fexists(prefix + sfx)) {
         fprintf(stderr, "Error: %s exists! Not overwriting.\n", (prefix+sfx).c_str());
         return 1;
      }
      suffixes.push_back( sfx );
      FILE *fp = NULL;
      filemap[it->pH()] = fp;
   }

   // Now go through and open every file
   for (RemdMap::iterator it = filemap.begin(); it != filemap.end(); it++) {
      char buf[128];
      sprintf(buf, ".pH_%.2f", it->first);
      std::string fname = prefix + std::string(buf);
      it->second = fopen(fname.c_str(), "w");
   }
   // Now go through every frame of every cpout and sort the records
   bool done = false;
   for (cpout_iterator it = cpouts.begin(); it != cpouts.end(); it++)
      done = done || it->Done();
   while (!done) {
      for (cpout_iterator it = cpouts.begin(); it != cpouts.end(); it++) {
         Record rec = it->GetRecord();
         if (rec.full) {
            fprintf(filemap[rec.pH], "Solvent pH: %8.5f\n", rec.pH);
            fprintf(filemap[rec.pH], "Monte Carlo step size: %8d\n", it->StepSize());
            fprintf(filemap[rec.pH], "Time step: %8d\n", rec.time_step);
            fprintf(filemap[rec.pH], "Time: %10.3f\n", rec.time);
         }
         for (std::vector<RecordPoint>::const_iterator pit = rec.points.begin();
                     pit != rec.points.end(); pit++)
            fprintf(filemap[rec.pH], "Residue %4d State: %2d\n",
                    pit->residue, pit->state);
         if (!rec.points.empty())
            fprintf(filemap[rec.pH], "\n"); // terminate this record
      }
      // Refresh whether or not we're done
      done = false;
      for (cpout_iterator it = cpouts.begin(); it != cpouts.end(); it++)
         done = done || it->Done();
   }
   
   // Now it's time to close all of the files
   for (RemdMap::const_iterator it = filemap.begin(); it != filemap.end(); it++)
      fclose(it->second);

   return 0;
}
