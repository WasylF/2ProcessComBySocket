#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
	int n = recv(newsockfd,buffer,255, MSG_DONTWAIT);
	if (n < 0) buffer[0] = 'c'; //"computing";
}

void close_connection(int newsockfd, int sockfd)
{
	close(newsockfd);
	close(sockfd);
}

int get_free_port()
{
	srand(time(NULL));
	while (1)
	{
		struct sockaddr_in sin;
		int port = abs(rand())%32767 + 1;
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd == -1) continue;

		bzero((char *) &sin, sizeof(sin));

		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = INADDR_ANY;
		sin.sin_port = htons(port);

		if (bind(sockfd, (struct sockaddr *) &sin, sizeof(sin)) >= 0)
		{
			close(sockfd);
			return port;
		}
	}

}

int create_process(string command)
{
	int pid = fork();
  	if (pid == -1) cout<<"Error creating process"<<endl;
  	if (pid == 0)
	{
		system(command.c_str());
		exit(0);
	}
	else
	{
        return pid;
	}
	return 0;
}

const int time_interval = 2;

int main(int argc, char *argv[])
{
	int portno = get_free_port();
	cout<<"Using port: "<<portno<<endl;

	string command = "./f.out " + std::to_string(portno) + "   100";
	int fPid = create_process(command);
	pair<int,int> f_connection = make_connection(portno);

	sleep(1);
	portno = get_free_port();
	cout<<"Using port: "<<portno<<endl;
	command = "./g.out " + std::to_string(portno) + "   10";
	int gPid = create_process(command);
	pair<int,int> g_connection = make_connection(portno);

	char* buffer = new char[256];
	bool computed = 0;

	// -1 - unknown, 0 - false, 1 - true
	int f_result = -1;
	int f = 0;
	int g_result = -1;
	int g = 0;


	while (!computed)
	{
		sleep(time_interval);

		if (f_result == -1)
		{
			read_from_socket(f_connection.first, buffer);
			if (buffer[0] != 'c')
			{
				f = atoi(buffer);
				printf("f result: %d\n", f);
				f_result = f % 123456 < 54321;
			}
			else
			{
				printf("f: still computing\n");
			}
		}

		if (g_result == -1)
		{
			read_from_socket(g_connection.first, buffer);
			if (buffer[0] != 'c')
			{
				g = atoi(buffer);
				printf("g result: %d\n", g);
				g_result = g > 10;
			}
			else
			{
				printf("g: still computing\n");
			}
		}

		computed = (f_result==1 && g_result==1) || f_result==0 || g_result==0;
		if (!computed)
		{
			cout<<"Press 1 to continue or 0 to exit: ";
			int cont;
			cin>>cont;
			if (cont==0) computed = 1;
		}

	}

	close_connection(f_connection.first, f_connection.second);
	close_connection(g_connection.first, g_connection.second);
	delete [] buffer;

    kill(fPid, SIGTERM);
    kill(gPid, SIGTERM);

	cout<<endl<<endl<<"######################"<<endl;
	cout<<"f_result: "<<f_result<<endl;
	cout<<"g_result: "<<g_result<<endl;
	cout<<"total_result: "<<(f_result==1 && g_result==1)<<endl;

	return 0;
}
