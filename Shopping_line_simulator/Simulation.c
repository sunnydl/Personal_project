#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>
#include"Job.h"
#include"Queue.h"

Job getJob(FILE* in){
   int a, d;
   fscanf(in, "%d %d\n", &a, &d);
   return newJob(a, d);
}

int getFastestQueue(IntegerQueue* QQ, int j){
   int Minlen = 1;
   for(int w = 1; w <= j; w++){
      if(length(QQ[Minlen]) > length(QQ[w])){
         Minlen = w;
      }
   }
   return Minlen;
}

void putJobInQueue(IntegerQueue Storage, int len){
   if(!isEmpty(Storage)){
      dequeueAll(Storage);
   }
   for(int i = 0; i < len; i++){
      enqueue(Storage, i);
   }
}

int grabFirstFinish(IntegerQueue* QQ, int Qlen){
   for(int i = 1; i <= Qlen; i++){
      if(!isEmpty(QQ[i])){
         return i;
      }
   }
   return 0;
}

int firstIsHere(Job J, int A){
   int j = getArrival(J);
   if(A == j){
      return 1;
    }
   return 0;
}

void printState(FILE* trace, IntegerQueue* processor, Job* reuse, int time, int i){
   fprintf(trace, "time=%d\n", time);
      for(int j = 0; j <= i; j++){
         if(!isEmpty(processor[j])){
            int len = length(processor[j]);
            fprintf(trace, "%d: ", j);
            for(int p = 0; p < len; p++){
               int t1 = dequeue(processor[j]);
               printJob(trace, reuse[t1]);
               enqueue(processor[j], t1);
            }
            fprintf(trace, "\n");
         }else{
            fprintf(trace, "%d:\n", j);
         }
      }
      fprintf(trace, "\n");
}

int conditionTrue(IntegerQueue* processor, Job* reuse, int numJ, int firstArrive){
   int t = (getArrival(reuse[peek(processor[0])]) != firstArrive) || (getFinish(reuse[numJ-1]) == UNDEF);
   return (t || length(processor[0])<numJ);
}

int main(int argc, char* argv[]){
   FILE* in;
   FILE* report;
   FILE* trace;
   int numJ;
   int* waitTime;
   Job* reuse;
   IntegerQueue* processor;

   if(argc!=2){
      printf("Usage: %s <input file>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   in = fopen(argv[1], "r");
   if(in == NULL){
      printf("Unable to read from file %s\n", argv[1]);
      exit(EXIT_FAILURE);
   }

   char* inName = malloc(strlen(argv[1]+5));

   sprintf(inName, "%s.rpt", argv[1]);
   report = fopen(inName, "wt");
   if(report == NULL){
      printf("Unable to write to file report\n");
      exit(EXIT_FAILURE);
   }

   sprintf(inName, "%s.trc", argv[1]);
   trace = fopen(inName, "wt");
   if(trace == NULL){
      printf("Unable to write to file trace\n");
      exit(EXIT_FAILURE);
   }

   free(inName);

   fscanf(in, "%d", &numJ);
   reuse = malloc(numJ*sizeof(Job));
   processor = malloc(numJ*sizeof(IntegerQueue));
   waitTime = calloc(numJ, sizeof(int));

   assert(reuse!=NULL);
   assert(processor!=NULL);
   assert(waitTime!=NULL);

   for(int i = 0; i < numJ; i++){
      processor[i] = newIntegerQueue();
   }
   for(int i = 0; i < numJ; i++){
      reuse[i] = getJob(in);
   }

   fprintf(report, "Report file: %s.rpt\n", argv[1]);
   fprintf(report, "%d Jobs:\n", numJ);
   for(int i = 0; i < numJ; i++){
      printJob(report, reuse[i]);
   }
   fprintf(report, "\n");
   fprintf(report, "***********************************************************\n");


   fprintf(trace, "Trace file: %s.trc\n", argv[1]);
   fprintf(trace, "%d Jobs:\n", numJ);
   for(int i = 0; i < numJ; i++){
      printJob(trace, reuse[i]);
   }
   fprintf(trace, "\n");
   fprintf(trace, "\n");

   for(int i = 1; i < numJ; i++){
      putJobInQueue(processor[0], numJ);

      for(int j = 0; j < numJ; j++){
         resetFinishTime(reuse[j]);
      }
      int time = 0;
      int firstArrive = getArrival(reuse[0]);
      fprintf(trace, "*****************************\n");
      if(i == 1){
         fprintf(trace, "%d processor:\n*****************************\n", i);
      }else{
         fprintf(trace, "%d processors:\n*****************************\n", i);
      }

      printState(trace, processor, reuse, time, i);

      while(conditionTrue(processor, reuse, numJ, firstArrive)){
         if(time == 0){
            int arrive = getArrival(reuse[peek(processor[0])]);
            time = arrive;

            int tlen = length(processor[0]);

            for(int d = 0; d < tlen; d++){
               int tArrive = getArrival(reuse[peek(processor[0])]);
               int dde = dequeue(processor[0]);
               if(arrive == tArrive){
                  int quickest = getFastestQueue(processor, i);
                  enqueue(processor[quickest], dde);
                  if(length(processor[quickest])==1){
                     computeFinishTime(reuse[peek(processor[quickest])], time);
                  }
               }else{
                  enqueue(processor[0], dde);
               }
            }

            printState(trace, processor, reuse, time, i);
         }

         int arrive = getArrival(reuse[peek(processor[0])]);
         int finishQ = grabFirstFinish(processor, i);
         int finish = getFinish(reuse[peek(processor[finishQ])]);

         for(int j = 1; j <= i; j++){
            if(!isEmpty(processor[j])){
               int tFinish = getFinish(reuse[peek(processor[j])]);
               if(finish > tFinish){
                  finish = tFinish;
                  finishQ = j;
                }
            }
         }

         if((arrive < finish) && (arrive!=firstArrive)&&(getFinish(reuse[peek(processor[0])])==UNDEF)){
            time = arrive;

            int tlen = length(processor[0]);
            int tArrive = 0;
            for(int d = 0; d < tlen; d++){
               tArrive = getArrival(reuse[peek(processor[0])]);
               int dde = dequeue(processor[0]);
               if(arrive == tArrive){
                  int quickest = getFastestQueue(processor, i);
                  enqueue(processor[quickest], dde);
                  if(getArrival(reuse[peek(processor[quickest])]) == arrive){
                     computeFinishTime(reuse[peek(processor[quickest])], time);
                   }
               }else{
                  enqueue(processor[0], dde);
               }
            }

            printState(trace, processor, reuse, time, i);

         }else{
            time = finish;

            int dFinish = 0;//
            for(int d = finishQ; d <= i; d++){//
               dFinish = getFinish(reuse[peek(processor[d])]);
               if(isEmpty(processor[d])){
                  dFinish = 0;
               }

               if(finish == dFinish){
                  int dde = dequeue(processor[d]);
                  enqueue(processor[0], dde);

                  if(!isEmpty(processor[d])){
                     computeFinishTime(reuse[peek(processor[d])], time);
                     }

               }
            }

            if(arrive == finish){
               int len2 = length(processor[0]);
               int tArrive = 0;
               for(int d = 0; d < len2; d++){
                  int dde = dequeue(processor[0]);
                  tArrive = getArrival(reuse[dde]);
                  if(arrive == tArrive){
                     int fq2 = getFastestQueue(processor, i);
                     enqueue(processor[fq2], dde);

                     if(peek(processor[fq2]) == dde){
                        computeFinishTime(reuse[peek(processor[fq2])], time);
                     }
                  }else{
                     enqueue(processor[0], dde);
                  }
                }
            }

            printState(trace, processor, reuse, time, i);

         }
      }
      for(int n = 0; n < numJ; n++){
         waitTime[n] = getWaitTime(reuse[n]);
      }
      int totalWait = 0;
      for(int n = 0; n < numJ; n++){
         totalWait += waitTime[n];
      }
      int maxWait = 0;
      for(int n = 0; n < numJ; n++){
         if(waitTime[n] > maxWait){
            maxWait = waitTime[n];
         }
      }
      double avgWait = (double)totalWait/(double)numJ;

      if(i == 1){
         fprintf(report, "%d processor: totalWait=%d, maxWait=%d, averageWait=%.2f\n", i, totalWait, maxWait, avgWait);
      }else{
         fprintf(report, "%d processors: totalWait=%d, maxWait=%d, averageWait=%.2f\n", i, totalWait, maxWait, avgWait);
      }
   }

   for(int i = 0; i < numJ; i++){
      freeJob(&(reuse[i]));
      freeIntegerQueue(&(processor[i]));
   }
   free(waitTime);
   free(reuse);
   free(processor);

   fclose(in);
   fclose(report);
   fclose(trace);

   return(EXIT_SUCCESS);
}
