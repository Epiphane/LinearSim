#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Report.h"

#define MAGIC_STALEY 42

typedef struct Pipe {
   int fd;
   struct Pipe *next;
} Pipe;

void SetupCell(int argc, char **argv, Report *stateReport, 
 int *simulations, Pipe **inputFiles, Pipe **outputFiles);
void PipeOut(Pipe *cursor, Report *stateReport);
void PipeListen(Pipe *cursor, Report *stateReport, Report readReport);

int main(int argc, char *argv[]) {
   Pipe *cursor;
   Report stateReport, readReport;
   int simulations;
   Pipe *inputFiles = NULL, *outputFiles = NULL;

   SetupCell(argc, argv, &stateReport, &simulations, &inputFiles, &outputFiles);

   for (; stateReport.step <= simulations; stateReport.step++) {
      cursor = outputFiles;
      PipeOut(cursor, &stateReport);
      cursor = inputFiles;
      PipeListen(cursor, &stateReport, readReport);
   }

   cursor = outputFiles;
   while (cursor) {
      close(cursor->fd);
      cursor = cursor->next;
   }

   return inputFiles ? 0 : STALEY;
}

void SetupCell(int argc, char **argv, Report *stateReport, int *simulations, 
 Pipe **inputFiles, Pipe **outputFiles) {
   int inputInt;
   double inputDoubleDec;
   char inputChar;
   Pipe *cursor;

   while (--argc && ++argv) {
      inputChar = *(*argv)++;

      if (inputChar == 'V') {
         sscanf(*argv, "%lf ", &stateReport->value);
      }
      else {
         sscanf(*argv, "%d ", &inputInt);

         switch (inputChar) {
         case 'S':
            *simulations = inputInt;
            break;

         case 'D':
            stateReport->id = inputInt;
            break;

         case 'O':
            cursor = malloc(sizeof(Pipe));
            cursor->fd = inputInt;
            cursor->next = *outputFiles;
            *outputFiles = cursor;
            break;

         case 'I':
            cursor = malloc(sizeof(Pipe));
            cursor->fd = inputInt;
            cursor->next = *inputFiles;
            *inputFiles = cursor;
            break;
         }
      }
   }

   stateReport->step = 0;
}

void PipeOut(Pipe *cursor, Report *stateReport) {
   int result;

   while (cursor) {
      result = write(cursor->fd, stateReport, sizeof(Report));

      cursor = cursor->next;
   }
}

void PipeListen(Pipe *cursor, Report *stateReport, Report readReport) {
   int result, count = 0;
   double average = 0;

   while (cursor) {
      if (read(cursor->fd, &readReport, sizeof(Report))) {
         count++;
         average += readReport.value;
      }
      cursor = cursor->next;
   }

   if (count) {
      average /= count;
      stateReport->value = average;
   }
}
