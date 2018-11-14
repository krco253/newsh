/*
-----------------------------------------------
|                    newsh                    |
|             a shell written in C++          |
-----------------------------------------------
|
| Compile with: -std=c++11
| Author: Kelsey Cole
-----------------------------------------------
*/

#include <string>
#include <cstring>
#include <iostream> /*cout, etc*/
#include <sstream> /*stringstream*/
#include <limits.h> /*realpath*/
#include <stdlib.h> /*realpath*/
#include <vector>
#include <map>
#include <unistd.h> /*getcwd*/
#include <stdio.h> /*getcwd*/
#include "csapp.h" /*enviro,etc*/

using namespace std;

/* typedefs*/

typedef map<string,string> strMap;
typedef vector<string> strVec;
typedef pair<string,string> strPair;

/* globals */
strMap shell_variables; /*stores values of variables from user*/
string shell_prompt; /*the shell prompt*/
strVec bg_processes; /*vector of processes running in background*/

/*prototypes*/
strVec tokenizeCmd(string line);
void execute_command(strVec com);
void set_variable_value(string var, string val);
void cd(string directory_name);
int cmd(strVec command);
void bp(void);
/*-----------------------------------------
|               main function
-------------------------------------------
Gets input from user until user 
selects CTRL-D or types quit
*/
int main()
{
	string input;
	/*initialize shell prompt*/
	shell_prompt = "newsh$ ";
	cout << shell_prompt;
	/*get initial command from user*/
	getline(cin, input);
	strVec cmd_line; /*temporary vector of strings to store tokenized command*/
	
	
	while (!cin.eof() && input != "quit") 
	{
	/* tokenize command */
	cmd_line = tokenizeCmd(input);
	/*execute command if there is one to execute*/
	if (cmd_line.size() != 0)
		execute_command(cmd_line);
	/*begin prompt for next command*/
	cout << shell_prompt;
	/*command has executed, clear vector*/
	cmd_line.clear();
	/*get the next command*/
	getline(cin, input);	
	}
	
	return 0;
}

/*------------------------------------------------ 
|                  tokenizeCmd
------------------------------------------------- 
tokenizes command 
returns the tokenized command in a vector of strings 
*/
strVec tokenizeCmd(string line)
{
	string token, value;
	stringstream ss(line);
	bool isVariable = false; 
	strVec command;

	/* tokenize input line*/
	while(getline(ss,token, ' '))
	{
		/*check if input is a variable*/
		if ((token[0] == '$'))
		{
		
			if (shell_variables.size() != 0)
			{
				for(strMap::const_iterator it = shell_variables.begin(); it != shell_variables.end(); it++)
				{
					string tok = token.substr(1);	
					if(it->first == tok)
					{
						isVariable = true; 
						value = it->second;
						command.push_back(value);
						break;
					}

				}
			}
			if(!isVariable)
			{
				cout << "Variable not found." <<  endl;
			}	
			
		}
		/*if input does not begin with $*/
		else if (token == ""){ } /*ignore if there are spaces in front of the command*/
		else if ((token[0] == '%'))/*ignore the rest of the line if this is a comment*/
		{ break; }
		else
		{
			command.push_back(token);/*push the token to the strVec of the command*/

		}				
	} 		
	
	return command;
}

/*--------------------------------------
|           execute_command
---------------------------------------
Determines the appropriate function to call to 
execute command 
then calls that function
*/
void execute_command(strVec com)
{


	if (com[0] == "set") 
	{
		if (com.size() <= 2) 
		{
			cout << "Please provide an identifer and a value." << endl;
			return;
		}
		set_variable_value(com[1],com[2]);
	}
	else if (com[0] == "cd")
	{
		if (com.size() <= 1)
		{ 
			cout << "Please provide directory name." << endl; 
			return;
		}
		cd(com[1]);
	}
	else if (com[0] == "bp")
	{
		bp();
	}
	else /*else it is not a built in command - call cmd*/
	{
		cmd(com);
	}
	
}

/*-----------------------------------------
|          set_variable_value
-------------------------------------------
take variable and desired value and store in 
global map shell_variables
if variable is prompt, change value of 
global shell_prompt appropriately
*/
void set_variable_value(string var, string val)
{
	strPair var_val(var,val);
	if(shell_variables.count(var) > 0)
	{
		shell_variables.erase(var); /*if the variable already exists, delete it*/
	}
	shell_variables.insert(var_val);
	
	/* if the variable was PROMPT, change shell prompt*/
	if (var == "PROMPT")
	{
		shell_prompt = val + ' ';
	}

}

/*-----------------------------------------
|                   cd 
-------------------------------------------
change current directory 
if directory doesn't exist, print error message
*/
void cd(string directory_name)
{
	char *dir = new char[directory_name.length() +1];
	strcpy(dir, directory_name.c_str());
	char actualpath [PATH_MAX+1];
	char *ptrpath;
	
	/*if the path isn't absolute*/	
	if(dir[0] != '/')
	{
		ptrpath = realpath(dir, actualpath); /*get the absolute path*/
		if (ptrpath == NULL)
		{
			cout << "Path error" << endl;
			return;
		}
	}	
	int success = chdir(dir); /*change directory*/

	if (success == -1)
	{
		cout << "Directory not found." << endl;
	}
	else
	{
		cout << "Successfully changed directory!" << endl;
	}

	delete[] dir;
}

/*------------------------------------------
|                     cmd
--------------------------------------------      
user submits a command to execute a program
if command is executed successfully, cmd  returns 0
else, cmd returns -1
*/
int cmd(strVec command)
{
	pid_t pid; /*process ID*/
	int end = command.size()-1;
	bool bg = false; /*background process?*/
	int com_found = 0;
	
	/*is this a background process?*/
	if (command[end] == "&")
	{
		bg = true;
		bg_processes.push_back(command[0]); /*add to list of background processes running*/
		command.pop_back(); /*remove & as a token*/
	}

		
	/*convert to char array*/
	char **c_command = new char*[command.size()+1];
	for(size_t i =0; i < command.size(); i++)
	{
		c_command[i] = new char[command[i].size()+1]; 
		strcpy(c_command[i],command[i].c_str());
	}
	c_command[command.size()] = NULL; /*add null pointer to end of array for use by execvp*/

	/*child runs process*/
	pid = fork();
	if (pid == 0)
	{
		if(execvp(c_command[0], c_command) <0)			
		{
			cout << "Command not known" << endl;
			if(bg) {bg_processes.pop_back();}
			return -1;
		}	
	}
	
	/*Parent waits for foreground job to terminate*/
	if(!bg) 
	{
		int status;	
		if(waitpid(pid,&status,0) < 0)
			cout  << "waitpid error" << endl;
	}
	else
		cout << pid << " " << command[0] <<endl; 

	
	/*free memory*/
	for(size_t i = 0; i < command.size(); i++)
	{
		delete [] c_command[i];
	}
	

	return 0;
}

/*------------------------------------------
|                     bp
--------------------------------------------      
displays background processes
*/
void bp(void)
{
	cout << "----------------- bp -------------------" << endl;
	if (bg_processes.size() == 0)
	{
		cout << "No background processes running." << endl;
		return;
	}
	for (int i = 0; i < bg_processes.size(); i++)
	{
		cout <<"Number in queue: " << i << " Name: " << bg_processes[0] << endl;
	}	
}

