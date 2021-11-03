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
#include <regex>
using namespace std;


struct client_info {
	int list_id;
	char* hostname;
	char ip_addr[16];
	int port_num;

	//own ?
    int sock_fd;
    bool loggedIn;
	int active_clients;
};

string port;
string host_name;
string ip;
int server_fd;
/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */


std::string extractCommand(std::string fullCommand) {
	string ans = "";

	for(int i=0;i<fullCommand.size();i++) {
		if(fullCommand[i] == ' ') return ans;
		ans = ans+fullCommand[i];
	}

	return ans;
}	

void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static bool customSort(struct client_info a, struct client_info b) {
	return (a.port_num <= b.port_num);
}

void printSortedList(struct client_info *client_record, int activeClients) {
	cout<<activeClients<<endl;
	sort(client_record, client_record+activeClients, customSort);
    for(int i=0;i<activeClients;i++) {
        if((client_record[i].sock_fd) != -1) {
            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i+1, client_record[i].hostname, client_record[i].ip_addr, client_record[i].port_num);								
        }
    } 	
}

int client(char **argv) {
	int fdsocket, activeClients;
	struct client_info *stat;
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
			printSortedList(stat, activeClients);
		}
		
		if ("LOGIN" == command) {
			struct sockaddr_in serv_addr;
			memset(&serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			string serv_ip = "";
			string serv_port = "";

			int it = 0;
			int word = 0;
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

			cout<<serv_port<<endl;
			cout<<serv_ip<<endl;
			
			serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
   			serv_addr.sin_port = htons(atoi(serv_port.c_str()));

			socket_to_connect = socket(AF_INET,SOCK_STREAM, 0);
			if(socket_to_connect == -1) {
				cse4589_print_and_log("Error creating socket\n");
				return-1;
			}


			struct sockaddr_in my_addr1;
			my_addr1.sin_family = AF_INET;
			my_addr1.sin_addr.s_addr = INADDR_ANY;
   			my_addr1.sin_port = htons(atoi(argv[2]));
			my_addr1.sin_addr.s_addr = INADDR_ANY;
			if (bind(socket_to_connect, (struct sockaddr*) &my_addr1, sizeof(struct sockaddr_in)) == 0) {
				printf("Binded Correctly\n");
			} else {
				printf("BIND FAILED");
			}

			/* Connect */
			if(connect(socket_to_connect, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
				cse4589_print_and_log("Connect failed\n");
			}

			if(recv(socket_to_connect, buffer, BUFFER_SIZE, 0) >= 0) { 
				cse4589_print_and_log("Server responded: %s\n", buffer);
				cse4589_print_and_log("%d size of stat\n",sizeof(stat));
				// memcpy(&stat, (struct client_info*)buffer, sizeof(stat));
				stat = (struct client_info *) buffer;
				fflush(stdout);
			} else {
				cse4589_print_and_log("RECVD NOTHIN\n");
			}

			for(int i=0;i<5;i++) {
				if((stat[i].sock_fd) != -1) {
					cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",stat[i].list_id, stat[i].hostname, stat[i].ip_addr, stat[i].port_num);
				}
			}
		}


		if ("LOGOUT" == command) {
			cse4589_print_and_log("BEFORE CLOSING\n");
			close(socket_to_connect);
			cse4589_print_and_log("CLOSED\n");
		}


		if("SEND" == command) {
			// SEND <client-ip> <msg>
			// Send message: <msg> to client with IP address: <client-ip>. <msg> can have a maximum length of 256 bytes and will consist of valid ASCII characters.

			// Exceptions to be handled

			// Invalid IP address.
			// Valid IP address which does not exist in the local copy of the list of logged-in clients (This list may be outdated. Do not update it as a result of this check).

			string dummy = "", client_ip = "", message = "";
			int it=0, word = 0;
			while(it < commandDummy.size()) {
				if(commandDummy[it] == ' ') {
					if(word == 0) {
						word++;
						dummy = "";
					} else if(word == 1) {
						client_ip = dummy;
						dummy = "";
						word++;
					}
				} else {
					dummy += commandDummy[it];
				}
				it++;
			}

			message = dummy;
			// if(a = send(server_fd, (void *) message, sizeof(message), 0) >= 0) {
				
			// }
		}
	}

	// 	cse4589_print_and_log("I got: %s(size:%d chars)", msg, strlen(msg));
		
	// 	cse4589_print_and_log("\nSENDing it to the remote server ... ");
	// 	if(send(server, msg, strlen(msg), 0) == strlen(msg))
	// 		cse4589_print_and_log("Done!\n");
	// 	fflush(stdout);
		

	// 	char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
	// 	memset(buffer, '\0', BUFFER_SIZE);
		
	// 		if(recv(stat->sock_fd, buffer, BUFFER_SIZE, 0) >= 0){
	// 			cse4589_print_and_log("Server responded: %s", buffer);
	// 			fflush(stdout);
	// 	}
	// 	}
	
	// 	fflush(stdout);
	// }

	cse4589_print_and_log("Returning from client function.\n");
	//return 0;
}

void getHostNameIpAndPort(char *myPort) {
	port = myPort;
	char myHostName[1024];
	gethostname(myHostName, sizeof(myHostName)-1);
	host_name = myHostName;

    char buffer[256];
    size_t bufLen = 256;

    int udp_sock_fd = socket(AF_INET,SOCK_DGRAM,0);
    const char *kGoogleDnsIp = "8.8.8.8";
    uint16_t kDnsPort = 53;
    struct sockaddr_in googleServInfo;
    memset(&googleServInfo, 0, sizeof(googleServInfo));
    googleServInfo.sin_family = AF_INET;
    googleServInfo.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    googleServInfo.sin_port = htons(kDnsPort);

    if(connect(udp_sock_fd, (struct sockaddr *) &googleServInfo, sizeof(googleServInfo)) == -1) {
		cse4589_print_and_log("Failed to connect with the Google server");
    }

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    if(getsockname(udp_sock_fd, (struct sockaddr *)&name, &namelen) == -1) {
        cse4589_print_and_log("Failed to get the socket name of the host\n");
    }
    const char *myIp = inet_ntop(AF_INET, &name.sin_addr, buffer, bufLen);
    ip = myIp;
    close(udp_sock_fd);
}

void initializeServer(char **argv) {
	getHostNameIpAndPort(argv[2]);

	int status;
  	struct addrinfo hints;
  	memset(&hints, 0, sizeof hints);  
  	hints.ai_family = AF_INET;        
  	hints.ai_socktype = SOCK_STREAM;  
  	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *addrInfo;

  	if ((status = getaddrinfo(NULL, argv[2], &hints, &addrInfo)) != 0) {
    	exit(1);
  	}

	//Do we need set sock opt here ?
  	
	if(server_fd = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol) == -1) {
		cse4589_print_and_log("Error creating socket\n");
		exit(-1);
	}

	if(bind(server_fd, (struct sockaddr *) &addrInfo, sizeof(struct sockaddr_in)) == -1) {
		cse4589_print_and_log("Error binding socket to the given port\n");
		exit(-1);
  	}

	if(listen(server_fd, BACKLOG) < 0) {
		cse4589_print_and_log("Unable to listen to port.\n");
		exit(-1);
	} 
	// else {
	// 	cse4589_print_and_log("I am able to listen to port.\n");
	// }
}


int server(int argc, char **argv) {
	initializeServer(argv);
	int sock_index, caddr_len, client_fd=0, activeClients = 0;
	char buffer[65535];
	fd_set watch_list, master_list;
	struct sockaddr_in client_addr;
	struct client_info client_record[5];
	// int clientSockets[5] = {0};

	/* Zero select FD sets */
	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);
	int max_socket = server_fd;

	for(int i=0;i<5;i++) {
		client_record[i].sock_fd = 0;
	}

	while(true) {
		FD_ZERO(&watch_list);
		FD_SET(server_fd, &master_list); /* Register the listening socket */
		FD_SET(fileno(stdin), &master_list); /* Register STDIN */

		// memcpy(&watch_list, &master_list, sizeof(master_list));
		// fflush(stdout);
		// struct timeval tv = {20, 0};

		int selret = select(max_socket+1, &watch_list, NULL, NULL, NULL);
		if(selret < 0) {
			cse4589_print_and_log("select failed \n");
			exit(-1);
		}

		for (int i = 0; i < 5; i++) {
      		int client_sock_fd = client_record[i].sock_fd;
      		if (client_sock_fd > 0) FD_SET(client_sock_fd, &watch_list);
      		if (client_sock_fd > max_socket) max_socket = client_sock_fd;
    	}
		
		if(selret > 0) {
			/* Loop through socket descriptors to check which ones are ready */
			// for(sock_index=0; sock_index <= max_socket; sock_index+=1) {
				if(FD_ISSET(fileno(stdin), &watch_list)) {
					/* Check if new command on STDIN */
					// if (sock_index == STDIN) {
						char *commandWithNewLine = (char*) malloc(sizeof(char)*CMD_SIZE);
						memset(commandWithNewLine, '\0', CMD_SIZE);
						if(fgets(commandWithNewLine, CMD_SIZE-1, stdin) == NULL) {
							cse4589_print_and_log("Please input a command\n");
							exit(-1);
						}

						char *command = (char *) calloc(strlen(commandWithNewLine), sizeof(char));
						memcpy(command, commandWithNewLine, strlen(commandWithNewLine)-1);
						command[strlen(commandWithNewLine)-1] = '\0';

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
			   				cse4589_print_and_log("PORT:%d\n", port);
						}
						if (strcmp("LIST",command) == 0) {
							printSortedList(client_record, activeClients);
						}
						if (strcmp("SEND", command) == 0) {
							server_handle_send(command);
						} 

						free(commandWithNewLine);
					}  else if(FD_ISSET(server_fd, &watch_list)) { /* Check if new client is requesting connection */
						// cse4589_print_and_log("Inside clients new connection if case\n");
						caddr_len = sizeof(client_addr);
						client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *) &caddr_len);
						if(client_fd < 0) {
							cse4589_print_and_log("Accept failed.\n");
						}
					
						// cse4589_print_and_log("Remote Host connected!\n");                        
						
						/* Add to socket list being watched */
						FD_SET(client_fd, &master_list);
						if(client_fd > max_socket)  {
							max_socket = client_fd;
						}

						activeClients++;
						cse4589_print_and_log("client socket %d\n", client_fd);
						for(int i=0;i<5;i++) {
							if((client_record[i].sock_fd) == -1) {
								client_record[i].sock_fd = client_fd;	
   								client_record[i].port_num = ntohs(client_addr.sin_port);
								client_record[i].loggedIn = true;
								client_record[i].active_clients = activeClients; 

								struct in_addr addr;
								struct hostent *he;
								char *client_ip = inet_ntoa(client_addr.sin_addr);
								inet_aton(client_ip, &addr);
								he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
								client_record[i].hostname = he->h_name;
								strcpy(client_record[i].ip_addr, inet_ntoa(client_addr.sin_addr));
    							break;	
							}
						}

						//once connect is successfull send the statistics
						cse4589_print_and_log("BEFORE SEND\n");
						char *testMsg = (char *) malloc(sizeof(char *) * BUFFER_SIZE);
						testMsg = "Hello Sandy ! Just try directly";
						cse4589_print_and_log("%s\n", testMsg);
						int a;
						a = send(client_fd, (void *) &testMsg, sizeof(testMsg), 0);
						if( a == sizeof(testMsg)) {
							cse4589_print_and_log("Done! bytes sent [%d]\n ", sizeof(testMsg));
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
									activeClients--;
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
			// }
		}
	}	

int main(int argc, char **argv) {
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));

	/*Start Here*/

	if(argc < 3) { // If we don't have the specified amount of args
		return -1;
	}

	if(strcmp(argv[1],"s") == 0) {	 // If we want to be a server
		server(argc, argv);
	}

	if(strcmp(argv[1],"c") == 0) {	// want to be a client
		client(argv);
	}

	return 0;
}
