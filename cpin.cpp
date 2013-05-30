// cpin.cpp

#include "cpin.h"

Cpin::Cpin(const char* cpinname) :
is_valid(false)
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
      is_valid = true;
}

Cpin::Cpin(std::string const& cpinname) :
is_valid(false)
{
   char my_fname[FN_LEN];
   char resname[TITR_RES_C+1][40];
   strncpy(my_fname, cpinname.c_str(), (size_t)FN_LEN);
   int ierr;
   parse_cpin_(&trescnt_, protcnt_, stateinf_, resname, my_fname, &ierr);

   for (int i = 0; i <= trescnt_; i++) {
      char r[41];
      strncpy(r, resname[i], 41);
      r[40] = '\0';
      resname_.push_back(std::string(r));
   }

   if (ierr != 0)
      fprintf(stderr, "Error: Could not open %s for reading!\n", my_fname);
   else
      is_valid = true;
}
