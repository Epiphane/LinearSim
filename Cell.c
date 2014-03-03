#include <stdio.h>
#include <stdlib.h>
#include "Report.h"

typedef struct Pipe {
   int fd;
   struct Pipe *next;
} Pipe;

int main(int argc, char *argv[]) {
   int inputInt, result;
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

   // TESTS TO MAKE SURE PIPES WORK CORRECTLY ---------
   // Say hi, Billy! (Testing the pipes)
   cursor = outputFiles;
   while(cursor) {
      if((result = write(cursor->fd, &stateReport, sizeof(Report))) <= 0)
         printf("%d: Issue writing to %d: write returned %d\n",
          stateReport.id, cursor->fd, result);
      close(cursor->fd);
      cursor = cursor->next;
   }

   // Listen to your buddies, Billy!
   cursor = inputFiles;
   while(cursor) {
      if(read(cursor->fd, &readReport, sizeof(Report))) {
         printf("%d: Cell %d said hi to me!!! :D\n",
          stateReport.id, readReport.id);
      }
      else {
         printf("%d: I didn't hear anything from File %d :( Closing it\n",
          stateReport.id, cursor->fd);
         close(cursor->fd);
      }
      cursor = cursor->next;
   }

   // END THE TEST ------------------------------------

   return fixed ? 42 : 0;
}
