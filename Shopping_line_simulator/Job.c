#include<stdlib.h>
#include<stdio.h>
#include"Job.h"

typedef struct JobObj{
   int arrival;
   int duration;
   int finish;
} JobObj;

Job newJob(int a, int d){
   Job J = malloc(sizeof(JobObj));
   J->arrival = a;
   J->duration = d;
   J->finish = UNDEF;
   return J;
}

void freeJob(Job* pJ){
   if( pJ!=NULL && *pJ!=NULL ){
      free(*pJ);
      *pJ = NULL;
   }
}

int getArrival(Job J){
   if( J==NULL ){
      fprintf(stderr,
         "Job Error: calling getArrival() on NULL Job reference\n");
      exit(EXIT_FAILURE);
   }

   return J->arrival;
}

int getDuration(Job J){
   if( J==NULL ){
      fprintf(stderr,
         "Job Error: calling getDuration() on NULL Job reference\n");
      exit(EXIT_FAILURE);
   }

   return J->duration;
}


int getFinish(Job J){
   if( J==NULL ){
       fprintf(stderr,
         "Job Error: calling getFinish() on NULL Job reference\n");
      exit(EXIT_FAILURE);
   }

   return J->finish;
}

int getWaitTime(Job J){
   if( J==NULL ){
      fprintf(stderr,
         "Job Error: calling getWaitTime() on NULL Job reference\n");
      exit(EXIT_FAILURE);
   }
   if( (J->finish)==UNDEF ){
      fprintf(stderr,
         "Job Error: calling getWaitTime() when finish time undefined\n");
      exit(EXIT_FAILURE);
   }

   return (J->finish) - (J->duration) - (J->arrival);
}

void computeFinishTime(Job J, int timeNow){
   if( J==NULL ){
      fprintf(stderr,
         "Job Error: computeFinishTime() on NULL Job reference\n");
      exit(EXIT_FAILURE);
   }

   (J->finish) = timeNow + (J->duration);
}

void resetFinishTime(Job J){
   if( J==NULL ){
      fprintf(stderr,
         "Job Error: resetFinishTime() on NULL Job reference\n");
      exit(EXIT_FAILURE);
   }

   (J->finish) = UNDEF;
}

void printJob(FILE* out, Job J){
   if( J==NULL ){
       fprintf(stderr,
         "Job Error: printJob() on NULL Job reference\n");
      exit(EXIT_FAILURE);
   }

   fprintf(out, "(%d, ", J->arrival);
   fprintf(out, "%d, ", J->duration);
   if( (J->finish)==UNDEF ){
      fprintf(out, "*) ");
   }else{
      fprintf(out, "%d) ", J->finish);
   }
}