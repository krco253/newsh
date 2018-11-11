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
#include <vector>
#include <map>
#include <unistd.h> /*getcwd*/
#include <stdio.h> /*getcwd*/

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
void cmd(strVec command);

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
						cout << value << endl;
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
		else if ((token[0] == '%'))
		{ break; }
		else
		{
			command.push_back(token);

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

	int command_found = 0;

	if (com[0] == "set") 
	{
		set_variable_value(com[1],com[2]);
	}
	else if (com[0] == "cd")
	{
		cd(com[1]);
	}
	else /*else it is not a built in command - call cmd*/
	{
		command_found = cmd(com);
	}
	if (command_found != 0)
	{
		cout << "Command not found." << endl;
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
		shell_variables.erase(var);
	}
	shell_variables.insert(var_val);
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
	int success = chdir(dir);
	if (success == -1)
	{
		cout << "Directory not found." << endl;
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
	int end = command.size()-1;
	/*if this is a background process*/
	if (command[end] == &)
	{

	}
	else /*if this is not a background process*/
	{

	}

}