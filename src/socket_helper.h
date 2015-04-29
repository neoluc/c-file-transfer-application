#ifndef MYFTP_SOCKET_HELPER
#define MYFTP_SOCKET_HELPER

#include <sys/socket.h>

int sh_send(int, void *, size_t);
int sh_recv(int, void *, size_t);

#endif