#include "ftp.h"

int ftp_get_full_path(char *name, char *directory, char *path) {
	char *base_name = basename(name);
	char *directory_exist = realpath(directory, path);
	if (!directory_exist) {
		return -1;
	}
	strcat(path, "/");
	strcat(path, base_name);
	return 0;
}

int ftp_is_file_exist(char *path) {
	struct stat buf;
	if (stat(path, &buf) < 0) {
		return -1;
	}
	if (!S_ISREG(buf.st_mode)) {
		return -1;
	}
	return 0;
}

int ftp_read_send_file(int fd, char *path) {
	FILE *file = fopen(path, "rb");
	if (!file) {
		return -1;
	}
	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	if (msg_send_message_params(fd, MESSAGE_TYPE_FILE_DATA, 0, 12 + file_size) < 0) {
		fclose(file);
		return -1;
	}
	char *payload = malloc(file_size * sizeof(char));
	if (!payload) {
		fclose(file);
		return -1;
	}
	if (fread(payload, file_size, 1, file) != 1 && file_size != 0) {
		fclose(file);
		free(payload);
		return -1;
	}
	if (msg_send_payload(fd, payload, file_size) < 0) {
		fclose(file);
		free(payload);
		return -1;	
	}
	fclose(file);
	free(payload);
	return 0;
}

int ftp_recv_write_file(int fd, char *path) {
	struct message_s message;
	if (msg_recv_message(fd, &message) < 0 || message.type != MESSAGE_TYPE_FILE_DATA) {
		return -1;
	}
	int payload_size = message.length - 12;
	char *payload = malloc(payload_size * sizeof(char));
	if (!payload) {
		return -1;
	}
	if (msg_recv_payload(fd, payload, payload_size) < 0) {
		free(payload);
		return -1;
	}
	FILE *file = fopen(path, "wb");
	if (!file) {
		free(payload);
		return -1;
	}
	if (fwrite(payload, payload_size, 1, file) != 1 && payload_size != 0) {
		free(payload);
		fclose(file);
		return -1;
	}
	fclose(file);
	free(payload);
	return 0;
}