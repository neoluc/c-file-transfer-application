#ifndef MYFTP_FTP_SERVER
#define MYFTP_FTP_SERVER

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libgen.h>
#include "ftp.h"

struct FtpSerConn {
	int fd;
	struct sockaddr_in address;
};

int ftp_server_fd;

int ftp_server_initialize();
int ftp_server_start(unsigned short);
int ftp_server_accept_connection(struct FtpSerConn *);
int ftp_server_close_connection(struct FtpSerConn *);
int ftp_server_action_open_conn(struct FtpSerConn *, const struct message_s *);
int ftp_server_action_auth(struct FtpSerConn *, const struct message_s *, char *);
int ftp_server_action_list(struct FtpSerConn *, const struct message_s *, char *);
int ftp_server_action_get(struct FtpSerConn *, const struct message_s *, char *);
int ftp_server_action_put(struct FtpSerConn *, const struct message_s *, char *);
int ftp_server_action_quit(struct FtpSerConn *, const struct message_s *);
int ftp_server_run(struct FtpSerConn *);

#endif