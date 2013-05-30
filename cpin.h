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
                        char[TITR_RES_C+1][40], char[FN_LEN], int*);
}

/// A titratable residue
class TitratableResidue {
   public:
      // Constructors (only the implemented/used ones are uncommented)
//    TitratableResidue(const char*);
//    TitratableResidue(std::string const&);
      TitratableResidue(std::string const&, std::vector<int> const&);
//    TitratableResidue(const char*, int*);

      // A way to add a list of states
      void defineStates(std::vector<int>);
      void defineStates(int*);

      // In-line Setters
      void setResname(const char* resname) { resname_ = std::string(resname); }
      void setResname(std::string const& resname) { resname_ = resname; }
      void setResnum(int resnum) { resid_ = resnum; }
      
      // In-line Getters
      std::string getResname() { return resname_; }
      int getResnum() { return resid_; }

      // Determine how many states are present
      int numStates() { return protonated_.size(); }

      // Determine if a particular state is protonated or not
      bool isProtonated(int state) { return protonated_[state]; }

   private:
      /// How many protons are in each state?
      std::vector<int> protcnts_;

      /// Is each state protonated?
      std::vector<bool> protonated_;

      /// Name of the titratable residue
      std::string resname_;

      /// Number of the residue
      int resid_;
};

/// Wrapper for calling the Fortran cpin
class Cpin {
   public:
      // Constructors
      Cpin(const char*);
      Cpin(std::string const&);

      // Get the data
      std::vector<TitratableResidue> getResidues();

      int getTrescnt() { return trescnt_; }

      bool isRead() { return is_valid_; }

   private:
      /// Is our cpin file valid?
      bool is_valid_;

      /// CpHMD state info array (analogous to the one used in sander)
      StateInfo stateinf_[TITR_RES_C];

      /// Number of titratable residues defined in the cpin
      int trescnt_;

      /// Number of 'active' protons in each state
      int protcnt_[TITR_STATES_C];

      /// Name of each titratable residue
      std::vector<std::string> resname_;

      /// List of titratable residues
      std::vector<TitratableResidue> residues_;

};
#endif /* CPIN_H */
