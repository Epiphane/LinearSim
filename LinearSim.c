#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Report.h"

#define PARAM_LENGTH 10
#define MAX_PARAMS 9

void ForkCells(int numCells, int *cellIDs, int finalTime, int *driverPipe, double *endValues);
int MakeCell(char **params);
void MovePipes(int *leftPipeIn, int *leftPipeOut, int *rightPipeIn, int *rightPipeOut);
char *MakeIntParameter(char letter, int value);
char *MakeDoubleParameter(double value);

int main() {
   double endValues[2];
   int finalTime, numCells, cellID;
   int driverPipe[2];
   int *cellIDs, ndx;
   int exitStatus;
   Report cellReport;
   char buffer[100];

   // Create pipe for driver
   pipe(driverPipe);

   scanf("%lf %lf %d %d", endValues, endValues + 1, &finalTime, &numCells);
   cellIDs = malloc(sizeof(int) * numCells);

   ForkCells(numCells, cellIDs, finalTime, driverPipe, endValues);

   // Cells initialized: Free the write end of driverPipe from LinearSim
   close(driverPipe[1]);

   while(read(driverPipe[0],&cellReport,sizeof(Report))) {
      printf("Result from %d, step %d: %.3f\n",cellReport.id, cellReport.step, cellReport.value);
   }

   // Close reader
   close(driverPipe[0]);

   while(numCells--) {
      cellID = wait(&exitStatus);
      for(ndx = 0; cellID != cellIDs[ndx]; ndx++)
         ;
      printf("Child %d exits ",ndx);
      printf("with %d\n",WEXITSTATUS(exitStatus));
   }

   return 0;
}

/* VISUAL DIAGRAM OF WHAT ITS GONNA END LIKE
 * The rightmost cell is created FIRST
 * Notes: LPO/LPI = leftPipeOut, leftPipeIn
 *        RPO/RPI = rightPipeOut, rightPipeIn
 *
 * Cell 0 I(RPI) <--- O(LPO) Cell 1 I(RPI) <--- O(LPO) Cell 2
 *    |   O(RPO) ---> I(LPI)    |   O(RPO) ---> I(LPI)    |
 *    v                         v                         v
 * (LinearSim)             (LinearSim)              (LinearSim)
 */
void ForkCells(int numCells, int *cellIDs, int finalTime, int *driverPipe, double *endValues) {
   int argNum;
   int rightPipeOut[2], rightPipeIn[2];
   int leftPipeOut[2], leftPipeIn[2];
   char *params[MAX_PARAMS], *strCursor;

   //Set first argument parameter to program call "Cell"
   *params = malloc(strlen("Cell") + 1);
   strcpy(*params, "Cell");

   // Set rightPipeIn[0] to -1, so that we know that no cells have been initialized yet.
   // rightPipeIn is first set up when we're making cell [numCells].
   rightPipeIn[0] = -1;

   while (numCells--) {
      argNum = 1;

      //Set cell ID number
      params[argNum++] = MakeIntParameter('D', numCells);
      params[argNum++] = MakeIntParameter('S', finalTime);
      params[argNum++] = MakeIntParameter('O', driverPipe[1]);

      // If last cell (first one created, largest ID)
      if(rightPipeIn[0] == -1) {
         params[argNum++] = MakeDoubleParameter(endValues[1]);
      }
      // Otherwise we have an input/output situation on the right
      else {
         params[argNum++] = MakeIntParameter('O', rightPipeOut[1]);
         params[argNum++] = MakeIntParameter('I', rightPipeIn[0]);
      }

      // If first cell (ID: 0, last one created)
      if(!numCells) {
         params[argNum++] = MakeDoubleParameter(endValues[0]);
      }
      // Otherwise we have an input/output situation on the left too!
      else {
         // Create leftPipe
         pipe(leftPipeOut);
         pipe(leftPipeIn);

         params[argNum++] = MakeIntParameter('O', leftPipeOut[1]);
         params[argNum++] = MakeIntParameter('I', leftPipeIn[0]);
      }

      params[argNum] = NULL;

      cellIDs[numCells] = MakeCell(params);

      MovePipes(leftPipeIn, leftPipeOut, rightPipeIn, rightPipeOut);
   }
}

int MakeCell(char **params) {
   int result;

   if(result = fork())
      ;
   else
      execv("Cell", params);

   return result;
}

/*
 * Change pipes
 * rightPipeOut ----> leftPipeIn
 * rightPipeIn  <---- leftPipeOut
 */
void MovePipes(int *leftPipeIn, int *leftPipeOut, int *rightPipeIn, int *rightPipeOut) {
   if(rightPipeIn[0] != -1) {
      close(rightPipeIn[0]);
      close(rightPipeIn[1]);
      close(rightPipeOut[0]);
      close(rightPipeOut[1]);
   }

   rightPipeIn[0] = leftPipeOut[0];
   rightPipeIn[1] = leftPipeOut[1];
   rightPipeOut[0] = leftPipeIn[0];
   rightPipeOut[1] = leftPipeIn[1];
}

char *MakeIntParameter(char letter, int value) {
   char *param = malloc(PARAM_LENGTH);

   sprintf(param, "%c%d", letter, value);

   return param;
}

char *MakeDoubleParameter(double value) {
   char *param = malloc(PARAM_LENGTH * 2);

   sprintf(param, "V%f", value);

   return param;
}
