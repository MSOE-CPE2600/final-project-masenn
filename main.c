/**
 * @file main.c
 * @brief main file for final lab 2024
 * 
 * Course: CPE2600
 * Section: 011
 * Assignment: Final
 * Name: Matthew Senn
 */

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include "wav.h"

void load_params(char* file, int nprocs);

double to_double(char* c) {
    char *endptr;
    double value;
    // Convert string to double
    value = strtod(c, &endptr);

    // Check for conversion errors
    if (*endptr != '\0') {
        printf("Conversion error with string \"%s\"\n",c);
        return 0;
    } else {
        return value;
    }
}

int main(){
    load_params("config.csv",3);
}

void load_params(char* file, int nprocs){
   FILE* fp;
   fp = fopen(file,"r");
   if(fp==NULL){
      printf("Cannot find file: %s\n",file);
      return;
   }
   WaveParameters params[20];
   char buf[300];
   char* data[8];
   int count = 0; 
   int isHeader = 1;
   int line = 1;
   int num_params = 0;
   while(fgets(buf,sizeof(buf),fp)!=NULL){
      //placing into the array
      char* token = strtok(buf, " \n,\r");
      while (token != NULL) {
         if(count > 8) {
            printf("Error in csv formatting!\n\n"); 
            break;
         }
         data[count] = token;
         count++;
        //  printf("%d: %s\n",count,data[count-1]);
         token = strtok(NULL, " \n,\r");
      }
      if(count!=8){
         printf("Error on line %d. Returning!\n",line);
         return;
      }
      count = 0; 
      if(!isHeader){
        WaveParameters wave;
        static char name[64];
        strcpy(name, data[0]);
      //   printf("name: %s\n---\n",data[0]);
        //name,samp_freq,samp_channels,samp_length,bit_depth,amp_depth,freq_depth,rot_freq
        wave.sample_freq = (int)to_double(data[1]);
        wave.sample_channels = (int)to_double(data[2]);
        wave.sample_length = (int)to_double(data[3]);
        wave.bit_depth = (int)to_double(data[4]);
        wave.amp_depth = to_double(data[5]);
        wave.freq_depth = to_double(data[6]);
        wave.rot_freq = to_double(data[7]);
        wave.MAX_VAL = pow(2,wave.bit_depth)/2;
         wave.name = strdup(data[0]);  // Allocate memory and copy the name
         params[num_params] = wave;
         num_params++;
      } else {
         isHeader = 0; 
      }
      line++;
   }
   fclose(fp);
   
   int total_generations = num_params; // Total number of generations to split
   int generations_per_proc = total_generations / nprocs;
   int remaining_generations = total_generations % nprocs;

   pid_t pids[nprocs];

   for (int i = 0; i < nprocs; i++) {
      if ((pids[i] = fork()) == 0) { // Child process
         int start_generation = i * generations_per_proc;
         int end_generation = start_generation + generations_per_proc;

         // Last process handles any remaining generations
         if (i == nprocs - 1) {
               end_generation += remaining_generations;
         }

         for (int gen = start_generation; gen < end_generation; gen++) {
               generate_package(&params[gen]);
         }

         // Exit child process to prevent continuing into parent code
         exit(0);
      } else if (pids[i] < 0) { // Fork failed
         perror("fork");
         exit(-1);
      }
      }

      // Parent process waits for all child processes to complete
      for (int i = 0; i < nprocs; i++) {
         wait(NULL);
      }

      for(int i = 0; i < num_params; i++){
         generate_package(&params[i]);
   }
}