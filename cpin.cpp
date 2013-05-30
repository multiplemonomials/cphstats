/** cpin.cpp: Handles the code for parsing cpin files and defining titratable
  * residues.
  */

#include <cstdio>
#include <cstdlib>
#include "cpin.h"
#include "string_manip.h"

/// Cpin constructor from a char array
Cpin::Cpin(const char* cpinname) :
is_valid_(false)
{

   char my_fname[FN_LEN];
   strncpy(my_fname, cpinname, (size_t)FN_LEN);
   char resname[TITR_RES_C+1][40];
   int ierr;
   parse_cpin_(&trescnt_, protcnt_, stateinf_, resname, my_fname, &ierr);

   for (int i = 0; i <= trescnt_; i++) {
      char r[41];
      strncpy(r, resname[i], 41);
      r[40] = '\0';
      resname_.push_back(std::string(r));
   }

   if (ierr != 0)
      fprintf(stderr, "Error: Could not open %s for reading!\n", cpinname);
   else
      is_valid_ = true;
}

/// Cpin constructor from a string
Cpin::Cpin(std::string const& cpinname) :
is_valid_(false)
{
   char my_fname[FN_LEN];
   char resname[TITR_RES_C+1][40];
   strncpy(my_fname, cpinname.c_str(), (size_t)FN_LEN);
   int ierr;
   parse_cpin_(&trescnt_, protcnt_, stateinf_, resname, my_fname, &ierr);

   for (int i = 0; i <= trescnt_; i++) {
      char r[41];
      strncpy(r, resname[i], 40);
      int last_item = 0;
      for (int j = 0; j < 40; j++)
         if (r[j] != ' ')
            last_item = j+1;
      r[last_item] = '\0';
      resname_.push_back(std::string(r));
   }

   if (ierr != 0)
      fprintf(stderr, "Error: Could not open %s for reading!\n", my_fname);
   else
      is_valid_ = true;
}

/// Returns a list of titratable residues
std::vector<TitratableResidue> Cpin::getResidues() {
   
   if (!is_valid_ || !residues_.empty()) return residues_;

   // Now define all of the residues
   for (int i = 0; i < trescnt_; i++) {
      int nstates = stateinf_[i].num_states;
      int firststate = stateinf_[i].first_state;
      std::vector<int> res_protcnt;
      for (int j = 0; j < nstates; j++)
         res_protcnt.push_back( protcnt_[firststate+j] );
      residues_.push_back( TitratableResidue(resname_[i+1], res_protcnt) );
   }

   return residues_;
}

/// Constructor for TitratableResidue
TitratableResidue::TitratableResidue(std::string const& resname,
                                     std::vector<int> const& protcnts) :
resid_(0)
{
   protcnts_ = protcnts;

   // Process the resname
   std::vector<std::string> words = split(resname);
   resname_ = words[1]; resid_ = atoi(words[2].c_str());

   // Now determine which states are "protonated" and which are "deprotonated"
   int max_prots = protcnts_[0];

   for (uint i = 1; i < protcnts_.size(); i++)
      max_prots = protcnts_[i] > max_prots ? protcnts_[i] : max_prots;

   for (uint i = 0; i < protcnts_.size(); i++)
      protonated_.push_back(protcnts_[i] == max_prots);
}
