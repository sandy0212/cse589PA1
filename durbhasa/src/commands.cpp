#include "../include/global.h"
#include "../include/logger.h"

bool checkAnyLowerCase(std::string command) {
	cse4589_print_and_log("Checking for any lower case characters in command: %s\n", command.c_str());
	for(int i=0;i<command.size();i++) {
		if(command[i] == ' ') break;
		else if(command[i]<'A' || command[i] >'Z') {
			cse4589_print_and_log("Found the lowercase letter: %c\n", command[i]);
			return true;
		}
	}
	return false;
}