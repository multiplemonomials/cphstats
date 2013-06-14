// conprob.cpp: contains the code to handle conditional probability evaluations
#include <sstream>
#include <cctype>
#include <cstdio>
#include <cstdlib>

#include "conprob.h"
#include "string_manip.h"

ConditionalProb::ConditionalProb() :
valid_(UNASSIGNED) {}

ConditionalProb::ConditionalProb(std::string const& instring) {
   valid_ = UNASSIGNED;
   instring_ = instring;
}

ConditionalProb::ConditionalProb(const char* instring) {
   valid_ = UNASSIGNED;
   instring_ = std::string(instring);
}

ConditionalProb::RetType ConditionalProb::Set(Cpin const& cpin) {

   if (instring_.empty()) {
      fprintf(stderr, "Error: Conditional probability string is empty!\n");
      return ERR;
   }

   // Build the active state list
   for (Cpin::ResIterator rit = cpin.begin(); rit != cpin.end(); rit++)
      active_states_.push_back( ActiveState(rit->numStates(), false) );

   // For comparisons
   std::string DEPROT = std::string("DEPROTONATED");
   std::string PROT = std::string("PROTONATED");

   // We have to parse something like; <number>:<state>,<number>:<state>,...
   std::vector<std::string> criteria = split(instring_.c_str(), ",");

   for (std::vector<std::string>::const_iterator cit = criteria.begin();
        cit != criteria.end(); cit++) {
      std::vector<std::string> parts = split(*cit, ":");
      if (parts.size() != 2) {
         fprintf(stderr, "Error: Conditional probability string [%s] not recognized!\n",
                 instring_.c_str());
         return ERR;
      }
      int resid = atoi(parts[0].c_str());
      if (resid == 0) {
         fprintf(stderr, "Error: Could not determine residue name from [%s] in [%s]\n",
                 cit->c_str(), instring_.c_str());
         return ERR;
      }
      // Find out which residue index this is
      int residx = -1;
      for (int i = 0; i < cpin.getTrescnt(); i++) {
         if (cpin.getResidues()[i].getResnum() == resid) {
            residx = i;
            break;
         }
      }
      if (residx < 0) {
         fprintf(stderr, "Error: Residue %d does not appear to be titratable!\n", 
                 resid);
         return ERR;
      }
      
      int stateidx;
      std::istringstream iss(parts[1]);
      if (iss >> stateidx) {
         if (stateidx >= cpin.getResidues()[residx].numStates()) {
            fprintf(stderr, "Error: State index %d out of range for residue %d!\n",
                    stateidx, resid);
            return ERR;
         }
         active_states_[residx][stateidx] = true;
//       printf("Residue %d, state %d is true.\n", residx, stateidx); // DEBUG
     }else {
         parts[1] = upper(parts[1]);
         if (PROT.size() >= parts[1].size() && 
             parts[1] == PROT.substr(0, parts[1].size())) {
            // Label all protonated states of this residue
            for (int i = 0; i < cpin.getResidues()[residx].numStates(); i++) {
               active_states_[residx][i] = cpin.getResidues()[residx].isProtonated(i);
//             // DEBUG
//             if (cpin.getResidues()[residx].isProtonated(i))
//                printf("Residue %d, state %d is true.\n", residx, i);
            }

        }else if (DEPROT.size() >= parts[1].size() &&
                  parts[1] == DEPROT.substr(0, parts[1].size())) {
            for (int i = 0; i < cpin.getResidues()[residx].numStates(); i++) {
               active_states_[residx][i] = !cpin.getResidues()[residx].isProtonated(i);
//             // DEBUG
//             if (!cpin.getResidues()[residx].isProtonated(i))
//                printf("Residue %d, state %d is true.\n", residx, i);
            }

        }else {
            fprintf(stderr, "Error: Could not identify conditional criteria [%s]\n",
                    parts[1].c_str());
            return ERR;
         }
      }
   }

   // Now filter out all of the residues that were not specified. If all states
   // in a given residue are 'false', then this residue was not specified in the
   // conditional probability, so turn them all to 'true'

   for (int i = 0; i < cpin.getTrescnt(); i++) {
      bool all_false = true;
      for (int j = 0; j < cpin.getResidues()[i].numStates(); j++) {
         if (active_states_[i][j]) {
            all_false = false;
            break;
         }
      }

      if (all_false)
         for (int j = 0; j < cpin.getResidues()[i].numStates(); j++)
            active_states_[i][j] = true;
   }

// // DEBUG
// for (int i = 0; i < 80; i++) printf("="); printf("\n");

   return OK;
}

bool ConditionalProb::SatisfiedBy(ProtVector invec) const {
   bool satisfied = true;
   for (size_t i = 0; i < invec.size(); i++)
      satisfied = satisfied && active_states_[i][invec[i]];
   return satisfied;
}
