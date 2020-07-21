#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>


#ifndef PORT
#define PORT "3500"
#endif

#ifndef BACKLOG
#define BACKLOG 10
#endif

void sigchld_handler(int s){
	// waitpid() might rewrite errno so we have to save it and recover.
	int saved_errno = errno;
	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char const *argv[])
{
	int sockfd, newfd;
	struct addrinfo hostinfo, *servinfo, *p;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hostinfo, 0, sizeof hostinfo);
	hostinfo.ai_family = AF_UNSPEC;
	hostinfo.ai_socktype = SOCK_STREAM;
	hostinfo.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, PORT, &hostinfo, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("setsockopt");
		}

		if (bind(sockfd, p->ai_addr, p-> ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server : bind");
			continue;			
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connection.....................\n");

	while(1)
	{
		sin_size = sizeof their_addr;
		newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (newfd == -1)
		{
			perror("accept");
		}

		inet_ntop(their_addr.ss_family, 
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);

		printf("server: got connection from %s\n", s);

		if (!fork())
		{
			close(sockfd);
			if (send(newfd, "Hello, client!", 14, 0) == -1)
			{
				perror("send");
			}
			close(newfd);
			exit(0);
		}
		close(newfd);
	}
	return 0;
}