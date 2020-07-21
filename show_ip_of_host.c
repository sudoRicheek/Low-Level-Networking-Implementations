#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


/*
**	Run : ```./show_ip <hostname>```
**	Example : 
**		./show_ip "localhost"
**		IP Addresses for localhost:
**
**		IPv4: 127.0.0.1
*/


int main(int argc, char const *argv[])
{
	struct addrinfo hostinfo, *res, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];

	if (argc != 2)
	{
		fprintf(stderr, "usage: show_ip hostname\n");
		return 1;
	}

	memset(&hostinfo, 0, sizeof hostinfo);
	hostinfo.ai_family = AF_UNSPEC;
	hostinfo.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(argv[1], NULL, &hostinfo, &res)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror);
		return 2;
	}

	printf("IP Addresses for %s:\n\n", argv[1]);

	for (p = res; p != NULL; p = p->ai_next)
	{
		void *addr;
		char *ipver;

		if (p->ai_family == AF_INET)
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else
		{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		printf(" %s: %s\n", ipver, ipstr);
	}


	freeaddrinfo(res);
	return 0;
}