#include "../include/global.h"
#include "../include/logger.h"


int ip_command(std::string command) {
    char myHostName[1024];
    gethostname(myHostName, sizeof(myHostName));

    char buffer[256];
    size_t bufLen = 256;

    // now attempt to connect to this google's server address
    int udp_sock_fd = socket(AF_INET,SOCK_DGRAM,0);
    const char *kGoogleDnsIp = "8.8.8.8";
    uint16_t kDnsPort = 53;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    serv.sin_port = htons(kDnsPort);

    if(connect(udp_sock_fd, (struct sockaddr *) &serv, sizeof(serv)) == -1) {
        cse4589_print_and_log("[%s:ERROR]\n", command.c_str());
        cse4589_print_and_log("[%s:END]\n", command.c_str());
        // cse4589_print_and_log("unable to connect to the google server\n");			
    }

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    if(getsockname(udp_sock_fd, (struct sockaddr *) &name, &namelen) == -1) {
        cse4589_print_and_log("Failed to get the socket name of the host\n");
    }

    const char *myIp = inet_ntop(AF_INET, &name.sin_addr, buffer, bufLen);
    close(udp_sock_fd);

    cse4589_print_and_log("[%s:SUCCESS]\n", command.c_str());
    cse4589_print_and_log("IP:%s\n", buffer);
    cse4589_print_and_log("[%s:END]\n", command.c_str());

    // void *ip_addr;
    // cse4589_print_and_log("%d\n",hints.ai_family);			
    // if (hints.ai_family == AF_INET) {
    // 	 // IPv4
    // 	 cse4589_print_and_log("Inside ipv4\n");
    // 	struct sockaddr_in *ipv4 = (struct sockaddr_in *) hints.ai_addr;
    // 	cse4589_print_and_log("Inside ipv4 line 2\n");
    // 	ip_addr = &(ipv4->sin_addr);
    // 	cse4589_print_and_log("Inside ipv4 line 3\n");
    // } else { // IPv6
    // 	struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)hints.ai_addr;
    // 	ip_addr = &(ipv6->sin6_addr);
    // }

    // char ip_str[INET_ADDRSTRLEN];
    // cse4589_print_and_log("Before inet_ntop\n");
    // inet_ntop(hints.ai_family, ip_addr, ip_str, sizeof ip_str);
        
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