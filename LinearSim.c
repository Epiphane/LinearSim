#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Report.h"

#define PARAM_LENGTH 10
#define MAX_PARAMS 8

char *makeIntParameter(char letter, int value);
char *makeDoubleParameter(double value);

int main() {
   double endValues[2];
   int finalTime, numCells, cellID, argNum = 1;
   int driverPipe[2];
   int rightPipeOut[2], rightPipeIn[2];
   int leftPipeOut[2], leftPipeIn[2];
   char *params[MAX_PARAMS], *strCursor;

   // Create pipe for driver
   pipe(driverPipe);

   // Set rightPipeIn[0] to -1, so that we know that no cells have been initialized yet.
   // rightPipeIn is first set up when we're making cell [numCells].
   rightPipeIn[0] = -1;

   scanf("%lf %lf %d %d", endValues, endValues + 1, &finalTime, &numCells);

   //Set first argument parameter to program call "Cell"
   *params = malloc(strlen("Cell") + 1);
   strcpy(*params, "Cell");

   // VISUAL DIAGRAM OF WHAT ITS GONNA END LIKE
   // The rightmost cell is created FIRST
   // Notes: LPO/LPI = leftPipeOut, leftPipeIn 
   //        RPO/RPI = rightPipeOut, rightPipeIn
   //
   // Cell 0 I(RPI) <--- O(LPO) Cell 1 I(RPI) <--- O(LPO) Cell 2
   //    |   O(RPO) ---> I(LPI)    |   O(RPO) ---> I(LPI)    |
   //    v                         v                         v
   // (LinearSim)             (LinearSim)              (LinearSim)
   while (numCells--) {
      //Set cell ID number
      params[argNum++] = makeIntParameter('D', numCells);
      params[argNum++] = makeIntParameter('S', finalTime);
      params[argNum++] = makeIntParameter('O', driverPipe[1]);

      // If last cell (first one created, largest ID)
      if(rightPipeIn[0] == -1) {
         params[argNum++] = makeDoubleParameter(endValues[1]);
      }
      // Otherwise we have an input/output situation on the right
      else {
         params[argNum++] = makeIntParameter('O', rightPipeOut[1]);
         params[argNum++] = makeIntParameter('I', rightPipeIn[0]);
      }

      // If first cell (ID: 0, last one created)
      if(!cellID) {
         params[argNum++] = makeDoubleParameter(endValues[0]);
      }
      // Otherwise we have an input/output situation on the left too!
      else {
         // Create leftPipe
         pipe(leftPipeOut);
         pipe(leftPipeIn);

         params[argNum++] = makeIntParameter('O', leftPipeOut[1]);
         params[argNum++] = makeIntParameter('I', leftPipeIn[0]);
      }

      params[argNum] = NULL;

      // Make a child and have it run Cell
      if(fork() == 0) {
         execv("Cell",params);
         printf("Cell creation failed\n");
      }

      // Free rightPipe since the driver don't care
      if(rightPipeIn[0] != -1) {
         close(rightPipeIn[0]);
         close(rightPipeIn[1]);
         close(rightPipeOut[0]);
         close(rightPipeOut[1]);
      }

      // Change pipes
      // rightPipeOut ----> leftPipeIn
      // rightPipeIn  <---- leftPipeOut 
      rightPipeIn = leftPipeOut;
      rightPipeOut = leftPipeIn;
   }

   // Cells initialized: Free the write end of driverPipe from LinearSim
   close(driverPipe[1]);

   // TODO: Main code

   // Close reader
   close(driverPipe[0]);

   return 0;
}

char *makeIntParameter(char letter, int value) {
   char *param = malloc(PARAM_LENGTH);
   
   sprintf(param, "%c%d", letter, value);

   return param;
}

char *makeDoubleParameter(double value) {
   char *param = malloc(PARAM_LENGTH);
   
   sprintf(param, "V%lf", value);

   return param;
}
