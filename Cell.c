#include <stdio.h>
#include <stdlib.h>

typedef struct Pipe {
   int fd;
   struct Pipe *next;
} Pipe;

int main(int argc, char *argv[]) {
   int inputInt;
   double inputDoubleDec;
   char inputChar;
   Pipe *cursor;

   Pipe *inputFiles = NULL, *outputFiles = NULL;
   double value;
   int id, simulations, fixed = 0;

   while(--argc && ++argv) {
      inputChar = *(*argv)++;
      
      if(inputChar == 'V') {
         value = inputDoubleDec = 0;
         while(**argv && **argv != '.') {
            value *= 10;
            value += *(*argv)++ - '0';
         }
        
         ++*argv; 
         while(**argv) {
            inputDoubleDec += *(*argv)++ - '0';
            inputDoubleDec /= 10;
         }

         value += inputDoubleDec;
         fixed = 1;
      }
      else {
         inputInt = 0;
         while(**argv) {
            inputInt *= 10;
            inputInt += *(*argv)++ - '0';
         }
         
         switch(inputChar) {
         case 'S':
            simulations = inputInt;
            break;
         
         case 'D':
            id = inputInt;
            break;

         case 'O':
            if(cursor = outputFiles) {
               while(cursor->next)
                  cursor = cursor->next;

               cursor->next = calloc(1, sizeof(Pipe));
               cursor = cursor->next;
               cursor->fd = inputInt;
            }
            else {
               outputFiles = calloc(1, sizeof(Pipe));
               outputFiles->fd = inputInt;
            }
            break;

         case 'I':
            if(cursor = inputFiles) {
               while(cursor->next)
                  cursor = cursor->next;

               cursor->next = calloc(1, sizeof(Pipe));
               cursor = cursor->next;
               cursor->fd = inputInt;
            }
            else {
               inputFiles = calloc(1, sizeof(Pipe));
               inputFiles->fd = inputInt;
            }
            break;
         }
      }
   }

   printf("ID: %d Simulations: %d Value: %.2f\n",id,simulations,value);
   printf("Input Files: ");
   while(inputFiles) {
      printf("%d ",inputFiles->fd);
      inputFiles = inputFiles->next;
   }
   printf("\nOutput Files: ");
   while(outputFiles) {
      printf("%d ",outputFiles->fd);
      outputFiles = outputFiles->next;
   }
   printf("\n");
   
   return 0;
}
