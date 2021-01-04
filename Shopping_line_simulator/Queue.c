#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include"IntegerQueue.h"

typedef struct NodeObj{
   int item;
   struct NodeObj* next;
}NodeObj;

typedef NodeObj* Node;

typedef struct IntegerQueueObj{
   Node front;
   Node back;
   int numItems;
} IntegerQueueObj;

Node newNode(int x){
   Node N = malloc(sizeof(NodeObj));
   assert(N!=NULL);
   N->item = x;
   N->next = NULL;
   return N;
}

void freeNode(Node* pN){
   if(pN!=NULL && *pN!=NULL){
      free(*pN);
      *pN = NULL;
   }
}

void freeAllNodes(Node H){
   if(H!=NULL){
      freeAllNodes(H->next);
      freeNode(&H);
   }
}

IntegerQueue newIntegerQueue(){
   IntegerQueue Q = malloc(sizeof(IntegerQueueObj));
   Q->front = Q->back = NULL;
   Q->numItems = 0;

   return Q;
}

void dequeueAll(IntegerQueue Q){
   if(Q==NULL){
      fprintf(stderr, "IntegerQueue Error: makeEmpty() called on NULL integerQueue reference\n");
      exit(EXIT_FAILURE);
   }

   freeAllNodes(Q->front);
   Q->front = Q->back = NULL;
   Q->numItems = 0;
}

void freeIntegerQueue(IntegerQueue* pQ){
   if(pQ!=NULL && *pQ!=NULL){
      dequeueAll(*pQ);
      free(*pQ);
      *pQ = NULL;
   }
}

int isEmpty(IntegerQueue Q){
   if(Q==NULL){
      fprintf(stderr, "IntegerQueue Error: isEmpty() called on NULL integerQueue reference\n");
      exit(EXIT_FAILURE);
   }

   return (Q->numItems == 0);
}

int length(IntegerQueue Q){
   if(Q==NULL){
      fprintf(stderr, "IntegerQueue Error: length() called on NULL integerQueue reference\n");
      exit(EXIT_FAILURE);
   }

   return (Q->numItems);
}

void enqueue(IntegerQueue Q, int x){
   if(Q==NULL){
       fprintf(stderr, "IntegerQueue Error: enqueue() called on NULL integerQueue reference\n");
      exit(EXIT_FAILURE);
   }

   Node N = newNode(x);
   if(Q->front==NULL){
      Q->front = N;
      Q->back = N;
   }
   else{
      Q->back->next = N;
      Q->back = N;
   }
   (Q->numItems)++;
}

int dequeue(IntegerQueue Q){
   if(Q==NULL){
      fprintf(stderr, "IntegerQueue Error: dequeue() called on NULL integerQueue reference\n");
      exit(EXIT_FAILURE);
   }

   if(!isEmpty(Q)){
      int de = Q->front->item;
      Node N = Q->front;
      if(Q->front == Q->back){
         Q->front = Q->back = NULL;
      }else{
         Q->front = Q->front->next;
      }

      freeNode(&N);
      (Q->numItems)--;
      return de;
   }else{
      return 0;
   }
}

int peek(IntegerQueue Q){
   if(Q==NULL){
      fprintf(stderr, "IntegerQueue Error: peek() called on NULL integerQueue reference\n");
      exit(EXIT_FAILURE);
   }

   if(!isEmpty(Q)){
      return (Q->front->item);
   }else{
      return 0;
   }
}

int countNum(Node NF){
   int count = 0;
   char tempstr [10];
   while(NF!=NULL){
      sprintf(tempstr, "%d", NF->item);
      count = count + strlen(tempstr);
      count++;
      NF = NF->next;
   }

   return count;
}

int countNumMinor(Node H){
   char tempstr2 [10];
   sprintf(tempstr2, "%d", H->item);
    int count = strlen(tempstr2);
   return count;
}

void putIntegerToCharArray(char* charArray, Node NF){
   while(NF!=NULL){
      //putIntegerToCharArray(charArray, NF->next);
      int templen = countNumMinor(NF);
      char tempCharArray [templen];
      sprintf(tempCharArray, "%d", NF->item);
      strcat(charArray, tempCharArray);
      strcat(charArray, " ");
      NF = NF->next;
   }
}

char* IntegerQueueToString(IntegerQueue Q){
   if(Q==NULL){
      fprintf(stderr, "IntegerQueue Error: IntegerQueueToString() called on NULL integerQueue reference\n");
      exit(EXIT_FAILURE);
   }

   Node N = Q->front;
   int len = countNum(N);
   char* IntegerQS = calloc(len, sizeof(char));
   putIntegerToCharArray(IntegerQS, N);
   IntegerQS[len-1] = '\0';

   return IntegerQS;
}

int equals(IntegerQueue Q, IntegerQueue R){
   if(Q==NULL){
      fprintf(stderr, "IntegerQueue Error: equals() called on NULL integerQueue reference\n");
      exit(EXIT_FAILURE);
   }

   int equal = (Q->numItems == R->numItems);
   if(equal==0){
      return 0;
   }
   Node H = Q->front;
   Node N = R->front;
   while(H!=NULL && N!=NULL){
      if((H->item)!=(N->item)){
         return 0;
      }
      H = H->next;
      N = N->next;
   }
   return equal;
}

