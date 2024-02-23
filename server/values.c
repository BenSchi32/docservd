#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void init_struct() {
	struct sockaddr_in addr = {
		AF_INET,
		htons(8080),
		0
	};
}

int main() {
	printf("AF_INET: %d\n", AF_INET);
	printf("SOCK_STREAM: %d\n", SOCK_STREAM);
	printf("Size sa_family_t: %d\n", sizeof(sa_family_t));
	printf("Size in_port_t: %d\n", sizeof(in_port_t));
	printf("Size struct in_addr: %d\n", sizeof(struct in_addr));
	printf("Size struct sockaddr_in: %d\n", sizeof(struct sockaddr_in));
	printf("G: %d\n", 'G');
	printf(" : %d\n", ' ');

	return 0;
}
