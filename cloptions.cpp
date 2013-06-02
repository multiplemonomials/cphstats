// cloptions.cpp: Manage the command-line options and parsing


#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "cloptions.h"
#include "utilities.h"

using namespace std;

CLOptions::CLOptions(int argc, char**argv) :
verbose_(0),
do_calcpka_(true),
cumulative_(false),
runavgwin_(0),
chunksize_(0),
overwrite_(false),
interval_(1),
protonated_(true),
time_step_(0.002f),
pKa_(false)
{
   
   // Initialize some strings
   runavgout_ = std::string("running_avgs.dat");
   cumout_ = std::string("cumulative.dat");
   chunkout_ = std::string("chunk.dat");

   // Now parse everything
   int i = 1;
   std::vector<bool> marked(argc, false);

   prog_ = std::string(argv[0]);
   prog_ = prog_.substr(prog_.find_last_of('/')+1);
   parse_return_ = OK;
   marked[0] = true; // this is the program name

   while (i < argc) {
      if (strncmp("-h", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         parse_return_ = HELP;
         break;
     }else if (strncmp("-O", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i] = true;
         overwrite_ = true;
     }else if (strncmp("--overwrite", argv[i], 11) == 0 && strlen(argv[i]) == 11) {
         marked[i] = true;
         overwrite_ = true;
     }else if (strncmp("--help", argv[i], 6) == 0 && strlen(argv[i]) == 6) {
         parse_return_ = HELP;
         break;
     }else if (strncmp("-V", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         parse_return_ = VERSION;
         break;
     }else if (strncmp("--version", argv[i], 9) == 0 && strlen(argv[i]) <= 9) {
         parse_return_ = VERSION;
         break;
     }else if (strncmp("-i", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         cpin_ = std::string(argv[i]);
     }else if (strncmp("--cpin", argv[i], 6) == 0 && strlen(argv[i]) == 6) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         cpin_ = std::string(argv[i]);
     }else if (strncmp("--calcpka-output", argv[i], 16) == 0 && 
               strlen(argv[i]) == 16) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         output_ = std::string(argv[i]);
     }else if (strncmp("-o", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         output_ = std::string(argv[i]);
     }else if (strncmp("-v", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         verbose_ = atoi(argv[i]);
     }else if (strncmp("--verbose", argv[i], 9) == 0 && strlen(argv[i]) == 9) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         verbose_ = atoi(argv[i]);
     }else if (strncmp("-R", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         runavgout_ = std::string(argv[i]);
     }else if (strncmp("--running-avg-out", argv[i], 17) == 0 && 
               strlen(argv[i]) == 17) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         runavgout_ = std::string(argv[i]);
     }else if (strncmp("--chunk-out", argv[i], 11) == 0 && strlen(argv[i]) == 11) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         chunkout_ = std::string(argv[i]);
     }else if (strncmp("--cumulative-out", argv[i], 16) == 0 && 
               strlen(argv[i]) == 16) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         chunkout_ = std::string(argv[i]);
     }else if (strncmp("--calcpka", argv[i], 9) == 0 && strlen(argv[i]) == 9) {
         marked[i] = true;
         do_calcpka_ = true;
     }else if (strncmp("--no-calcpka", argv[i], 12) == 0 && strlen(argv[i]) == 12) {
         marked[i] = true;
         do_calcpka_ = false;
     }else if (strncmp("-r", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         runavgwin_ = atoi(argv[i]);
         if (runavgwin_ == 0) {
            fprintf(stderr, "Error: Bad argument for running average window!\n");
            parse_return_ = ERR;
            break;
         }
     }else if (strncmp("-t", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         time_step_ = atof(argv[i]);
     }else if (strncmp("--time-step", argv[i], 11) == 0 && strlen(argv[i]) == 11) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         time_step_ = atof(argv[i]);
     }else if (strncmp("--running-avg", argv[i], 13) == 0 && strlen(argv[i]) == 13) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         runavgwin_ = atoi(argv[i]);
         if (runavgwin_ == 0) {
            fprintf(stderr, "Error: Bad argument for running average window!\n");
            parse_return_ = ERR;
            break;
         }
     }else if (strncmp("--chunk", argv[i], 7) == 0 && strlen(argv[i]) == 7) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         chunksize_ = atoi(argv[i]);
         if (chunksize_ == 0) {
            fprintf(stderr, "Error: Bad argument for running average window!\n");
            parse_return_ = ERR;
            break;
         }
     }else if (strncmp("--cumulative", argv[i], 12) == 0 && strlen(argv[i]) == 12) {
         marked[i] = true;
         cumulative_ = true;
     }else if (strncmp("--fix-remd", argv[i], 10) == 0 && strlen(argv[i]) == 10) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         reorder_prefix_ = std::string(argv[i]);
     }else if (strncmp("-n", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         interval_ = atoi(argv[i]);
     }else if (strncmp("--interval", argv[i], 10) == 0 && strlen(argv[i]) == 10) {
         marked[i++] = true;
         if (i == argc) {parse_return_ = INSUFARGS; break;}
         marked[i] = true;
         interval_ = atoi(argv[i]);
     }else if (strncmp("-p", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i] = true;
         protonated_ = true;
     }else if (strncmp("--protonated", argv[i], 12) == 0 && strlen(argv[i]) == 12) {
         marked[i] = true;
         protonated_ = true;
     }else if (strncmp("-d", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i] = true;
         protonated_ = false;
     }else if (strncmp("--deprotonated", argv[i], 14) == 0 && strlen(argv[i]) == 14) {
         marked[i] = true;
         protonated_ = false;
     }else if (strncmp("-a", argv[i], 2) == 0 && strlen(argv[i]) == 2) {
         marked[i] = true;
         pKa_ = true;
     }else if (strncmp("--pKa", argv[i], 5) == 0 && strlen(argv[i]) == 5) {
         marked[i] = true;
         pKa_ = true;
     }else if (strncmp("-", argv[i], 1) == 0) {
         fprintf(stderr, "Unrecognized command-line option: %s\n", argv[i]);
         parse_return_ = ERR;
         break;
      }
      i++;
   }

   if (parse_return_ == OK) {
      // Now fill the list of cpout files
      for (int j = 1; j < argc; j++) {
         if (!marked[j]) {
            marked[j] = true;
            cpouts_.push_back(std::string(argv[j]));
         }
      }
   }

   if (cpouts_.size() == 0 && parse_return_ == OK) parse_return_ = HELP;
}

void CLOptions::Help() {

   Usage();
   printf("\n");
   printf("General Options:\n");
   printf("    -h, --help     Print this help and exit.\n");
   printf("    -V, --version  Print the version number and exit.\n");
   printf("    -O, --overwrite\n");
   printf("                   Allow existing outputs to be overwritten.\n");
   printf("\n");
   printf("Input Files and Options:\n");
   printf("    -i FILE, --cpin FILE\n");
   printf("\n");
   printf("                   Input cpin file (from sander) with titrating residue\n");
   printf("                   information.\n");
   printf("    -t FLOAT, --time-step FLOAT\n");
   printf("                   This is the time step in ps you used in your simulations.\n");
   printf("                   It will be used to print data as a function of time.\n");
   printf("                   Default is 2 fs (0.002)\n");
   printf("\n");
   printf("Output Files:\n");
   printf("    -o FILE, --calcpka-output FILE\n");
   printf("                   File to which the standard `calcpka'-type statistics\n");
   printf("                   are written. Default is stdout\n");
   printf("    -R FILE, --running-avg-out FILE\n");
   printf("                   Output file where the running averages for each\n");
   printf("                   residue is printed. Default is [running_avgs.dat]\n");
   printf("    --chunk-out FILE\n");
   printf("                   Output file where the protonated fraction calculated\n");
   printf("                   over chunks of the simulation are printed.\n");
   printf("                   Default is [chunk.dat]\n");
   printf("    --cumulative-out FILE\n");
   printf("                   Output file where the cumulative protonated fraction\n");
   printf("                   is printed. Default is [cumulative.dat]\n");
   printf("\n");
   printf("Output Options:\n");
   printf("  These options modify how the output files will appear\n");
   printf("\n");
   printf("    -v INT, --verbose INT\n");
   printf("                   Controls how much information is printed to the\n");
   printf("                   calcpka-style output file. Options are:\n");
   printf("                      (0) Just print fraction protonated. [Default]\n");
   printf("                      (1) Print everything calcpka prints.\n");
   printf("    -n INT, --interval INT\n");
   printf("                   An interval between which to print out time series data\n");
   printf("                   like `chunks', `cumulative' data, and running averages.\n");
   printf("    -p, --protonated\n");
   printf("                   Print out protonation fraction instead of deprotonation\n");
   printf("                   fraction in time series data (Default behavior).\n");
   printf("    -d, --deprotonated\n");
   printf("                   Print out deprotonation fraction instead of protonation\n");
   printf("                   fraction in time series data.\n");
   printf("    -a, --pKa      Print predicted pKas (via Henderson-Hasselbalch) in place\n");
   printf("                   of fraction (de)protonated. This option overrides all\n");
   printf("                   instances of -p/-d described above. NOT default behavior.\n");
   printf("\n");
   printf("Analysis Options:\n");
   printf("  These options control which analyses are done. By default, only\n");
   printf("  the original, calcpka-style analysis is done.\n");
   printf("\n");
   printf("    --calcpka      Triggers the calcpka-style output [On by default]\n");
   printf("    --no-calcpka   Turns off the calcpka-style output\n");
   printf("    -r WINDOW, --running-avg WINDOW\n");
   printf("                   Defines a window size for a moving, running average\n");
   printf("                   fraction protonated. <WINDOW> is the number of MD\n");
   printf("                   steps (NOT the number of MC exchange attempts).\n");
   printf("    --chunk WINDOW\n");
   printf("                   Computes the pKa's over a chunk of the simulation of\n");
   printf("                   size <WINDOW> time steps.\n");
   printf("    --cumulative   Computes the cumulative fraction protonated over the\n");
   printf("                   course of the trajectory.\n");
   printf("    --fix-remd PREFIX\n");
   printf("                   This option will trigger %s to reassemble the\n", prog_.c_str());
   printf("                   titration data into pH-specific ensembles. This\n");
   printf("                   is an exclusive mode of the program---no other\n");
   printf("                   analyses will be done.\n");
   printf("\n");
   printf("This program analyzes constant pH output files (cpout) from Amber.\n");
   printf("These output files can be compressed using gzip compression. The\n");
   printf("compression will be detected automatically by the file name extension.\n");
   printf("You must have the gzip headers for this functionality to work.\n");
   
   return;
}

void CLOptions::Usage() {
   printf("Usage: %s [-O] [-V] [-h] [-i <cpin>] [-t] [-o FILE] [-R FILE -r INT]\n", prog_.c_str());
   printf("             [--chunk INT --chunk-out FILE] [--cumulative --cumulative-out FILE]\n");
   printf("             [-v INT] [-n INT] [-p|-d] [--calcpka|--no-calcpka] [--fix-remd]\n");
   printf("             cpout1 [cpout2 [cpout3 ...] ]\n");

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
   else if (parse_return_ == INSUFARGS) {
      fprintf(stderr, "Insufficient arguments!\n");
      Usage();
  }else if (parse_return_ == ERR) {
      Usage();
   }

   return parse_return_;
}

int CLOptions::CheckInput() {
   int inerr = 0;
   if (runavgwin_ < 0) {
      fprintf(stderr, "Error: -r/--running-avg window must be non-negative!\n");
      inerr = 1;
   }

   if (chunksize_ < 0) {
      fprintf(stderr, "Error: --chunk window must be non-negative!\n");
      inerr = 1;
   }

   if (time_step_ <= 0) {
      fprintf(stderr, "Error: --time-step must be a positive number!\n");
      inerr = 1;
   }

   if (interval_ <= 0) {
      fprintf(stderr, "Error: -n/--interval must be a positive integer!\n");
      inerr = 1;
   }

   // Make sure no files exist that we want to overwrite
   if (runavgwin_ > 0 && !overwrite_)
      if (fexists(runavgout_)) {
         fprintf(stderr, "Error: %s exists; not overwriting.\n", runavgout_.c_str());
         inerr = 1;
      }

   if (do_calcpka_ && !overwrite_)
      if (!output_.empty() && fexists(output_)) {
         fprintf(stderr, "Error: %s exists; not overwriting.\n", output_.c_str());
         inerr = 1;
      }

   if (chunksize_ > 0 && !overwrite_)
      if (fexists(chunkout_)) {
         fprintf(stderr, "Error: %s exists; not overwriting.\n", chunkout_.c_str());
         inerr = 1;
      }

   if (cumulative_ && !overwrite_)
      if (fexists(cumout_)) {
         fprintf(stderr, "Error: %s exists; not overwriting.\n", cumout_.c_str());
         inerr = 1;
      }

   // Make sure we provided a prefix and didn't accidentally just dump a bunch
   // of cpout files on the command line without a prefix
   if (!reorder_prefix_.empty() && fexists(reorder_prefix_)) {
      fprintf(stderr, "--------| Interpreting %s as the new REMD file prefix. If this is\n", 
              reorder_prefix_.c_str());
      fprintf(stderr, "Warning | a cpout file, kill this program and re-run with a correct\n");
      fprintf(stderr, "--------| REMD file prefix.\n");
   }

   return inerr;
}
