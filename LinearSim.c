#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Report.h"

#define PARAM_LENGTH 10
#define MAX_PARAMS 8

void makeIntParameter(char *param, char letter, int value);
void makeDoubleParameter(char *param, double value);

int main() {
   double endValues[2];
   int finalTime, numCells, cellID, argNum = 1;
   int driverPipe[2], tempPipe[2];
   char *params[MAX_PARAMS], *strCursor;

   scanf("%lf %lf %d %d", endValues, endValues + 1, &finalTime, &numCells);

   //Set first argument parameter to program call "Cell"
   *params = malloc(strlen("Cell") + 1);
   strcpy(*params, "Cell");

   for (cellID = 0; cellID < numCells; cellID++) {
      //Set cell ID number
      makeIntParameter(params[argNum++], 'S', numCells);

      //If first cell
      if(!cellID) {
         makeDoubleParameter(params[argNum++], endValues[0]);
         makeIntParameter(params[argNum++], 'O', cellID + 1);
      }

      //If last cell
      if(cellID == numCells - 1) {
         makeDoubleParameter(params[argNum++], endValues[1]);
         makeIntParameter(params[argNum++], 'O', cellID - 1);
      }
      /*
      while(temp) {

      }*/
   }

   return 0;
}

void makeIntParameter(char *param, char letter, int value) {
   param = malloc(PARAM_LENGTH);
   sprintf(param, "%c%d", letter, value);
}

void makeDoubleParameter(char *param, double value) {
   param = malloc(PARAM_LENGTH);
   sprintf(param, "V%lf", value);
}
