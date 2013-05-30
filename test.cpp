/// test.cpp: Contains functions for testing various parts of the program

#include <cstdio>
#include "constants.h"
#include "test.h"

void test_clopt(CLOptions clopt) {
   
   printf("The verbose level is: %d\n", clopt.Verbose());
   printf("The cpouts are:\n");
   for (uint i = 0; i < clopt.Cpouts().size(); i++)
      printf("\t%s\n", clopt.Cpouts()[i].c_str());
   printf("The cpin is: %s\n", clopt.Cpin().c_str());
   if (clopt.doCumulative())
      printf("Cumulative file name: %s\n", clopt.CumulativeOutput().c_str());
   else
      printf("NOT doing cumulative protonation calcs.\n");

   if (clopt.RunningAvgWindow() > 0)
      printf("Running average: Window %d; File %s\n", clopt.RunningAvgWindow(),
                      clopt.RunningAvgOutput().c_str());
   else
      printf("NOT doing running averages.\n");

   if (clopt.ChunkWindow() > 0)
      printf("Chunk statistics: Window %d; File %s\n", clopt.ChunkWindow(),
                      clopt.ChunkOutput().c_str());
   else
      printf("NOT doing chunk analysis.\n");

   if (clopt.Calcpka())
      if (clopt.Output().empty())
         printf("Output file: stdout\n");
      else
         printf("Output file: %s\n", clopt.Output().c_str());
   else
      printf("NOT doing calcpka analysis.\n");

   if (clopt.CheckInput())
      printf("Errors in input detected!\n");

   return;
}
