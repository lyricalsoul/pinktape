#pragma once
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void free_buffer(const char* uh) {
    free((char *)uh);
}

int build_tcp_socket() {
    auto protoent = getprotobyname("tcp");
    if (protoent == NULL) return -1;
    auto sock = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    return sock;
}

Jakt::Optional<sockaddr_in> get_sock_addr(const char* hostname, unsigned short port) {
    auto serv = gethostbyname(hostname);
    if (serv == NULL) return JaktInternal::NullOptional {};
    auto in_addr = inet_addr(inet_ntoa(*(struct in_addr*) serv->h_addr));
    if (in_addr == (in_addr_t) -1) return JaktInternal::NullOptional {};
    struct sockaddr_in sockaddr_in;
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(port);
    sockaddr_in.sin_addr.s_addr = in_addr;
    return sockaddr_in;
}

bool connect_socket(int sock, sockaddr_in addr) {
    if (connect(sock, (sockaddr*) &addr, sizeof(addr)) == -1) return false;
    return true;
}

int send_request(int sock, const char* request) {
    auto bytes_total = 0;
    auto request_len = strlen(request);
    while (bytes_total < request_len) {
        auto bytes_sent = write(sock, request + bytes_total, request_len - bytes_total);
        if (bytes_sent == -1) return -1;
        bytes_total += bytes_sent;
    }

    return bytes_total;
}

Jakt::Optional<char*> read_request(int sock) {
    auto size = sizeof(char) * 1024;
    auto buffer = (char*) malloc(size+1);
    // FIXME: make this allocate as much memory as it needs (based on some kind of var or something so to avoid OOM issues)
    int bytes_total = read(sock, buffer, size);
    if (bytes_total >= size) {
        auto newSize = size * 2;
        buffer = (char*) realloc(buffer, newSize+1);
        auto nbytes_total = read(sock, buffer + bytes_total, newSize - size);
        if (nbytes_total == newSize - size) printf("already reallocated once, not doing it again\n");
    }

    if (bytes_total < 1) {
        free(buffer);
        return JaktInternal::NullOptional {};
    }

    return buffer;
}

void close_socket(int sock) {
    close(sock);
}