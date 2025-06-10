#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>



int Socket(int domain, int type, int protocol) {
    int MasterSocket = socket(domain, type, protocol);
    if (MasterSocket < 0) {
        perror("[master socket]");
        exit(EXIT_FAILURE);
    }
    return MasterSocket;
}


void Connect(int* sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = connect(*sockfd, addr, addrlen);
    if (res < 0) {
        perror("[connect]");
        exit(EXIT_FAILURE);
    }
    return;
}


int main(int argc, char** argv) {

    int ClientSocket = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(12345);
    SockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // connect(Socket, (struct sockaddr *)(&SockAddr), sizeof(SockAddr));
    Connect(&ClientSocket, (struct sockaddr *)(&SockAddr), sizeof(SockAddr)); 

    char Buffer[] = "PING";
    send(ClientSocket, Buffer, 4, MSG_NOSIGNAL);
    recv(ClientSocket, Buffer, 4, MSG_NOSIGNAL);

    shutdown(ClientSocket, SHUT_RDWR);
    close(ClientSocket);

    printf("%s\n", Buffer);

    return EXIT_SUCCESS;
}
