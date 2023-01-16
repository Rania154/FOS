/*	Simple command-line kernel prompt useful for
	controlling the kernel and exploring the system interactively.


KEY WORDS
==========
CONSTANTS:	WHITESPACE, NUM_OF_COMMANDS
VARIABLES:	Command, commands, name, description, function_to_execute, number_of_arguments, arguments, command_string, command_line, command_found
FUNCTIONS:	readline, cprintf, execute_command, run_command_prompt, command_kernel_info, command_help, strcmp, strsplit, start_of_kernel, start_of_uninitialized_data_section, end_of_kernel_code_section, end_of_kernel
=====================================================================================================================================================================================================
 */


#include <kern/cmd/command_prompt.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/kdebug.h>
#include <kern/cons/console.h>
#include "commands.h"


//invoke the command prompt
void run_command_prompt()
{
	char command_line[1024];

	while (1==1)
	{
		//get command line
		readline("FOS> ", command_line);

		//parse and execute the command
		if (command_line != NULL)
			if (execute_command(command_line) < 0)
				break;
	}
}

/***** Kernel command prompt command interpreter *****/

//define the white-space symbols
#define WHITESPACE "\t\r\n "

//Function to parse any command and execute it
//(simply by calling its corresponding function)
int getSize (char * s) {
    char * t; // first copy the pointer to not change the original
    int size = 0;

    for (t = s; *t != '\0'; t++) {
        size++;
    }

    return size;
}

int execute_command(char *command_string)
{
	// Split the command string into whitespace-separated arguments
	int number_of_arguments;
	//allocate array of char * of size MAX_ARGUMENTS = 16 found in string.h
	char *arguments[MAX_ARGUMENTS];


	strsplit(command_string, WHITESPACE, arguments, &number_of_arguments) ;
	if (number_of_arguments == 0)
		return 0;

	// Lookup in the commands array and execute the command
	int command_found = 0;
	int i ;
	for (i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if (strcmp(arguments[0], commands[i].name) == 0)
		{
			command_found = 1;
			break;
		}
	}

	if(command_found)
	{
		int return_value;
		return_value = commands[i].function_to_execute(number_of_arguments, arguments);
		return return_value;
	}
	else
	{
		char *firstChar = arguments[0];
		char *command;
		int i , j,flag,cnt =0;
		int found =0;
		int inputSize = getSize(firstChar), commandSize;
		//if not found, then it's unknown command
		for( i = 0; i < NUM_OF_COMMANDS;i++)
					{
						flag = 1;
						command = commands[i].name;
						commandSize = getSize(command);
						if (commandSize >= inputSize)
						{
							for(j =0;j<inputSize;j++)
							{
								if (command[j] != firstChar[j] )
								{
									flag = 0;
								}
							}
						}
						else
						{
							flag = 0;
						}
						if ( flag == 1)
						{
							cnt++;
							cprintf("%s \n",command);
						}

					}
		if (cnt  == 0)
		cprintf("Unknown command '%s'\n", arguments[0]);
		return 0;
	}
}
