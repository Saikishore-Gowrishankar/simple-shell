# Simple Shell

This is a simple shell program written in C++. To compile, simply use the
provided makefile by typing "make". To run the program, type "./a.out".

To compile manually, use the following commands:

g++ shell.cpp -std=c++17
./a.out

## Commands/How to Use

The shell supports a number of custom commands, which are listed below:

C file1 file2			Copy, create file2, copy file2 to file2
D file				Delete the named file
E comment			Print the comment to the screen
H				H; Display user manual
L				List current directory contents (ls -l)
M file				Make, create the named text file
P file				Print, display file to stdout
Q				Exit the shell
S				Open Firefox in the background
W				Wipe, clear the screen

Besides this, it supports other typical executable UNIX commands such as
'ls'. You can also run other installed programs in this shell.

To use, simply type the desired command in the prompt.

## Functions

The Shell class deals with the main program operation of the shell. There
should only be one instance of it in a program, thus it's constructor is
private and user access to a singleton object is provided by a static member
function of the class. The Command class stores the data for a particular
command, including its executable name, arguments, and other information.
Shell holds a Command instance that stores the current command's state. This
is reset on every cycle of Shell::run_cycle() using the Command::reset()
method.

The necessary operations are perfomed in the run_cycle() method, which calls
three different methods: fill_buffer() (which refills the buffer with the
user's raw input), parse() (which performs processing on the raw buffer and
parses the individual components of it into an internal Command object,
which stores the current command's state), and execute() (which performs the
desired operation by calling exec() and loading the requested command and
its arguments. 

This is performed in a loop in the public accessor function run(), which is
encapsulated by a try/catch block as errors are thrown as exceptions from
within one of these functions, which automatically performs needed stack
unwinding and gracefully continues execution of the shell for nonfatal
errors (fatal errors set the "terminate" flag, which will kill the process).

The mapping between the custom commands and their executable commands are
stored in a hash map-based lookup table, in which a custom command is mapped
to a tuple storing its true executable filename, along with additional
parameters such as initial arguments, message, and a background execution
flag.

## Citations
1. Linux man-pages: https://www.kernel.org/doc/man-pages/
2. Effective Modern C++, Scott Meyers
3. cppreference: en.cppreference.com 
4. GNU Make: https://www.gnu.org/software/make/manual/
5. GCC online documentation: https://gcc.gnu.org/onlinedocs/
