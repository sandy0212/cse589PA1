/**
 * @durbhasa_assignment1
 * @author  Sandeep Durbha <durbhasa@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>

#include "../include/global.h"
#include "../include/logger.h"
#include "commands.h"

using namespace std;


struct client_info {
	int list_id;
	char* hostname;
	char ip_addr[16];
	int port_num;

	//own ?
    int sock_fd;
    bool loggedIn;
};

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */


std::string extractCommand(std::string fullCommand) {
	// char *command1 = (char *) malloc(sizeof(char)*20);
	// command1 = strtok(fullCommand, " ");
	// char *command2 = (char *) malloc(sizeof(char)*20);
	// command2 = strtok(fullCommand, "\n");

	// if(command1==NULL) {
	// 	cse4589_print_and_log("%s\n", command2);
	// 	return command2;
	// } else {
	// 	cse4589_print_and_log("%s\n", command1);
	// 	return command1;
	// }

	string ans = "";

	for(int i=0;i<fullCommand.size();i++) {
		if(fullCommand[i] == ' ') return ans;
		ans = ans+fullCommand[i];
	}

}

int client(char **argv) {
	int fdsocket;
	struct client_info stat[5];
	char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
	memset(buffer, '\0', BUFFER_SIZE);
	int socket_to_connect = -1;

	while(TRUE) {
		char *commandWithNewLine = (char*) malloc(sizeof(char)*CMD_SIZE);
		memset(commandWithNewLine, '\0', CMD_SIZE);
		if(fgets(commandWithNewLine, CMD_SIZE-1, stdin) == NULL) {
			cse4589_print_and_log("Please input a command\n");
		}

		std::string commandDummy(commandWithNewLine, commandWithNewLine + strlen(commandWithNewLine)-1);
		std::string command = "";
		command = extractCommand(commandDummy);

		//CheckforCommonCommands
		if(checkAnyLowerCase(command)) {
			cse4589_print_and_log("The command should be given in all capital letters\n");
		}

		if("AUTHOR" == command) {
			cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "durbhasa");
		}

		if("IP" == command) {
			ip_command(command);
		}

		if("PORT" == command) {
			cse4589_print_and_log("PORT:%d\n", atoi(argv[2]));
		}

		if ("LIST" == command) {
			// char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
			// memset(msg, '\0', MSG_SIZE);
			// sprintf(msg,"LIST OTHER CLIENTS");
			// cse4589_print_and_log("LIST OTHER CLIENTS");
			// if(send(fd, msg, strlen(msg), 0) == strlen(msg)) {
			// 	cse4589_print_and_log("Done!\n");
			// }
			// cse4589_print_and_log("CLIENT LIST OPTION\n");
			
		}
		
		if ("LOGIN" == command) {
			cse4589_print_and_log("Inside LOGIN\n");
			struct sockaddr_in serv_addr;
			memset(&serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			string serv_ip = "";
			string serv_port = "";

			int it = 0;
			int word = 0;
			cse4589_print_and_log("Command dummy is %s\n", commandDummy.c_str());
			string dummy = "";
			while(it < commandDummy.size()) {
				if(commandDummy[it] == ' ') {
					if(word == 0) {
						word++;
						dummy = "";
					} else if(word == 1) {
						serv_ip = dummy;
						dummy = "";
						word++;
					}
				} else {
					dummy += commandDummy[it];
				}
				it++;
			}

			serv_port = dummy;

			cse4589_print_and_log("ip address is %s\n",serv_ip.c_str());
			cse4589_print_and_log("port is %s\n", serv_port.c_str());

			serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
   			serv_addr.sin_port = htons(atoi(serv_port.c_str()));

			socket_to_connect = socket(AF_INET,SOCK_STREAM, 0);
			if(socket_to_connect == -1) {
				cse4589_print_and_log("Error creating socket\n");
				return-1;
			}
			/* Connect */
			if(connect(socket_to_connect, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
				cse4589_print_and_log("Connect failed\n");
			}	


			if(recv(socket_to_connect, buffer, BUFFER_SIZE, 0) >= 0) { 
				cse4589_print_and_log("Server responded: %s\n", buffer);
				cse4589_print_and_log("%d size of stat\n",sizeof(stat));
				memcpy(&stat,(struct client_info*)buffer,sizeof(stat));
				fflush(stdout);
			} else {
				cse4589_print_and_log("RECVD NOTHIN\n");
			}
			
			cse4589_print_and_log("|--------------------------------------------------------------------|\n");			
			cse4589_print_and_log("| List ID    |           Host Name        | IP address     | Port No |\n");  
			cse4589_print_and_log("|--------------------------------------------------------------------|\n"); 
				
			for(int i=0;i<5;i++) {
				if((stat[i].sock_fd) != -1) {
					cse4589_print_and_log("|     %-5d  |   %-25s| %-16s|%-8d|\n", stat[i].list_id, stat[i].hostname, stat[i].ip_addr, stat[i].port_num);								}
				}
			cse4589_print_and_log("|--------------------------------------------------------------------|\n");
		}


		if ("LOGOUT" == command) {
			cse4589_print_and_log("BEFORE CLOSING\n");
			close(socket_to_connect);
			cse4589_print_and_log("CLOSED\n");
		}

		fflush(stdout);
	
		
	/*	
		cse4589_print_and_log("I got: %s(size:%d chars)", msg, strlen(msg));
		
		cse4589_print_and_log("\nSENDing it to the remote server ... ");
		if(send(server, msg, strlen(msg), 0) == strlen(msg))
			cse4589_print_and_log("Done!\n");
		fflush(stdout);
		

		char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
		memset(buffer, '\0', BUFFER_SIZE);
		
		if(recv(server, buffer, BUFFER_SIZE, 0) >= 0){
			cse4589_print_and_log("Server responded: %s", buffer);
			fflush(stdout);
		}
		*/
	}
	// Else, if we were able to connect, try to send something.
	cse4589_print_and_log("Returning from client function.\n");
	//return 0;

}


int server(int argc, char **argv) {
	// Begin a server listening on the port given.
	int head_socket, port_num = atoi(argv[2]), sock_index, caddr_len, client_fd=0;
	fd_set master_list, watch_list;
	struct sockaddr_in addr,client_addr;

	struct client_info client_record[5];

	int serv_fd = socket(AF_INET,SOCK_STREAM, 0);
	if(serv_fd == -1) {
		cse4589_print_and_log("Error creating socket\n");
		return-1;
	}
	
	memset(&addr, 0, sizeof(addr));
	addr.sin_port = htons(port_num);
  	addr.sin_addr.s_addr = 0;
 	addr.sin_addr.s_addr = INADDR_ANY;
  	addr.sin_family = AF_INET; 
	
	if(bind(serv_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in) ) == -1) {
		cse4589_print_and_log("Error binding socket to the given port\n");
		return -1;
  	}

	if(listen(serv_fd, BACKLOG) < 0) {
		cse4589_print_and_log("Unable to listen to port.\n");
		exit(-1);
	} else {
		cse4589_print_and_log("I am able to listen to port.\n");
	}

	/* Zero select FD sets */
	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);
	
	
	FD_SET(serv_fd, &master_list); /* Register the listening socket */
	FD_SET(STDIN, &master_list); /* Register STDIN */
	
	head_socket = serv_fd;

	for(int i=0;i<5;i++) {
		client_record[i].sock_fd = -1;
	}

	while(true) {
		memcpy(&watch_list, &master_list, sizeof(master_list));
		// printf("\n[durbhasa-SERVER@CSE489/589]$ ");
		fflush(stdout);
		// cse4589_print_and_log("inside while\n");
		/* select() system call. This will BLOCK */
		struct timeval tv = {20, 0};
		int selret = select(head_socket+1, &watch_list, NULL, NULL, &tv);
		if(selret < 0) {
			cse4589_print_and_log("select failed \n");
			exit(-1);
		}
		
		cse4589_print_and_log("%d", selret);
		if(selret > 0) {
			/* Loop through socket descriptors to check which ones are ready */
			for(sock_index=0; sock_index<=head_socket; sock_index+=1) {
				if(FD_ISSET(sock_index, &watch_list)) {
					/* Check if new command on STDIN */
					if (sock_index == STDIN) {
						char *commandWithNewLine = (char*) malloc(sizeof(char)*CMD_SIZE);
						memset(commandWithNewLine, '\0', CMD_SIZE);
						if(fgets(commandWithNewLine, CMD_SIZE-1, stdin) == NULL) {
							cse4589_print_and_log("Please input a command\n");
							exit(-1);
						}

						char *command = (char *) calloc(strlen(commandWithNewLine), sizeof(char));
						memcpy(command, commandWithNewLine, strlen(commandWithNewLine)-1);
						command[strlen(commandWithNewLine)-1] = '\0';

						cse4589_print_and_log("Received the command %s\n", command);

						//CheckforCommonCommands
						if(checkAnyLowerCase(&command[0])) {
						        cse4589_print_and_log("The command should be given in all capital letters\n");
						}

						if(strcmp("AUTHOR",command) == 0) {
							cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "durbhasa");
						}
						if (strcmp("IP",command) == 0) {
							ip_command(command);
						}		
						if (strcmp("PORT",command) == 0) {
			   				cse4589_print_and_log("PORT:%d\n",ntohs(addr.sin_port));
						}
						if (strcmp("LIST",command) == 0) {
							cse4589_print_and_log("|--------------------------------------------------------------------|\n");			
							cse4589_print_and_log("| List ID    |           Host Name        | IP address     | Port No |\n");  
							cse4589_print_and_log("|--------------------------------------------------------------------|\n"); 
				
			   				for(int i=0;i<5;i++) {
								if((client_record[i].sock_fd) != -1) {
									cse4589_print_and_log("|     %-5d  |   %-25s| %-16s|%-8d|\n", client_record[i].list_id, client_record[i].hostname, client_record[i].ip_addr, client_record[i].port_num);								
								}
							}
							cse4589_print_and_log("|--------------------------------------------------------------------|\n"); 					
						}
						free(commandWithNewLine);
					} else if(sock_index == serv_fd) { /* Check if new client is requesting connection */
						cse4589_print_and_log("Inside clients new connection if case\n");
						caddr_len = sizeof(client_addr);
						client_fd = accept(serv_fd, (struct sockaddr *)&client_addr, (socklen_t *) &caddr_len);
						if(client_fd < 0) {
							perror("Accept failed.\n");
						}
					
						cse4589_print_and_log("Remote Host connected!\n");                        
						
						/* Add to socket list being watched */
						FD_SET(client_fd, &master_list);
						if(client_fd > head_socket)  {
							head_socket = client_fd;
						}
					
						cse4589_print_and_log("client socket %d", client_fd);
						for(int i=0;i<5;i++) {
							if((client_record[i].sock_fd) == -1) {
								client_record[i].sock_fd = client_fd;	
								//struct sockaddr_in client_addr;
								//socklen_t len=sizeof(client_addr);
    							//int ret = getpeername(fdaccept, (struct sockaddr *)&client_addr, len);
								//cse4589_print_and_log("\n ret value %d",ret);
								client_record[i].list_id=i+1;   								
   								client_record[i].port_num = ntohs(client_addr.sin_port);
   								cse4589_print_and_log("client PORT:%hu\n",client_record[i].port_num);
								client_record[i].loggedIn = true;
								client_record[i].hostname = "krishnaaaaa";
								strcpy(client_record[i].ip_addr, inet_ntoa(client_addr.sin_addr));

   								cse4589_print_and_log("CLIENT PORT ntohs:%d\n",ntohs(client_addr.sin_port));

   								cse4589_print_and_log("client ip:%s\n",client_record[i].ip_addr);

   								cse4589_print_and_log("client name:%s\n",client_record[i].hostname);
							
    							break;	
							}
						}

						//once connect is successfull send the statistics
						cse4589_print_and_log("BEFORE SEND\n");	
						int a=0;
						if(a=send(client_fd, (void*)&client_record, sizeof(client_record), 0) == sizeof(client_record)) {
							cse4589_print_and_log("Done! bytes sent [%d] [%d\n", a, sizeof(client_record));
						}
						cse4589_print_and_log("AFTER SEND\n");
					} else { 
						/* Read from existing clients */
						/* Initialize buffer to receieve response */
						char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
						memset(buffer, '\0', BUFFER_SIZE);
						
						if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0) {
							close(sock_index);
							printf("Remote Host terminated connection!\n");
							
							for(int i=0;i<5;i++) {
								if(client_record[i].sock_fd == sock_index) {
									client_record[i].loggedIn = false;
								}
							}
							/* Remove from watched list */
							FD_CLR(sock_index, &master_list);
						} else {
							//Process incoming data from existing clients here...
							printf("\nClient sent me: %s\n", buffer);
							
							if(strcmp(buffer,"LIST OTHER CLIENTS")==0) {
								printf("ECHOing it back to the remote host ... ");	
							}

							if(send(client_fd, buffer, strlen(buffer), 0) == strlen(buffer)) {
								printf("Done!\n");
							}

							fflush(stdout);
						}						
						free(buffer);
					}
				}
			}
		}
	}
}	

int main(int argc, char **argv) {
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));

	/*Start Here*/

	if(argc < 3) { // If we don't have the specified amount of args
		cse4589_print_and_log("Invalid number of arguments.\n");
		return -1;
	}

	if(strcmp(argv[1],"s") == 0) {	 // If we want to be a server
		cse4589_print_and_log("About to create a server.\n");
		server(argc, argv);
	}

	if(strcmp(argv[1],"c") == 0) {	// want to be a client
		cse4589_print_and_log("About to create a client.\n");
		client(argv);
	}

	return 0;
}