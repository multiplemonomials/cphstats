#ifndef CPIN_H
#define CPIN_H

#include <cstring>
#include <string>
#include <vector>
#include "constants.h"

// External Fortran functions for parsing the cpin

typedef struct {
   int num_states, first_atom,  num_atoms,  first_state, first_charge;
} StateInfo;

extern "C" {
   void parse_cpin_(int*, int[TITR_STATES_C], StateInfo[TITR_RES_C],
                        char[40][TITR_RES_C+1], char[FN_LEN], int*);
}

// Wrapper for calling the Fortran cpin

class Cpin {
   public:
   // Constructors
   Cpin(const char*);
   Cpin(std::string const&);

   // Get the data
   std::vector<std::string> getResnames() {return resname_;}

   int getTrescnt() { return trescnt_; }

   bool isOpen() { return is_valid; }

   private:
   // Data
   bool is_valid;
   // CpHMD state info struct and array
   StateInfo stateinf_[TITR_RES_C];
   // Number of titrating residues
   int trescnt_;
   // Array of protonation states
   int protcnt_[TITR_STATES_C];
   // Residue names
   //char resname_[TITR_RES_C+1][40];
   std::vector<std::string> resname_;

};
#endif /* CPIN_H */
