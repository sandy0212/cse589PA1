#include "../include/global.h"
#include "../include/logger.h"
#include "../include/PA1.h"


int ip_command(std::string command) {
    char myHostName[1024];
    gethostname(myHostName, sizeof(myHostName)-1);
    host_name = myHostName;

    char buffer[256];
    size_t bufLen = 256;

    // now attempt to connect to this google's server address
    int udp_sock_fd = socket(AF_INET,SOCK_DGRAM,0);
    const char *kGoogleDnsIp = "8.8.8.8";
    uint16_t kDnsPort = 53;
    struct sockaddr_in googleServInfo;
    memset(&googleServInfo, 0, sizeof(googleServInfo));
    googleServInfo.sin_family = AF_INET;
    googleServInfo.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    googleServInfo.sin_port = htons(kDnsPort);

    if(connect(udp_sock_fd, (struct sockaddr *) &googleServInfo, sizeof(googleServInfo)) == -1) {
        cse4589_print_and_log("[%s:ERROR]\n", command.c_str());
        cse4589_print_and_log("[%s:END]\n", command.c_str());		
    }

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    if(getsockname(udp_sock_fd, (struct sockaddr *)&name, &namelen) == -1) {
        cse4589_print_and_log("Failed to get the socket name of the host\n");
    }
    const char *myIp = inet_ntop(AF_INET, &name.sin_addr, buffer, bufLen);
    ip = myIp;
    close(udp_sock_fd);

    cse4589_print_and_log("[%s:SUCCESS]\n", command.c_str());
    cse4589_print_and_log("IP:%s\n", buffer);
    cse4589_print_and_log("[%s:END]\n", command.c_str());
    return 0;
}

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


int server_handle_send(std::string commandDummy) {

			// SEND <client-ip> <msg>
			// Send message: <msg> to client with IP address: <client-ip>. <msg> can have a maximum length of 256 bytes and will consist of valid ASCII characters.

			// Exceptions to be handled

			// Invalid IP address.
			// Valid IP address which does not exist in the local copy of the list of logged-in clients (This list may be outdated. Do not update it as a result of this check).
			//char *message = (char *) malloc(sizeof(char *) * MSG_SIZE);

			int it = 0;
			int word = 0;
			char * client_ip = "";

			char * dummy = "";
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

    return 0;
}