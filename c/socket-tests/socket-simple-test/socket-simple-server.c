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
        return EXIT_FAILURE;
    }
    return MasterSocket;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int bind_res = bind(sockfd, addr, addrlen);
    if (bind_res < 0) {
        perror("[bind master socket]");
    }
    return;
}

void Listen(int sockfd, int backlog) {
    int listen_res = listen(sockfd, backlog);
    if (listen_res < 0) {
        perror("[listen master socket]");
    }
    return;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addlen) {
    int SlaveSocket = accept(sockfd, addr, addlen);
    if (SlaveSocket < 0) {
        perror("[accept slave socket]");
        return EXIT_FAILURE;
    }
    return SlaveSocket;
}


int main(int argc, char** argv) {
    
    char* ip_address;
    int port = 0;

    if (argc < 3) {
        ip_address = "0.0.0.0";
        port = 12345;
    } else {
        ip_address = argv[1];
        port = atoi(argv[2]);
        // port = strtol(argv[2], NULL, 10);
    }

    int MasterSocket = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    // SockAddr.sin_port = htons(12345);
    // SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);   // Любой адрес
    SockAddr.sin_port = htons(port);
    SockAddr.sin_addr.s_addr = inet_addr(ip_address); 

    Bind(MasterSocket, (struct sockaddr *)(&SockAddr), sizeof(SockAddr));

    Listen(MasterSocket, SOMAXCONN); 
    
    while (1) {
        int SlaveSocket = Accept(MasterSocket, 0, 0);

        int Buffer[5] = {0, 0, 0, 0, 0};
        
        // unsigned int counter = 0;
        // while (counter < 4) {
        //     int res = recv(SlaveSocket, Buffer + 4 - counter, counter, MSG_NOSIGNAL);
        //     if (res > 0) counter += res;
        // }
        int res = recv(SlaveSocket, Buffer, 4, MSG_NOSIGNAL);
        
        send(SlaveSocket, Buffer, 4, MSG_NOSIGNAL);
        shutdown(SlaveSocket, SHUT_RDWR);
        close(SlaveSocket);
        printf("%s\n",  Buffer);
        
    }

    return EXIT_SUCCESS;
}
