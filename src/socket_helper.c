#include "socket_helper.h"

int sh_send(int fd, void *buffer, size_t buffer_size) {
	size_t total = 0;
	while (total < buffer_size) {
		size_t remain = buffer_size - total;
		int sent = send(fd, buffer, remain, 0);
		if (sent < 0) {
			return -1;
		}
		buffer = (char *)buffer + sent;
		total += sent;
	}
	return total;
}

int sh_recv(int fd, void *buffer, size_t buffer_size) {
	size_t total = 0;
	while (total < buffer_size) {
		size_t remain = buffer_size - total;
		int received = recv(fd, buffer, remain, 0);
		if (received < 0) {
			return -1;
		}
		buffer = (char *)buffer + received;
		total += received;
	}
	return total;
}