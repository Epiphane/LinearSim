#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Report.h"

typedef struct Pipe {
   int fd;
   struct Pipe *next;
} Pipe;

void PipeOut(Pipe *cursor, Report stateReport);
void PipeListen(Pipe *cursor, Report *stateReport, Report readReport);

int main(int argc, char *argv[]) {
   int inputInt;
   double inputDoubleDec;
   char inputChar;
   Pipe *cursor;

   Report stateReport, readReport;
   Pipe *inputFiles = NULL, *outputFiles = NULL;
   int simulations, fixed = 0;

   while(--argc && ++argv) {
      inputChar = *(*argv)++;

      if(inputChar == 'V') {
         sscanf(*argv, "%lf ", &stateReport.value);
         fixed = 1;
      }
      else {
         sscanf(*argv, "%d ", &inputInt);

         switch(inputChar) {
         case 'S':
            simulations = inputInt;
            break;

         case 'D':
            stateReport.id = inputInt;
            break;

         case 'O':
            cursor = malloc(sizeof(Pipe));
            cursor->fd = inputInt;
            cursor->next = outputFiles;
            outputFiles = cursor;
            break;

         case 'I':
            cursor = malloc(sizeof(Pipe));
            cursor->fd = inputInt;
            cursor->next = inputFiles;
            inputFiles = cursor;
            break;
         }
      }
   }

   // Set up Report to know it's at the start
   stateReport.step = 0;

   cursor = outputFiles;
   PipeOut(cursor, stateReport);
   cursor = inputFiles;
   PipeListen(cursor, &stateReport, readReport);

   // END THE TEST ------------------------------------

   return fixed ? 42 : 0;
}

void PipeOut(Pipe *cursor, Report stateReport) {
   // TESTS TO MAKE SURE PIPES WORK CORRECTLY ---------
   // Say hi, Billy! (Testing the pipes)
   int result;

   while(cursor) {
      if((result = write(cursor->fd, &stateReport, sizeof(Report))) <= 0)
         printf("%d: Issue writing to %d: write returned %d\n",
          stateReport.id, cursor->fd, result);
      close(cursor->fd);
      cursor = cursor->next;
   }
}

void PipeListen(Pipe *cursor, Report *stateReport, Report readReport) {
   // Listen to your buddies, Billy!
   int result, count = 0;
   double average = 0;

   while(cursor) {
      if(read(cursor->fd, &readReport, sizeof(Report))) {
         printf("%d: Cell %d said hi to me!!! :D\n",
          stateReport->id, readReport.id);

         count++;
         average += readReport.value;
      }
      else {
         printf("%d: I didn't hear anything from File %d :( Closing it\n",
          stateReport->id, cursor->fd);
         close(cursor->fd);
      }
      cursor = cursor->next;
   }

   if (count) {
      average /= count;
      stateReport->value = average;
   }
}
