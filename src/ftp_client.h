#ifndef MYFTP_FTP_CLIENT
#define MYFTP_FTP_CLIENT

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ftp.h"

int ftp_client_fd;

int ftp_client_initialize();
int ftp_client_connect_server(char *, char *);
int ftp_client_disconnect_server();
int ftp_client_action_open_conn();
int ftp_client_action_auth(char *, char *);
int ftp_client_action_list(char *, int);
int ftp_client_action_get(char *);
int ftp_client_action_put(char *);
int ftp_client_action_quit();

#endif