#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

using namespace std;


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

std::pair<int,int> make_connection(int portno)
{
	struct sockaddr_in serv_addr, cli_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");
	listen(sockfd,5);
	socklen_t clilen = sizeof(cli_addr);
	int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) error("ERROR on accept");

	return make_pair(newsockfd, sockfd);
}

void read_from_socket(int newsockfd, char* buffer)
{
	bzero(buffer,256);
	int n = read(newsockfd,buffer,255);

	if (n < 0) error("ERROR reading from socket");
}

void write_to_socket(int newsockfd)
{
	int n = write(newsockfd,"I got your message",18);
	if (n < 0) error("ERROR writing to socket");
}

void close_connection(int newsockfd, int sockfd)
{
	close(newsockfd);
	close(sockfd);
}


int main(int argc, char *argv[])
{
	pair<int,int> p = make_connection(1337);

	char* buffer = new char[256];
	while (1)
	{
		read_from_socket(p.first, buffer);
		printf("Here is the message: %s\n",buffer);
		write_to_socket(p.first);
		//sleep(1);
	}
	close_connection(p.first, p.second);
	delete [] buffer;
	
	return 0;
}
