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
	string hostname;
	string ip_addr;
	int port_num;

	//own ?
	string port;
	int msgSent;
	int msgRecvd;
    int sock_fd;
    bool loggedIn;
};

string port;
string host_name;
string ip;
int server_fd;
int client_fd = 0;

vector<client_info> clientData;
/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */


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
	return;
}


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

void printSortedList(vector<client_info> clientData) {
	sort(clientData.begin(), clientData.end(), customSort);
    for(int i=0;i<clientData.size();i++) {
        if((clientData[i].sock_fd) != -1) {
            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i+1, clientData[i].hostname.c_str(), clientData[i].ip_addr.c_str(), clientData[i].port_num);
        }
    }
}

int client(char **argv) {

	getHostNameIpAndPort(argv[2]);
	int fdsocket, activeClients;
	char buffer[65535];
	fd_set watch_list;
	int max_socket;

	while(TRUE) {
		FD_ZERO(&watch_list);
    	FD_SET(fileno(stdin), &watch_list);
    	FD_SET(max_socket, &watch_list);
    	max_socket = client_fd;
		char *commandWithNewLine = (char*) malloc(sizeof(char)*CMD_SIZE);
		memset(commandWithNewLine, '\0', CMD_SIZE);
		if(fgets(commandWithNewLine, CMD_SIZE-1, stdin) == NULL) {
			cse4589_print_and_log("Please input a command\n");
		}

		std::string commandDummy(commandWithNewLine, commandWithNewLine + strlen(commandWithNewLine)-1);
		std::string command = "";
		command = extractCommand(commandDummy);

		// int selret = select(max_socket+ 1, &watch_list, NULL, NULL, NULL);

		// if(selret < 0) {
		// 	cse4589_print_and_log("select failed \n");
		// 	exit(-1);
		// }

		//Check for Common Commands

		// if(selret > 0) {
			if(checkAnyLowerCase(command)) {
				cse4589_print_and_log("The command should be given in all capital letters\n");
			}

			if("AUTHOR" == command) {
				cse4589_print_and_log("[%s:SUCCESS]\n", command.c_str());
				cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "durbhasa");
				cse4589_print_and_log("[%s:END]\n", command.c_str());
			}

			if("IP" == command) {
				cse4589_print_and_log("[%s:SUCCESS]\n", command.c_str());
				cse4589_print_and_log("IP:%s\n", ip.c_str());
				cse4589_print_and_log("[%s:END]\n", command.c_str());
			}	

			if("PORT" == command) {
				cse4589_print_and_log("[%s:SUCCESS]\n", command.c_str());
				cse4589_print_and_log("PORT:%s\n", port.c_str());
				cse4589_print_and_log("[%s:END]\n", command.c_str());
			}

			if ("LIST" == command) {
				//TO DO after login done properly.
				printSortedList(clientData);
			}
		
			if ("LOGIN" == command) {
				// struct sockaddr_in serv_addr;
				struct addrinfo hints, *servAddrInfo;
				memset(&hints, 0, sizeof hints);
  				hints.ai_family = AF_INET;
  				hints.ai_socktype = SOCK_STREAM;

				//Extract server ip and server port from commandDummy.
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

  				if (client_fd == 0) {
    				getaddrinfo(serv_ip.c_str(), serv_port.c_str(), &hints, &servAddrInfo);
    				client_fd = socket(servAddrInfo->ai_family, servAddrInfo->ai_socktype, servAddrInfo->ai_protocol);
    				if (client_fd == -1) {
						cse4589_print_and_log("Error creating socket\n");
						return -1;
    				}

					if(connect(client_fd, servAddrInfo->ai_addr, servAddrInfo->ai_addrlen) < 0) {
						cse4589_print_and_log("Connect failed\n");
					}
				}

				string msg = "LOGIN " + host_name + " " + ip + " " + port;
  				send(client_fd, msg.c_str(), strlen(msg.c_str()), 0);

				vector<string> recvmsg, recvdata;
				char temp[65535];
				int recvSize = recv(client_fd, temp, sizeof(temp), 0);
				if(recvSize < 0) { 
					cse4589_print_and_log("Recv Failed\n");
				}

				clientData.clear();
				cout<<temp<<endl;
				fflush(stdout);
			}


			if ("LOGOUT" == command) {
				cse4589_print_and_log("BEFORE CLOSING\n");
				close(client_fd);
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
			}
		// }
		
	}

	//return 0;
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
	int opt = 1;
  	server_fd = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
	if(server_fd == -1) {
		cse4589_print_and_log("Error creating socket\n");
		exit(-1);
	}

	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) <0) {
		cse4589_print_and_log("Set sock opt failed\n");
	}

	if(bind(server_fd, addrInfo->ai_addr, addrInfo->ai_addrlen) == -1) {
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

client_info* newClientInfo(int sock_fd, string hostname, string ip, int port_num) {
  client_info* client = new client_info;
  client->sock_fd = sock_fd;
  client->hostname = hostname;
  client->ip_addr = ip;
  client->port = to_string(port_num);
  client->port_num = port_num;
  client->msgSent = 0;
  client->msgRecvd = 0;
  client->loggedIn = true;
  return client;
}

client_info* getClientData(int client_fd) {
	for(int i=0;i<clientData.size();i++) {
		if(clientData[i].sock_fd == client_fd) return &clientData[i];
	}

	return NULL;
}

vector<string> extractLoginParams(string msg) {
	vector<string> params;
	string dummy = "";
	for(int i=0;i<msg.size();i++) {
		if(msg[i] == ' ' || msg[i] == '\n') {
			params.push_back(dummy);
			dummy = "";
		} else {
			dummy += msg[i];
		}
	}

	params.push_back(dummy);
	return params;
}


int server(int argc, char **argv) {
	initializeServer(argv);
	struct sockaddr_in client_addr; 
	int sock_index, activeClients = 0;
	char buffer[65535];
	fd_set watch_list, master_list;

	int clientSockets[5] = {0};

	/* Zero select FD sets */
	// FD_ZERO(&master_list);
	// FD_ZERO(&watch_list);
	int max_socket = server_fd;

	while(true) {
		FD_ZERO(&watch_list);
		FD_SET(server_fd, &watch_list); /* Register the listening socket */
		FD_SET(fileno(stdin), &watch_list); /* Register STDIN */

		// memcpy(&watch_list, &master_list, sizeof(master_list));
		// fflush(stdout);
		// struct timeval tv = {20, 0};

		for (int i = 0; i < 5; i++) {
      		int client_sock_fd = clientSockets[i];
      		if (client_sock_fd > 0) FD_SET(client_sock_fd, &watch_list);
      		if (client_sock_fd > max_socket) max_socket = client_sock_fd;
    	}

		int selret = select(max_socket+1, &watch_list, NULL, NULL, NULL);
		if(selret < 0) {
			cse4589_print_and_log("select failed \n");
			exit(-1);
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

						//CheckforCommonCommands867uy65
						if(checkAnyLowerCase(&command[0])) {
						    cse4589_print_and_log("The command should be given in all capital letters\n");
						}

						if(strcmp("AUTHOR",command) == 0) {
							cse4589_print_and_log("[%s:SUCCESS]\n", command);
							cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "durbhasa");
							cse4589_print_and_log("[%s:END]\n", command);
						}
						if (strcmp("IP",command) == 0) {
							cse4589_print_and_log("[%s:SUCCESS]\n", command);
							cse4589_print_and_log("IP:%s\n", ip.c_str());
							cse4589_print_and_log("[%s:END]\n", command);
						}		

						if (strcmp("PORT",command) == 0) {
							cse4589_print_and_log("[%s:SUCCESS]\n", command);
			   				cse4589_print_and_log("PORT:%s\n", port.c_str());
							cse4589_print_and_log("[%s:END]\n", command);
						}

						if (strcmp("LIST",command) == 0) {
							printSortedList(clientData);
						}
						if (strcmp("SEND", command) == 0) {
							server_handle_send(command);
						} 

						free(commandWithNewLine);
					}  else if(FD_ISSET(server_fd, &watch_list)) { /* Check if new client is requesting connection */
						socklen_t caddr_len = sizeof(client_addr);
						int new_client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &caddr_len);
						if(new_client_fd < 0) {
							cse4589_print_and_log("Accept failed.\n");
						}
					
						for (int i = 0; i < 5; i++) {
        					if(clientSockets[i] == 0) {
          						clientSockets[i] = new_client_fd;
          						break;
        					}
						}

						//Receive Login Request
						recv(new_client_fd, buffer, sizeof(buffer), 0);
						string msg = buffer;
						vector<string> loginParams = extractLoginParams(msg);

						//once connect is successfull send the statistics
						client_info *clientInfo = getClientData(new_client_fd);
						if(clientInfo == NULL) {
							cout<<loginParams[1]<<" "<<loginParams[1]<<" "<<loginParams[2]<<" "<<loginParams[3]<<endl;
							clientInfo = newClientInfo(client_fd, loginParams[1], loginParams[2], stoi(loginParams[3]));
							clientData.push_back(*clientInfo);
						} else {
							clientInfo->loggedIn = true;
						}

						msg = "LOGINSUCCESS\n";
						for(int i=0;i<clientData.size();i++) {
							if(clientData[i].loggedIn) {
								msg += clientData[i].hostname + " " + clientData[i].ip_addr + " " + clientData[i].port + "\n";
							}
						}
						msg += "LOGINEND";
						send(new_client_fd, msg.c_str(), strlen(msg.c_str()), 0);
					} else { 
						/* Read from existing clients */
						/* Initialize buffer to receieve response */
						char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
						memset(buffer, '\0', BUFFER_SIZE);
						
						if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0) {
							close(sock_index);
							printf("Remote Host terminated connection!\n");
							
							for(int i=0;i<5;i++) {
								if(clientData[i].sock_fd == sock_index) {
									clientData[i].loggedIn = false;
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
