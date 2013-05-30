#ifndef CLOPTIONS_H
#define CLOPTIONS_H

#define VERSION_STR "0.1a"

#include <cstring>
#include <string>
#include <vector>

using namespace std;

class CLOptions {
   public:
      enum RetType {OK=0, HELP, VERSION, ERR};

      CLOptions(int, char**);

      vector<string> Cpouts() { return cpouts_; }

      string Cpin() { return cpin_; }

      int Parse();

      void Help();

      void Version();

   private:
      /// Name of the input cpin file
      string cpin_;

      /// Name of the input cpout files
      vector<string> cpouts_;

      /// Name of the program
      string prog_;

      /// Status of the parsing
      RetType parse_return_;

};

#endif /* CLOPTIONS_H */
