##
## Shopping line simulator

### Introduction
In this project I am building a program that will simulate the shopping checkout line where customers with different amount of items come at different time and try to finish checkout as soon as possible, the goal is to allocate customers to the most efficient shopping line.

### Technologies I used
* C for the language
* Dynamic Queue for the Data Structure
* I/O Stream for managing input information of customers and output on files

### Basic Instruction
##### Open a terminal and go to the directory where you place all the files for this project
##### You can modify the numbers in the input file to test out different result
##### The format of the input will be as follow:
##### Number_of_customers
##### arrival_time_1 duration_time_1(latency)
##### arrival_time_2 duration_time_2
##### arrival_time_3 duration_time_3
##### ...
##### 
##### Then enter the following in terminal
```bash
make
./Simulation input output
```
##### After that, 2 output files will be made showcasing the result.
