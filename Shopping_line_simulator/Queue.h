#include<stdlib.h>
#include<stdio.h>

#ifndef _Queue_H_INCLUDE_
#define _Queue_H_INCLUDE_

typedef struct IntegerQueueObj* IntegerQueue;

IntegerQueue newIntegerQueue();

void freeIntegerQueue(IntegerQueue* pQ);

int isEmpty(IntegerQueue Q);

int length(IntegerQueue Q);

void enqueue(IntegerQueue Q, int x);

int dequeue(IntegerQueue Q);

int peek(IntegerQueue Q);

void dequeueAll(IntegerQueue Q);

char* IntegerQueueToString(IntegerQueue Q);

int equals(IntegerQueue Q, IntegerQueue R);

#endif
