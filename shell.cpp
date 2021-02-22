/*
    shell.cpp (OS Lab #1)

    A simple shell program in C++

    Copyright (c) Saikishore Gowrishankar 2021. All rights reserved.
    All owned trademarks belong to their respective owners. Lawyers love tautologies.

*/

// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed:__________Saikishore Gowrishankar____________ Date:___02/21/21______

// 3460:426 Lab 1 - Basic C shell rev. 9/10/2020

//Standard includes
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <utility>

//C POSIX API
#include <sys/wait.h>
#include <unistd.h>

//For delimiter comparison
#define FIND_DELIM(ch) \
    ch == ' ' || ch == ',' || ch == '\t' || ch == '\n'

namespace
{
    using lookup_table_t = std::unordered_map<std::string, std::pair<std::string, bool>>;
    bool terminate = false; //Set if exec() fails and requires child termination.

    //This structure holds the data for a command
    struct Command
    {
        std::string name{};
        int argc{};
        std::vector<std::string> argv;
        bool bg_flag = false;

        //Reset command's state
        void reset()
        {
            argv.clear();
            argc = 0;
            name = "";
            bg_flag = false;
        }

        //Lookup table contains mappings of command to filename, and the boolean 
        //is set/unset if the program is to run in the background/foreground, respectively
        static inline const lookup_table_t lookup_table =
        {
            {"C",{"cp", false}},
            {"D",{"rm",false}},
            {"E",{"echo",false}},
            {"H",{"help",false}},
            {"L",{"ls",false}},
            {"M",{"nano",false}},
            {"P",{"more",false}},
            {"W",{"clear",false}},
            {"S",{"firefox",true}},
            {"Q",{"exit",false}},
            {"W",{"clear",false}}
        };

    };

    //Instance of a shell.
    class Shell
    {
    private:
        std::string buf{}; //Holds raw character sequence
        Command cur;	   //Parsed current command
        Shell() = default; //Private constructor (singleton)

        //Displays "man pages" for the shell
        static void help()
        {
            std::cout << "\tList of commands: \n"
                      << "C file1 file2        Copy; create file2, copy all bytes of file1 to file2 without deleting file1.\n"
                      << "D file               Delete the named file.\n"
                      << "E comment            Echo; display comment on screen followed by a new line\n"
                      << "H Help;              display the user manual.\n"
                      << "L                    List the contents of the current directory.\n"
                      << "M file               Make; create the named text file by launching a text editor.\n"
                      << "P file               Print; display the contents of the named file on screen.\n"
                      << "Q                    Quit the shell.\n"
                      << "S                    Surf the web by launching a browser as a background process.\n"
                      << "W                    Wipe; clear the screen.\n";
        }

        //Receive user command and display prompt.
        void fill_buffer()
        {
            std::cout << "\u001b[36mlinux (sg264)|> \u001b[0m";
            std::getline(std::cin, buf);
        }

        //Parse user command
        void parse()
        {

            //Replace all delimiters with newline for simple parsing, then
            //insert in current command's argument buffer
            for(char& ch : buf) if(FIND_DELIM(ch)) ch = '\n';
            std::istringstream ss{buf};
            cur.argv.assign(std::istream_iterator<std::string>(ss), {});

            //Find entry mapping in lookup table, if exists
            auto iter = Command::lookup_table.find(cur.argv[0]);
            if(iter != std::cend(Command::lookup_table))
            {
                cur.name = (iter->second).first; 	//Mapped name
                cur.bg_flag = (iter->second).second;	//Background flag
            }
            else cur.name = cur.argv[0];	//Attempt execution if name not found in lookup table

            cur.argc = std::size(cur.argv)-1;
        }

        //Executes current command
        void execute()
        {
            if(cur.name == "exit") std::exit(0);
            else if(cur.name == "help") { help(); return; }

            //Parent
            if(auto pid = fork(); pid > 0)
            {
                if(!cur.bg_flag)
                {
                    if(waitpid(pid, nullptr, 0) < 0) throw "wait() failed to reap child";
                }
                else std::cout << "Running in BG\n";
                cur.reset();
            }

            //Child
            else if(pid == 0)
            {
                //Convert std::vector<std::string> to char* const*
                std::vector<const char*> args{};
                std::transform(std::begin(cur.argv), std::end(cur.argv), std::back_inserter(args),
                    [](std::string const& str){ return str.c_str();});

                //Execute requested function
                execvp(cur.name.c_str(), const_cast<char* const*>(args.data()));

                //exec() will terminate the child if it returns here.
                terminate = true;
                throw "exec() failed to initialize requested program";
            }
            else throw "fork() failed to create a child process";
        }
        void run_cycle() {fill_buffer(); parse(); execute();}

    public:
        //Singleton
        static Shell& singleton(){static Shell s; return s;}

        //User access method. Main loop begins here
        //Errors are reported as exceptions within the main loop of run_cycle(). 
        //Errors are typically handled by merely resetting the current parsed
        //command and continuing execution at the beginning of the loop, thereby
        //ensuring fault-tolerance. In fatal errors, the terminate flag may be set,
        //in which case the current process will terminate instead of continuing
        //execution. 
        void run()
        {
            while(true)
            {
                try { run_cycle(); }
                catch(const char* err)
                {
                    std::cerr << "\u001b[31mERROR: " << err << "\u001b[0m\n";
                    cur.reset();

                    if(terminate) std::exit(1);
                    else continue;
                }
            }
        }
    };

}

int main()
{
    Shell::singleton().run();
}