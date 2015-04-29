#ifndef MYFTP_FTP_CLIENT
#define MYFTP_FTP_CLIENT

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ftp.h"

enum FtpClientError {
	FTP_CLIENT_ERROR_INVALID_INPUT = -1,
	FTP_CLIENT_ERROR_OPEN_CONN = -2,
	FTP_CLIENT_ERROR_AUTH = -3,
	FTP_CLIENT_ERROR_LIST = -4,
	FTP_CLIENT_ERROR_GET = -5,
	FTP_CLIENT_ERROR_PUT = -6,
	FTP_CLIENT_ERROR_QUIT = -7,
};

int ftp_client_fd;
int ftp_client_state;

int ftp_client_initialize();
int ftp_client_connect_server(char *, char *);
int ftp_client_disconnect_server();
int ftp_client_action_open_conn();
int ftp_client_action_auth(char *, char *);
int ftp_client_action_list(char *, int);
int ftp_client_action_get(char *);
int ftp_client_action_put(char *);
int ftp_client_action_quit();
// int ftp_client_process(char **, int);

#endif