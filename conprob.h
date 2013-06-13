#ifndef CONPROB_H
#define CONPROB_H

#include <string>

#include "cpin.h"
#include "types.h"

// This header contains classes that manage conditional probability requests

class ConditionalProb {
   public:
      enum RetType {OK=0, ERR, UNASSIGNED};
      // Constructors
      ConditionalProb();
      ConditionalProb(std::string const&, Cpin const&);
      ConditionalProb(const char*, Cpin const&);
   
      static int DEBUG;

      // Sets up the conditional probability
      RetType Set(std::string const&, Cpin const&);
      RetType Set(const char*, Cpin const&);

      // Determines if this conditional probability is satisfied by a
      // protonation state vector
      bool SatisfiedBy(ProtVector);

   private:

      // A vector of states with either True (satisfied) or False (unsatisfied)
      typedef std::vector<bool> ActiveState;

      std::vector<ActiveState> active_states_;

      RetType valid_;
};

#endif /* CONPROB_H */
