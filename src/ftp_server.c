#include "ftp_server.h"
#include <errno.h>
int ftp_server_initialize() {
	ftp_server_fd = -1;
	return 0;
}

int ftp_server_start(unsigned short port) {
	ftp_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (ftp_server_fd < 0) {
		return -1;
	}
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(struct sockaddr_in));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port);
	if (bind(ftp_server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
		return -1;
	}
	if (listen(ftp_server_fd, 1024) < 0) {
		return -1;
	}
	return 0;
}

int ftp_server_accept_connection(struct FtpSerConn *conn) {
	unsigned int address_length = sizeof(struct sockaddr_in);
	conn->fd = accept(ftp_server_fd, (struct sockaddr *)&(conn->address), &address_length);
	if (conn->fd < 0) {
		return -1;
	}
	return 0;
}

int ftp_server_close_connection(struct FtpSerConn *conn) {
	close(conn->fd);
	return 0;
}

int ftp_server_action_open_conn(struct FtpSerConn *conn, const struct message_s *message) {
	if (message->type != MESSAGE_TYPE_OPEN_CONN_REQUEST || message->length != 12) {
		return -1;
	}
	if (msg_send_message_params(conn->fd, MESSAGE_TYPE_OPEN_CONN_REPLY, 1, 12) < 0) {
		return -1;
	}
	return 0;
}

int ftp_server_action_auth(struct FtpSerConn *conn, const struct message_s *message, char *file_path) {
	if (message->type != MESSAGE_TYPE_AUTH_REQUEST) {
		return -1;
	}
	int payload_size = message->length - 12;
	char *payload = malloc(payload_size * sizeof(char));
	if (!payload) {
		return -1;
	}
	if (msg_recv_payload(conn->fd, payload, payload_size) < 0) {
		free(payload);
		return -1;
	}
	FILE *file = fopen(file_path, "r");
	if (!file) {
		free(payload);
		return -1;
	}
	char line[1024];
	int is_matched = 0;
	while (fgets(line, 1024, file)) {
		int line_size = strlen(line);
		if (line[line_size - 1] == '\n') {
			line[line_size - 1] = '\0';
			line_size--;
		}
		if (strcmp(line, payload) == 0) {
			is_matched = 1;
			break;
		}
	}
	if (msg_send_message_params(conn->fd, MESSAGE_TYPE_AUTH_REPLY, is_matched, 12) < 0) {
		fclose(file);
		free(payload);
		return -1;
	}
	fclose(file);
	free(payload);
	if (!is_matched) {
		return -1;
	}
	return 0;
}

int ftp_server_action_list(struct FtpSerConn *conn, const struct message_s *message, char *directory) {
	if (message->type != MESSAGE_TYPE_LIST_REQUEST || message->length != 12) {
		return -1;
	}
	DIR *dir = opendir(directory);
	if (!dir) {
		return -1;
	}
	char payload[1024] = {0};
	struct dirent *ent;
	while (ent = readdir(dir)) {
		if (ent->d_type == DT_REG) {
			strcat(payload, ent->d_name);
			strcat(payload, "\n");
		}
	}
	closedir(dir);
	int payload_size = strlen(payload) + 1;
	if (msg_send_message_params(conn->fd, MESSAGE_TYPE_LIST_REPLY, 0, 12 + payload_size) < 0) {
		return -1;
	}
	if (msg_send_payload(conn->fd, payload, payload_size) < 0) {
		return -1;
	}
	return 0;
}

int ftp_server_action_get(struct FtpSerConn *conn, const struct message_s *message, char *directory) {
	if (message->type != MESSAGE_TYPE_GET_REQUEST) {
		return -1;
	}
	int name_size = message->length - 12;
	char *name = malloc(name_size * sizeof(char));
	if (!name) {
		return -1;
	}
	if (msg_recv_payload(conn->fd, name, name_size) < 0) {
		free(name);
		return -1;
	}
	char path[512];
	if (ftp_get_full_path(name, directory, path) < 0) {
		free(name);
		return -1;
	}
	int is_file_exist = 1;
	if (ftp_is_file_exist(path) < 0) {
		is_file_exist = 0;
	}
	if (msg_send_message_params(conn->fd, MESSAGE_TYPE_GET_REPLY, is_file_exist, 12) < 0) {
		free(name);
		return -1;
	}
	if (!is_file_exist) {
		free(name);
		return -1;
	}
	if (ftp_read_send_file(conn->fd, path) < 0) {
		free(name);
		return -1;
	}
	free(name);
	return 0;
}

int ftp_server_action_put(struct FtpSerConn *conn, const struct message_s *message, char *directory) {
	if (message->type != MESSAGE_TYPE_PUT_REQUEST) {
		return -1;
	}
	int name_size = message->length - 12;
	char *name = malloc(name_size * sizeof(char));
	if (!name) {
		return -1;
	}
	if (msg_recv_payload(conn->fd, name, name_size) < 0) {
		free(name);
		return -1;
	}
	if (msg_send_message_params(conn->fd, MESSAGE_TYPE_PUT_REPLY, 0, 12) < 0) {
		free(name);
		return -1;
	}
	char path[512];
	if (ftp_get_full_path(name, directory, path) < 0) {
		free(name);
		return -1;
	}
	if (ftp_recv_write_file(conn->fd, path) < 0) {
		free(name);
		return -1;
	}
	free(name);
	return 0;
}

int ftp_server_action_quit(struct FtpSerConn *conn, const struct message_s *message) {
	if (message->type != MESSAGE_TYPE_QUIT_REQUEST || message->length != 12) {
		return -1;
	}
	if (msg_send_message_params(conn->fd, MESSAGE_TYPE_QUIT_REPLY, 0, 12) < 0) {
		return -1;
	}
	return 0;
}

int ftp_server_run(struct FtpSerConn *conn) {
	enum FtpState state = FTP_STATE_OPEN_CONN;
	int is_server_running = 1;
	while (is_server_running) {
		struct message_s message;
		if (msg_recv_message(conn->fd, &message) < 0) {
			continue;
		}
		switch (state) {
		case FTP_STATE_OPEN_CONN:
			if (message.type == MESSAGE_TYPE_OPEN_CONN_REQUEST) {
				if (ftp_server_action_open_conn(conn, &message) < 0) {
					break;
				}
				state = FTP_STATE_AUTH;
			}
			break;
		case FTP_STATE_AUTH:
			if (message.type == MESSAGE_TYPE_AUTH_REQUEST) {
				if (ftp_server_action_auth(conn, &message, "access.txt") < 0) {
					is_server_running = 0;
					break;
				}
				state = FTP_STATE_MAIN;
			}
			break;
		case FTP_STATE_MAIN:
			if (message.type == MESSAGE_TYPE_LIST_REQUEST) {
				if (ftp_server_action_list(conn, &message, "filedir") < 0) {
					break;
				}
			} else if (message.type == MESSAGE_TYPE_GET_REQUEST) {
				if (ftp_server_action_get(conn, &message, "filedir") < 0) {
					break;
				}
			} else if (message.type == MESSAGE_TYPE_PUT_REQUEST) {
				if (ftp_server_action_put(conn, &message, "filedir") < 0) {
					break;
				}
			} else if (message.type == MESSAGE_TYPE_QUIT_REQUEST) {
				if (ftp_server_action_quit(conn, &message) < 0) {
					break;
				}
				is_server_running = 0;
			}
			break;
		}
	}
	return 0;
}
