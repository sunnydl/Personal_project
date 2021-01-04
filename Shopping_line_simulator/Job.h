#include<stdlib.h>
#include<stdio.h>

#ifndef _JOB_H_INCLUDE_
#define _JOB_H_INCLUDE_

#define UNDEF -1

typedef struct JobObj* Job;

Job newJob(int a, int d);

void freeJob(Job* pJ);

int getArrival(Job J);

int getDuration(Job J);

int getFinish(Job J);

int getWaitTime(Job J);

void computeFinishTime(Job J, int timeNow);

void resetFinishTime(Job J);

void printJob(FILE* out, Job J);


#endif