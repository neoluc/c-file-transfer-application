#include "ftp_client.h"

int ftp_client_initialize() {
	ftp_client_fd = -1;
	return 0;
}

int ftp_client_connect_server(char *ip, char *port) {
	ftp_client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (ftp_client_fd < 0) {
		return -1;
	}
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(ip);
	server_address.sin_port = htons(atoi(port));
	if (connect(ftp_client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
		return -1;
	}
	return 0;
}

int ftp_client_disconnect_server() {
	close(ftp_client_fd);
	return 0;
}

int ftp_client_action_open_conn() {
	if (msg_send_message_params(ftp_client_fd, MESSAGE_TYPE_OPEN_CONN_REQUEST, 0, 12) < 0) {
		return -1;
	}
	struct message_s message;
	if (msg_recv_message(ftp_client_fd, &message) < 0 || message.type != MESSAGE_TYPE_OPEN_CONN_REPLY || message.status != 1 || message.length != 12) {
		return -1;
	}
	return 0;
}

int ftp_client_action_auth(char *username, char *password) {
	int username_size = strlen(username);
	int password_size = strlen(password);
	int payload_size = username_size + 1 + password_size + 1;
	char *payload = malloc(payload_size * sizeof(char));
	strcat(payload, username);
	strcat(payload, " ");
	strcat(payload, password);
	if (msg_send_message_params(ftp_client_fd, MESSAGE_TYPE_AUTH_REQUEST, 0, 12 + payload_size) < 0) {
		free(payload);
		return -1;
	}
	if (msg_send_payload(ftp_client_fd, payload, payload_size) < 0) {
		free(payload);
		return -1;
	}
	struct message_s message;
	if (msg_recv_message(ftp_client_fd, &message) < 0 || message.type != MESSAGE_TYPE_AUTH_REPLY || message.status != 1 || message.length != 12) {
		free(payload);
		return -1;
	}
	free(payload);
	return 0;
}

int ftp_client_action_list(char result[], int result_size) {
	if (msg_send_message_params(ftp_client_fd, MESSAGE_TYPE_LIST_REQUEST, 0, 12) < 0) {
		return -1;
	}
	struct message_s message;	
	if (msg_recv_message(ftp_client_fd, &message) < 0) {
		return -1;
	}
	if (message.type != MESSAGE_TYPE_LIST_REPLY) {
		return -1;
	}
	int payload_size = message.length - 12;
	char *payload = malloc(payload_size * sizeof(char));
	if (!payload) {
		return -1;
	}
	if (msg_recv_payload(ftp_client_fd, payload, payload_size) < 0) {
		free(payload);
		return -1;
	}
	if (payload_size > result_size) {
		free(payload);
		return -1;
	}
	strcpy(result, payload);
	free(payload);
	return 0;
}

int ftp_client_action_get(char *name) {
	int name_size = strlen(name);
	if (msg_send_message_params(ftp_client_fd, MESSAGE_TYPE_GET_REQUEST, 0, 12 + name_size + 1) < 0) {
		return -1;
	}
	if (msg_send_payload(ftp_client_fd, name, name_size + 1) < 0) {
		return -1;
	}
	struct message_s message;
	if (msg_recv_message(ftp_client_fd, &message) < 0 || message.type != MESSAGE_TYPE_GET_REPLY || message.status != 1 || message.length != 12) {
		return -1;
	}
	char path[512];
	if (ftp_get_full_path(name, ".", path) < 0) {
		return -1;
	}
	if (ftp_recv_write_file(ftp_client_fd, path) < 0) {
		return -1;
	}
	return 0;
}

int ftp_client_action_put(char *name) {
	int name_size = strlen(name);
	char path[512];
	if (ftp_get_full_path(name, ".", path) < 0) {
		return -1;
	}
	if (ftp_is_file_exist(path) < 0) {
		return -1;
	}
	if (msg_send_message_params(ftp_client_fd, MESSAGE_TYPE_PUT_REQUEST, 0, 12 + name_size + 1) < 0) {
		return -1;
	}
	if (msg_send_payload(ftp_client_fd, name, name_size + 1) < 0) {
		return -1;
	}
	struct message_s message;
	if (msg_recv_message(ftp_client_fd, &message) < 0 || message.type != MESSAGE_TYPE_PUT_REPLY || message.length != 12) {
		return -1;
	}
	if (ftp_read_send_file(ftp_client_fd, path) < 0) {
		return -1;
	}
	return 0;
}

int ftp_client_action_quit() {
	if (msg_send_message_params(ftp_client_fd, MESSAGE_TYPE_QUIT_REQUEST, 0, 12) < 0) {
		return -1;
	}
	struct message_s message;
	if (msg_recv_message(ftp_client_fd, &message) < 0 || message.type != MESSAGE_TYPE_QUIT_REPLY || message.length != 12) {
		return -1;
	}
	return 0;
}