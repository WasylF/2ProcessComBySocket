#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <utility>
#include <string>
#include <chrono>
#include <ctime>

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int make_connection(int portno, string hostName)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

	server = gethostbyname(hostName.c_str());
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)  error("ERROR connecting");

	return sockfd;
}

void write_to_socket(int sockfd, char* buffer)
{
	int n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) error("ERROR writing to socket");
}


void close_connection(int sockfd)
{
    close(sockfd);
}

int f(int n)
{
    if (n < 2) return 1;
    return f(n-1) + f(n-2);
}


int main(int argc, char *argv[])
{
	sleep(1);
	int portno = 0;
	int fibN = 100;
	if (argc>1) portno = atoi(argv[1]);
	if (argc>2) fibN = atoi(argv[2]);

	cout<<"Connecting to port: "<<portno<<endl;
	int sockfd = make_connection(portno, "y50-ub");
	char* buffer = new char[1];

	int res = f(fibN);

	const char* buf = to_string(res).c_str();
	write_to_socket(sockfd, (char*)buf);

	close_connection(sockfd);

    return 0;
}
