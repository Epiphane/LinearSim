#include <stdio.h>
#include <stdlib.h>
#include "Report.h"

#define PARAM_LENGTH 10
#define MAX_PARAMS 8

void makeIntParameter(char *param, char letter, int value);
void makeDoubleParameter(char *param, char letter, double value);

int main() {
   double endValues[2];
   int finalTime, numCells, cellID;
   int driverPipe[2], tempPipe[2];
   char *params[MAX_PARAMS], *strCursor;

   scanf("%lf %lf %d %d", endvalues, endValues + 1, &finalTime, &numCells);
   
   *params = malloc(5);
   strcpy(*params, "Cell");

   for(cellID = 0; cellID < numCells; cellID++) {
      makeIntParameter(params[1], 'S', cellID);
      while(temp) {
         
      }

      if(!cellID) {
         
      }
   }

   return 0;
}

void makeIntParameter(char *param, char letter, int value) {
   int ndx;
   
   param = malloc(PARAM_LENGTH);
   
}

void makeVParameter(char *param, double value) {
   param = malloc(PARAM_LENGTH);

}
