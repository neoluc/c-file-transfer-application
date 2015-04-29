#include <pthread.h>
#include "ftp_server.h"

void *start_routine(void *arg) {
	struct FtpSerConn *conn = (struct FtpSerConn *)arg;
	ftp_server_run(conn);
	ftp_server_close_connection(conn);
	printf("Closed connection: %d\n", conn->fd);
	free(conn);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage:\n");
		printf("    app_name PORT_NUMBER\n");
		return 1;
	}
	ftp_server_initialize();
	if (ftp_server_start(atoi(argv[1])) < 0) {
		printf("ERROR: Fail to start server.\n");
		return 1;
	}
	printf("Server started. Waiting for connctions.\n");
	while (1) {
		struct FtpSerConn *conn = malloc(sizeof(struct FtpSerConn));
		if (ftp_server_accept_connection(conn) < 0) {
			printf("ERROR: Fail to accept a new connection.");
			free(conn);
			continue;
		}
		printf("Accepted new connection: %d\n", conn->fd);
		pthread_t thread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if (pthread_create(&thread, &attr, start_routine, conn) < 0) {
			printf("ERROR: Fail to create thread.");
			free(conn);
			continue;
		}
	}
	return 0;
}