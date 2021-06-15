#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <ctype.h>
#include <sys/types.h>

#include <pthread.h>


// queue data structure with Linked list implementation
// the queue basic data structure is insipred by geeksforgeeks
// and I customized it a lot to make it work for the multi-threaded httpserver
////////////////////////////////////////////////////////////////////////////
struct QNode {
  int connection;
  struct QNode* next;
};

struct Queue {
  int log_file; // place to store the log file descriptor, when it is -1, it means it is being occupied
  int need_to_log; // indicator of whether need to log or not
  struct QNode *front, *rear;
};
// initialization of a new node with 1 connection stored within
struct QNode* newNode(int c){
  struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
  temp->connection = c;
  temp->next = NULL;
  return temp;
}
// initialization of queue with linked list implementation
// the queue also store the log file descriptor and a flag for logging
struct Queue* newQueue(int l_f, int need_log){
  struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
  q->front = q->rear = NULL;
  q->log_file = l_f;
  q->need_to_log = need_log;
  return q;
}

// function used to put file descriptor back to queue
void putFileBackQueue(struct Queue* q, int f){
  q->log_file = f; // put the file descriptor back so other threads can take it now
}

// function used to get file descriptor for log file, after grabbing it, it will set it to -1 to indicate it is being occupied
int getFileFromQueue(struct Queue* q){
  int logFile = q->log_file;
  q->log_file = -1; // set the log file variable to -1 just so other threads know that it is occupied
  return logFile;
}
// basic enqueue function to add new connection to the queue
void enQueue(struct Queue* q, int c){
  struct QNode* temp = newNode(c);

  // if queue was empty
  if(q->rear==NULL){
    q->front = q->rear = temp;
    return;
  }

  // if not empty then just append it at the end
  q->rear->next = temp;
  q->rear = temp;
}
// dequeue function that return the connection on top of the queue,
// it will return -1 if the queue is empty
int deQueue(struct Queue* q){
  int value;
  // if queue is empty then return nothing
  if(q->front==NULL){
    return -1;
  }
  // grab the front node out of the queue and update the order
  struct QNode* temp = q->front;
  q->front = q->front->next;
  // if after grabbing the node out the queue is emtpy, then set the rear to empty too
  if(q->front==NULL){
    q->rear = NULL;
  }
  // grab the value from the connection and clear the memory for that node
  value = temp->connection;
  free(temp);
  return value;
}
/////////////////////////////////////////////////////////////////////////////////////////

pthread_mutex_t mutex_queue; // mutex and condition_var for queue
pthread_cond_t condition_queue; // mutex and condition_var for queue
pthread_mutex_t mutex_log; // mutex and condition_var for logging
pthread_cond_t condition_log; // mutex and condition_var for logging

/* 
  function for grabbing the log file from the queue,
  if the queue returns -1, it will mean the log file
  is currently being used, and the conditional variable
  for log file will tell the thread to wait until signaled
  that the log file is free to grab, it also lock the critical
  region using the mutex for log file
*/
int getLogFile(struct Queue* q){
  // check if log is needed
  if(!(q->need_to_log)){
    return 0;
  }

  int log_file;

  // get the log descriptor from the queue data structure
  pthread_mutex_lock(&mutex_log);
  if((log_file = getFileFromQueue(q)) == -1){
    pthread_cond_wait(&condition_log, &mutex_log);
    // wait until we can get the descriptor
    log_file = getFileFromQueue(q);
  }
  pthread_mutex_unlock(&mutex_log);
  return log_file;
}

/*
  function used to implement the logging, it will
  first check if the log is needed, if not it will simply
  return by doing nothing. Else it will proceed to see if
  the request to log is successful or not, if it is successful
  it will log the request using the success format, if not it will
  log the file using the fail format. After logging, it will
  put back the file descriptor to the queue data structure
*/
void log_to_file(int successful, int log_file, int err_code, long int content_length, char* requestType, char* filePath, char* httpVersion, char* hostName, struct Queue* q){
  // check if log is needed
  if(!(q->need_to_log)){
    return;
  }

  // log
  if(successful){
    // if successful
    // log the request type
    if(write(log_file, requestType, strlen(requestType)) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    if(write(log_file, "\t/", 2) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    // log the file path
    if(write(log_file, filePath, strlen(filePath)) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    if(write(log_file, "\t", 1) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    // log the host name
    if(write(log_file, hostName, strlen(hostName)) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    // log the content-length
    char* c_len = (char*)malloc(sizeof(char)*30);
    sprintf(c_len, "\t%ld\n", content_length);
    if(write(log_file, c_len, strlen(c_len)) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
      free(c_len);
    }
    free(c_len);
  } else {
    // log the fail
    if(write(log_file, "FAIL\t", 5) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    // log the request type
    if(write(log_file, requestType, strlen(requestType)) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    if(write(log_file, " /", 2) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    // log the file path
    if(write(log_file, filePath, strlen(filePath)) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    // log the HTTP version and err code 400
    if(write(log_file, " ", 1) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    // log the HTTP version and err code 400
    if(write(log_file, httpVersion, strlen(httpVersion)) < 0){
      errx(EXIT_FAILURE, "Cannot log to the file");
    }
    if(err_code==400){
      // log the err code 400
      if(write(log_file, "\t400\n", 5) < 0){
        errx(EXIT_FAILURE, "Cannot log to the file");
      }
    } else if(err_code==501){
      // log the err code 501
      if(write(log_file, "\t501\n", 5) < 0){
        errx(EXIT_FAILURE, "Cannot log to the file");
      }
    } else if(err_code==404){
      // log the err code 404
      if(write(log_file, "\t404\n", 5) < 0){
        errx(EXIT_FAILURE, "Cannot log to the file");
      }
    } else if(err_code==403){
      // log the err code 403
      if(write(log_file, "\t403\n", 5) < 0){
        errx(EXIT_FAILURE, "Cannot log to the file");
      }
    } else if(err_code==500){
      // log the err code 500
      if(write(log_file, "\t500\n", 5) < 0){
        errx(EXIT_FAILURE, "Cannot log to the file");
      }
    }
  }
  // finish logging and return the file descriptor to the queue
  pthread_mutex_lock(&mutex_log);
  putFileBackQueue(q, log_file);
  pthread_cond_signal(&condition_log);
  pthread_mutex_unlock(&mutex_log);
}

/**
   Helper function for the get request.
   This function get the file path and the file descriptor of the request and
   return the content of the file to the client
 */
void getRequest(int request, char* filePath, char* httpVersion, char* hostName, struct Queue* q) {

  int readFile; // file descriptor for the file needed to read
  int log_file; // file descriptor for the log file

  // check if the file exist or not
  if(access(filePath, F_OK) < 0){
    // if no such file
    send(request, "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n", 56, 0);

    // log and exit
    log_file = getLogFile(q);
    // log
    log_to_file(0, log_file, 404, 0, "GET", filePath, httpVersion, hostName, q);
    return;
  }

  // check the permission of the file
  if(access(filePath, R_OK) < 0){
    // if do not have permission to read the file
    send(request, "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n", 56, 0);

    // log and exit
    log_file = getLogFile(q);
    // log
    log_to_file(0, log_file, 403, 0, "GET", filePath, httpVersion, hostName, q);
    return;
  }

  // if file exists and has permission to read it
  readFile = open(filePath, O_RDONLY); // open the file and set it to read only
  // check if problem occurs when opening file
  if(readFile < 0){
    // if having problems opening the file
    send(request, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n", 80, 0);

    // log and exit
    log_file = getLogFile(q);
    // log
    log_to_file(0, log_file, 500, 0, "GET", filePath, httpVersion, hostName, q);
    return;
  }

  // nothing goes wrong
  char* httpResponse = (char*)malloc(100); // for http version and content-length
  char* fileContent = (char*)malloc(10000);
  // check the file size using stat
  struct stat buf;
  stat(filePath, &buf);
  long int fileSize = buf.st_size;

  sprintf(httpResponse, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", fileSize);
  int responseSize = strlen(httpResponse);

  int readSize;
  if(fileSize <= 10000){
    // if the content of the file is smaller than the buffer, then read it in 1 go
    readSize = read(readFile, fileContent, 10000);
    if( readSize < 0){
      // if something wrong with read
      send(request, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n", 80, 0);

      // log and exit
      log_file = getLogFile(q);
      // log
      log_to_file(0, log_file, 500, 0, "GET", filePath, httpVersion, hostName, q);
      free(httpResponse);
      free(fileContent);
      return;
    } else {
      send(request, httpResponse, responseSize, 0);
      send(request, fileContent, readSize, 0);
    }
  } else {
    // if contetn of file is bigger than the buffer, send it one at a time
    readSize = read(readFile, fileContent, 10000);
    if( readSize < 0){
      // if something wrong with read
      send(request, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n", 80, 0);

      // log and exit
      log_file = getLogFile(q);
      // log
      log_to_file(0, log_file, 500, 0, "GET", filePath, httpVersion, hostName, q);
      free(httpResponse);
      free(fileContent);
      return;
    } else {
      send(request, httpResponse, responseSize, 0);
      send(request, fileContent, readSize, 0);
      // do a while loop and keep reading and sending
      while((readSize = read(readFile, fileContent, 10000)) > 0){
        send(request, fileContent, readSize, 0);
      }
    }
  }
  // log and exit
  log_file = getLogFile(q);
  // log
  log_to_file(1, log_file, 0, fileSize, "GET", filePath, httpVersion, hostName, q);

  // free the memory
  free(httpResponse);
  free(fileContent);

  // close file
  close(readFile);
  
  return;
}

/**
   helper function for the put request.
   This function get the file path, the file descriptor of the request, buffer that stores
   the information of the request, the length of content needed to read, the current size of
   the buffer, and the capacity of the buffer. And at the end it will return the content of 
   the file to the client
 */
void putRequest(int request, char* filePath, char* httpVersion, char* hostName, char* requestBuffer, int content_length, int* requestBuffer_Byte_size, int requestBuffer_size, struct Queue* q) {
  // get the size of the filePath
  int file; // file descriptor for opening file to write
  int exists = 0; // checker/reminder for file existence
  int recv_size = -1; // file descriptor
  int log_file; // file descriptor
  long int c_len = content_length; // for logging

  // check if the file exists
  if(access(filePath, F_OK) < 0){
    // if not exists
    exists = 0;
  } else {
    // if file exists
    // check if we have permission to access/write the file
    if(access(filePath, W_OK) < 0){
      // if no write permission
      send(request, "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n", 56, 0);

      // log and exit
      log_file = getLogFile(q);
      // log
      log_to_file(0, log_file, 403, 0, "PUT", filePath, httpVersion, hostName, q);
      return;
    }
    exists = 1;
  }

  // create the file when file not exist
  file = open(filePath, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
  // if something wrong when creating file
  if(file < 0){
    send(request, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n", 80, 0);

    // log and exit
    log_file = getLogFile(q);
    // log
    log_to_file(0, log_file, 500, 0, "PUT", filePath, httpVersion, hostName, q);
    return;
  }

  // if nothing goes wrong with filepath, then start writing the body to the file
  // if the size is bigger or equal to content_length, then we only take the size of content we need
  if(*requestBuffer_Byte_size >= content_length){

    // write to the file directly
    if(write(file, requestBuffer, content_length) < 0){
      // something goes wrong on the server side
      send(request, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n", 80, 0);

      // log and exit
      log_file = getLogFile(q);
      // log
      log_to_file(0, log_file, 500, 0, "PUT", filePath, httpVersion, hostName, q);
      close(file);
      return;
    }

    // if write success
    // remove the body that we took
    memmove(requestBuffer, requestBuffer+content_length, *requestBuffer_Byte_size);
    // update current buffer size
    *requestBuffer_Byte_size = *requestBuffer_Byte_size - content_length;
    content_length = 0;
  } else {
    // if the size is smaller than content_length, then write to the file first and empty the request Buffer
    // check if the size is 0
    if(*requestBuffer_Byte_size>0){
      // write to the file directly
      if(write(file, requestBuffer, *requestBuffer_Byte_size) < 0){
        // something goes wrong on the server side
        send(request, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n", 80, 0);

        // log and exit
        log_file = getLogFile(q);
        // log
        log_to_file(0, log_file, 500, 0, "PUT", filePath, httpVersion, hostName, q);
        close(file);
        return;
      }
    }

    // if write success, continue reading from request
    content_length = content_length - *requestBuffer_Byte_size;
    //empty the buffer
    memset(requestBuffer, 0, requestBuffer_size);

    // update the current size
    *requestBuffer_Byte_size = 0;

    // at this point requestBuffer is empty so we can use it to pass the content
    while((recv_size = recv(request, requestBuffer, requestBuffer_size, 0)) >= 0){

      // update the current size
      *requestBuffer_Byte_size = *requestBuffer_Byte_size + recv_size;

      if(recv_size==0){
        // if the client close the connection before reading enough

      }

      // decide the size to write into
      if(*requestBuffer_Byte_size >= content_length){
        // if the size received is larger than needed, then only write the size needed
        if(write(file, requestBuffer, content_length) < 0){
          // something goes wrong on the server side
          send(request, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n", 80, 0);

          // log and exit
          log_file = getLogFile(q);
          // log
          log_to_file(0, log_file, 500, 0, "PUT", filePath, httpVersion, hostName, q);
          close(file);
          return;
        }
        // remove the content from the buffer
        memmove(requestBuffer, requestBuffer + content_length, *requestBuffer_Byte_size);
        // update size
        *requestBuffer_Byte_size = *requestBuffer_Byte_size - content_length;
        content_length = 0;
        // finish writing

        break;
      }
      // if buffer size is still smaller or equal to content-length then write size of recv_size
      if(write(file, requestBuffer, *requestBuffer_Byte_size) < 0){
        // something goes wrong on the server side
        send(request, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server Error\n", 80, 0);

        // log and exit
        log_file = getLogFile(q);
        // log
        log_to_file(0, log_file, 500, 0, "PUT", filePath, httpVersion, hostName, q);
        close(file);
        return;
      }

      //empty the buffer
      memset(requestBuffer, 0, requestBuffer_size);
      // update the content-length
      content_length = content_length - *requestBuffer_Byte_size;
      // update the current size
      *requestBuffer_Byte_size = 0;
    }
  }
  // finish writing to file

  // send back the response
  if(exists){
    send(request, "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n", 41, 0);
  } else {
    send(request, "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n", 51, 0);
  }

  // log and exit
  log_file = getLogFile(q);
  // log
  log_to_file(1, log_file, 0, c_len, "PUT", filePath, httpVersion, hostName, q);

  // close file
  close(file);

  return;
}


/**
   helper function for the HEAD request.
   This function get the file path and the file descriptor of the request and
   return the content length of the file to the client
 */
void headRequest(int request, char* filePath, char* httpVersion, char* hostName, struct Queue* q){
  int readFile; // file descriptor for the file needed to read
  int log_file; // file descripttor for log_file

  // error check...

  // check if the file exist or not
  if(access(filePath, F_OK) < 0){
    // if no such file
    send(request, "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\n", 46, 0);

    // log and exit
    log_file = getLogFile(q);
    // log
    log_to_file(0, log_file, 404, 0, "HEAD", filePath, httpVersion, hostName, q);
    return;
  }

  // check the permission of the file
  if(access(filePath, R_OK) < 0){
    // if do not have permission to read the file
    send(request, "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\n", 46, 0);

    // log and exit
    log_file = getLogFile(q);
    // log
    log_to_file(0, log_file, 403, 0, "HEAD", filePath, httpVersion, hostName, q);
    return;
  }

  // if file exists and has permission to read it
  readFile = open(filePath, O_RDONLY); // open the file and set it to read only
  // check if problem occurs when opening file
  if(readFile < 0){
    // if having problems opening the file
    send(request, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\n", 58, 0);
    // log and exit
    log_file = getLogFile(q);

    // log
    log_to_file(0, log_file, 500, 0, "HEAD", filePath, httpVersion, hostName, q);
    return;
  }

  // nothing goes wrong
  char* httpResponse = (char*)malloc(100); // for http version and content-length
  // check the file size using stat
  struct stat buf;
  stat(filePath, &buf);
  long int fileSize = buf.st_size;


  sprintf(httpResponse, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", fileSize);
  int responseSize = strlen(httpResponse);
  send(request, httpResponse, responseSize, 0);

  // log and exit
  log_file = getLogFile(q);
  // log
  log_to_file(1, log_file, 0, fileSize, "HEAD", filePath, httpVersion, hostName, q);

  // free all the buffers
  free(httpResponse);
  
  // close file
  close(readFile);

  return;
}

/**
   Converts a string to an 16 bits unsigned integer.
   Returns 0 if the string is malformed or out of the range.
 */
uint16_t strtouint16(char number[]) {
  char *last;
  long num = strtol(number, &last, 10);
  if (num <= 0 || num > UINT16_MAX || *last != '\0') {
    return 0;
  }
  return num;
}

/**
   Creates a socket for listening for connections.
   Closes the program and prints an error message on error.
 */
int create_listen_socket(uint16_t port) {
  struct sockaddr_in addr;
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    err(EXIT_FAILURE, "socket error");
  }

  memset(&addr, 0, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htons(INADDR_ANY);
  addr.sin_port = htons(port);
  if (bind(listenfd, (struct sockaddr*)&addr, sizeof addr) < 0) {
    err(EXIT_FAILURE, "bind error");
  }

  if (listen(listenfd, 500) < 0) {
    err(EXIT_FAILURE, "listen error");
  }

  return listenfd;
}
// function used to handle the requests within the connection
void handle_connection(int connfd, struct Queue* q) {

  char* requestBuffer = (char*)malloc(sizeof(char)*10000); // buffer for storing the first part of the header
  char* requestPasser = (char*)malloc(sizeof(char)*50);
  int requestBuffer_size = 10000; // keep track of the capacity of the requestBuffer
  int requestBuffer_Byte_size = 0; // keep track of the byte size of the requestBuffer 
  int recv_size; // file descriptor for recv
  int stopConnection = 0; // flag/checker for stopping the connection
  int log_file; // log file descriptor

  memset(requestBuffer, 0, 10000);
  memset(requestPasser, 0, 50);

  // loop until connection breaks
  while(1){
    int badRequest = 0; // flag/checker for bad request
    int content_length = -1; // flag/checker for content_length
    int hasHost = 0; // flag/checker for host

    // read until the client closes the connection to get the first line of header, for new request
    while((recv_size = recv(connfd, requestPasser, 50, 0)) >= 0){
      char* requestType = (char*)malloc(sizeof(char)*20); // buffer for storing the request type
      char* filePath = (char*)malloc(sizeof(char)*20); // buffer for storing the file path
      char* hostName = (char*)malloc(sizeof(char)*30); // buffer for storing host name
      char* httpVersion = (char*)malloc(sizeof(char)*20); // buffer for storing http version

      memset(requestType, 0, 20);
      memset(filePath, 0, 20);
      memset(hostName, 0, 30);
      memset(httpVersion, 0, 20);

      //stop the connection when recv_size is 0 and the data in request buffer is not right
      if(recv_size==0 && requestBuffer_Byte_size==0){
        printf("time for a break\n");
        stopConnection = 1;
        break;
      }
      // check to see if more memory is needed to store the new request data
      int newSize = requestBuffer_Byte_size + recv_size;
      if(newSize > requestBuffer_size){
        // if the length is larger than the original buffer, then expand the size
        newSize = newSize*2;
        requestBuffer = (char*)realloc(requestBuffer, sizeof(char)*newSize);
        // update the buffer size
        requestBuffer_size = newSize;
      }
      // get the new content to the buffer
      memmove(requestBuffer+requestBuffer_Byte_size, requestPasser, recv_size);
      // update the current size of the buffer
      requestBuffer_Byte_size = requestBuffer_Byte_size + recv_size;

      // check if there is a new line slash_r_slash_n in the request buffer
      char* newLine; // pointer to the newline at the end of a request
      newLine = strstr(requestBuffer, "\r\n\r\n");
      if(newLine){
        // if there is a newLine in this buffer, get the content before the newLine and leave the rest in the buffer for later use
        
        // get the position of the newLine
        int pos = newLine - requestBuffer; // get the position using pointer
        
        // get the substring into a buffer
        char* requestFirstLine = (char*)malloc(sizeof(char)*pos+10);
        memset(requestFirstLine, 0, pos+10);
        memmove(requestFirstLine, requestBuffer, pos + 4);


        // tokenizing the request and gather info
        char* end_str;
        char* token = strtok_r(requestFirstLine, "\r\n", &end_str);
        int outerCounter = 0; // tracker for reading and tokenizing
        int innerCounter = 0; // tracker for reading and tokenizing
        while(token != NULL){
          char* end_token;
          char* token2 = strtok_r(token, " ", &end_token);
          while(token2!=NULL){
            // while getting the first part of the request
            if(outerCounter==0){
              // first get the request type
              if(innerCounter==0){
                // if first token is larger then 4, then probably not implemented or error
                if(strlen(token2)>4){
                  badRequest = 1;
                }
                // memmove(requestType, token2, strlen(token2));
                strcpy(requestType, token2);
              }
              // then get the file path
              if(innerCounter==1){
                if(strlen(token2)!=16){
                  // if size not match, then it is a bad request
                  badRequest = 1;
                }
                strcpy(filePath, token2);
                printf("%s\n", filePath);
              }
              // then see the HTTP version
              if(innerCounter==2){
                // if the version is not http 1.1 then it is a bad request
                if(strcmp(token2, "HTTP/1.1")){
                  badRequest = 1;
                } else {
                  strcpy(httpVersion, token2);
                }
              }
            }

            // while getting second part of the request
            if(outerCounter>0){
              // if it is content_length then proceed and get the length and save it
              if(!strcmp(token2, "Content-Length:")){
                token2 = strtok_r(NULL, " ", &end_token);
                if(token2!=NULL){
                  sscanf(token2, "%d", &content_length);
                } else{
                  // if nothing after content-Length
                  badRequest = 1;
                }
              } else if(!strcmp(token2, "Host:")){
                // if it is Host then proceed and get the host name and save it
                token2 = strtok_r(NULL, " ", &end_token);
                if(token2!=NULL){
                  // copy the host name to a memory
                  strcpy(hostName, token2);
                  hasHost = 1;
                } else{
                  // if nothing after Host
                  badRequest = 1;
                }
              }
            }

            innerCounter++;
            token2 = strtok_r(NULL, " ", &end_token);
          }
          
          outerCounter++;
          token = strtok_r(NULL, "\r\n", &end_str);
        }

        // remove the substring from the request buffer
        pos = pos + 4; // take in count for the 4 characters for the new line r and n at the end of the request
        memmove(requestBuffer, requestBuffer+pos, requestBuffer_Byte_size);
        // update the current size of the buffer
        requestBuffer_Byte_size = requestBuffer_Byte_size - pos;

        free(requestFirstLine);

        // analyze the information and do the request

        // check the filePath requirement if it stores something
        if(strlen(filePath) > 0){
          // check if there is a slash at the beginning
          if(filePath[0]!='/'){
            badRequest = 1;
          }
          
          // get rid of the dash
          memmove(filePath, filePath+1, strlen(filePath));
          int filePath_size = strlen(filePath);
          for(int i = 0; i < filePath_size; i++){
            // check if each letter meet the requirement
            if(!isalnum(filePath[i])){
              badRequest = 1;
            }
          }
        }

        // check if the host exists or not
        if(!hasHost){
          badRequest = 1;
        }

        // check the bad request flag
        if(badRequest){
          // if something goes wrong, send back 400 msg and set the break flag to true to stop the connection
          if(!strcmp(requestType, "HEAD")){
            // if it is a head request, then no body in the 400
            send(connfd, "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\n", 48, 0);
          } else {
            // anything else should have body in the 400 response 
            send(connfd, "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n", 60, 0);
          }
          // log and exit
          log_file = getLogFile(q);
          // log the 400 error
          log_to_file(0, log_file, 400, 0, requestType, filePath, httpVersion, hostName, q);

          stopConnection = 1;
          free(requestType);
          free(filePath);
          free(hostName);
          free(httpVersion);
          break;
        }

        // if nothing goes wrong then starts processing the request based on the request type

        if(!badRequest&&(!strcmp(requestType, "GET"))){
          // if it is not a bad request and it is a get request
          getRequest(connfd, filePath, httpVersion, hostName, q);
        } else if(!badRequest&&(!strcmp(requestType, "HEAD"))){
          // if it is not a bad request and it is a head request
          headRequest(connfd, filePath, httpVersion, hostName, q);
        } else if(!badRequest&&(!strcmp(requestType, "PUT"))) {
          putRequest(connfd, filePath, httpVersion, hostName, requestBuffer, content_length, &requestBuffer_Byte_size, requestBuffer_size, q);
        } else if(!badRequest){
          // if it is not get, head, or put request, then it is a 501
          send(connfd, "HTTP/1.1 501 Not Implemented\r\nContent-Length: 16\r\n\r\n", 52, 0);

          // log and exit
          log_file = getLogFile(q);
          // log the 501 error
          log_to_file(0, log_file, 501, 0, requestType, filePath, httpVersion, hostName, q);
        }
        free(requestType);
        free(filePath);
        free(hostName);
        free(httpVersion);
      } else {
        if(recv_size==0){
          // if no new line and no more recv from client
          stopConnection = 1;
          free(requestType);
          free(filePath);
          free(hostName);
          free(httpVersion);
          break;
        }
      }
    }

    // check if needed to stop connection
    if(stopConnection){
      break;
    }
  }

  free(requestBuffer);
  free(requestPasser);

  // when done, close socket
  close(connfd);
}


/*
  function to run when worker threads are dispatched
  In here the worker threads are constantly looking
  for connection to be process from the queue. And if
  queue is empty, it will wait until signaled that there
  is a new connection available
*/
void* thread_func(void* arg){
  // get the queue from the arg
  struct Queue* q = (struct Queue*)arg;

  // make the thread runs in a loop to constantly work
  while(1){
    int connection;
    pthread_mutex_lock(&mutex_queue);
    if((connection = deQueue(q)) == -1){
      pthread_cond_wait(&condition_queue, &mutex_queue);
      // wait until signaled that queue is not empty
      connection = deQueue(q);
    }
    pthread_mutex_unlock(&mutex_queue);

    // handle the connection when there is something coming out of queue
    handle_connection(connection, q);
  }
}

int main(int argc, char *argv[]) {
  int optInput;
  int listenfd;
  int num_threads = 4;
  int log_file = -1;
  int need_to_log = 0;
  uint16_t port = 0;

  while((optInput = getopt(argc, argv, "N:l:")) != -1){
    switch(optInput){
      case 'N':
        // get the number of threads arg
        sscanf(optarg, "%d", &num_threads);
        break;
      case 'l':
        // get the log file descriptor
        printf("file to write to is %s\n", optarg);
        log_file = open(optarg, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
        need_to_log = 1;
        break;
      case '?':
        if(optopt == 'N' || optopt == 'l'){
          // if nothing after the flags, then err
          errx(EXIT_FAILURE, "Need arguments for option %c", optopt);
        } else {
          // if unknown flags happens
          errx(EXIT_FAILURE, "Unknown option %c", optopt);
        }
        break;
      default:
        // what happens in default
        errx(EXIT_FAILURE, "Usage: %s [port number] -N num_threads -l logfile", argv[0]);
    }
  }
  printf("log is needed: %d\n", need_to_log);
  // check the port number
  printf("%d\n", optind);
  if(argv[optind]==NULL){
    printf("null\n");
    errx(EXIT_FAILURE, "Port number needed");
  }
  port = strtouint16(argv[optind]);
  printf("port: %d\n", port);
  if (port == 0) {
    errx(EXIT_FAILURE, "invalid port number: %s", argv[optind]);
  }
  listenfd = create_listen_socket(port);
  // check the log file whether we need to log or not
  if(need_to_log){
    // if need to log, then check if we can open the file to write
    if(log_file < 0){
      // if any error happens when trying to open to file to log
      errx(EXIT_FAILURE, "Cannot open the log file");
    }
  }

  pthread_t thread_list[num_threads]; // thread pool
  // initialize the mutex and condition variable for the queue data structure
  pthread_mutex_init(&mutex_queue, NULL);
  pthread_cond_init(&condition_queue, NULL);
  pthread_mutex_init(&mutex_log, NULL);
  pthread_cond_init(&condition_log, NULL);

  // initialize the queue to take in connections
  struct Queue* q = newQueue(log_file, need_to_log);

  // initialize the threads
  for(int i = 0; i < num_threads; i++){
    pthread_create(&thread_list[i], NULL, thread_func, q);
  }

  while(1) {
    int connfd = accept(listenfd, NULL, NULL);
    if (connfd < 0) {
      warn("accept error");
      continue;
    }
    // if the connection is ok then add that into the queue
    pthread_mutex_lock(&mutex_queue);
    enQueue(q, connfd);
    pthread_cond_signal(&condition_queue);
    pthread_mutex_unlock(&mutex_queue);
  }
  
  return EXIT_SUCCESS;
}
