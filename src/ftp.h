#ifndef MYFTP_FTP
#define MYFTP_FTP

#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "message.h"
#include <libgen.h>

enum FtpState {
	FTP_STATE_OPEN_CONN,
	FTP_STATE_AUTH,
	FTP_STATE_MAIN,
};

int ftp_get_full_path(char *, char *, char *);
int ftp_is_file_exist(char *);
int ftp_read_send_file(int, char *);
int ftp_recv_write_file(int, char *);

#endif