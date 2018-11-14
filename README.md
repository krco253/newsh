# newsh
a shell written in C++

##Built in commands
cd <directory>
-change to  direcroty

quit
-exit the shell

bp 
-list background processes

set <variable> <value>
-set variable to value

<path to command>
-can execute non-built in commands from a file
given a path

##To run 
use the included makefile to run make. 

output1 is the result of 
./newsh < test1 >output1

output2 is the result of 
./newsh < test2 > output2

file1, file2, and file3 are included for the purpose
of testing /bin/cat

##Acknowledgements
-I used the csapp.h given and the shellex.c as a reference

