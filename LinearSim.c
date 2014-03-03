#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Report.h"

#define PARAM_LENGTH 10
#define MAX_PARAMS 9

char *makeIntParameter(char letter, int value);
char *makeDoubleParameter(double value);

int main() {
   double endValues[2];
   int finalTime, numCells, cellID, argNum;
   int driverPipe[2];
   int rightPipeOut[2], rightPipeIn[2];
   int leftPipeOut[2], leftPipeIn[2];
   char *params[MAX_PARAMS], *strCursor;
   Report cellReport;

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
   cellID = numCells;
   while (cellID--) {
      argNum = 1;
      
      //Set cell ID number
      params[argNum++] = makeIntParameter('D', cellID);
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
         if(pipe(leftPipeOut))
            printf("Error creating leftPipeOut\n");
         if(pipe(leftPipeIn))
            printf("Error creating leftPipeIn\n");
         printf("New pipes: [%d %d] [%d %d]\n",leftPipeOut[0],leftPipeOut[1],leftPipeIn[0],leftPipeIn[1]);

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
      if(0 || rightPipeIn[0] != -1) {
         close(rightPipeIn[0]);
         close(rightPipeIn[1]);
         close(rightPipeOut[0]);
         close(rightPipeOut[1]);
      }

      // Change pipes
      // rightPipeOut ----> leftPipeIn
      // rightPipeIn  <---- leftPipeOut
      rightPipeIn[0] = leftPipeOut[0];
      rightPipeIn[1] = leftPipeOut[1];
      rightPipeOut[0] = leftPipeIn[0];
      rightPipeOut[1] = leftPipeIn[1];
   }

   // Cells initialized: Free the write end of driverPipe from LinearSim
   close(driverPipe[1]);

   // TODO: Main code
   while(read(driverPipe[0],&cellReport,sizeof(Report))) {
      printf("Driver: %d said hi to me! :D\n",cellReport.id);
   }
   
   // Close reader
   close(driverPipe[0]);
   
   while(numCells--) {
      printf("Child %d exits ",wait(&cellID));
      printf("which %d\n",WEXITSTATUS(cellID));
   }

   return 0;
}

char *makeIntParameter(char letter, int value) {
   char *param = malloc(PARAM_LENGTH);
   
   sprintf(param, "%c%d", letter, value);

   return param;
}

char *makeDoubleParameter(double value) {
   char *param = malloc(PARAM_LENGTH * 2);
   
   sprintf(param, "V%f", value);

   return param;
}
