#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "../include/global.h"
#include "../include/logger.h"

using namespace std;


extern string host_name;
extern string port;
extern string ip;
extern int server_fd;