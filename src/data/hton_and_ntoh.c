#include <stdint.h>
#include <stdio.h>

typedef unsigned char u8;

// network byte order = 大端
// host to network
uint16_t my_htons(uint16_t);
uint32_t my_htonl(uint32_t);

// network to host
uint16_t my_ntohs(uint16_t);
uint32_t my_ntohl(uint32_t);

void show_bytes(u8*, size_t);

int main() {
    uint32_t x = 0x12345678; // 小端存储

    show_bytes((u8*)&x, sizeof(uint32_t));

    uint32_t net = my_htonl(x); // 发送到网络
    show_bytes((u8*)&net, sizeof(uint32_t));

    uint32_t host = my_ntohl(net); // 从网络接收
    show_bytes((u8*)&host, sizeof(uint32_t));

    return 0;
}

uint16_t my_htons(uint16_t x) {
    return (x >> 8) | (x << 8);
}

uint32_t my_htonl(uint32_t x) {
    return ((x >> 24) & 0x000000FF) | ((x >> 8) & 0x0000FF00) | ((x << 8) & 0x00FF0000) | ((x << 24) & 0xFF000000);
}

uint16_t my_ntohs(uint16_t x) {
    return my_htons(x);
}

uint32_t my_ntohl(uint32_t x) {
    return my_htonl(x);
}

void show_bytes(u8* start, size_t len) {
    size_t i;
    for (i = 0; i < len; i++) {
        printf("%p\t0x%.2x\n", start + i, start[i]);
    }
    putchar('\n');
}