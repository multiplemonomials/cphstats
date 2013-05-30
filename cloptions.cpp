// cloptions.cpp: Manage the command-line options and parsing


#include <cstring>
#include <cstdio>
#include "cloptions.h"

CLOptions::CLOptions(int argc, char**argv) {
   
   int i = 1;
   std::vector<bool> marked(argc, false);

   prog_ = std::string(argv[0]);
   prog_ = prog_.substr(prog_.find_last_of('/')+1);
   parse_return_ = OK;

   while (i < argc) {
      if (strncmp("-h", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         parse_return_ = HELP;
         break;
     }else if (strncmp("--help", argv[i], 6) == 0 && strlen(argv[i]) == 6) {
         parse_return_ = HELP;
         break;
     }else if (strncmp("-v", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         parse_return_ = VERSION;
         break;
     }else if (strncmp("--version", argv[i], 9) == 0 && strlen(argv[i]) <= 9) {
         parse_return_ = VERSION;
         break;
     }else if (strncmp("-i", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i++] = true;
         marked[i] = true;
         cpin_ = std::string(argv[i]);
     }else if (strncmp("--cpin", argv[i], 6) == 0 && strlen(argv[i]) == 6) {
         marked[i++] = true;
         marked[i] = true;
         cpin_ = std::string(argv[i]);
     }else if (strncmp("-", argv[i], 1) == 0) {
         parse_return_ = ERR;
      }
      i++;
   }

   if (parse_return_ == OK) {
      // Now fill the list of cpout files
      for (int j = 1; j < argc; j++) {
         if (!marked[argc]) {
            marked[argc] = true;
            cpouts_.push_back(std::string(argv[j]));
         }
      }
   }

   if (cpouts_.size() == 0 && parse_return_ == OK) parse_return_ = HELP;
}

void CLOptions::Help() {

   printf("Usage: %s [Options] cpout1 [cpout2 [cpout3 ...] ]\n", prog_.c_str());
   printf("\n");
   printf("General Options:\n");
   printf("    -h, --help     Print this help and exit.\n");
   printf("    -v, --version  Print the version number and exit.\n");
   printf("\n");
   printf("Input Files:\n");
   printf("    -i FILE, --cpin FILE\n");
   printf("                   Input cpin file (from sander) with titrating residue\n");
   printf("                   information.\n");
   printf("\n");
   
   return;
}

void CLOptions::Version() {
   printf("%s: Version %s\n", prog_.c_str(), VERSION_STR);

   return;
}

int CLOptions::Parse() {
   if (parse_return_ == HELP)
      Help();
   else if (parse_return_ == VERSION)
      Version();

   return parse_return_;
}
