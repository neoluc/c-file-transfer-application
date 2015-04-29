#include <string.h>
#include "ftp_client.h"

void print_usage() {
	printf("usage:\n");
	printf("    open [IP address] [port number]: send the OPEN CONN REQUEST message.\n");
	printf("    auth [username] [password]: send the AUTH REQUEST message.\n");
	printf("    ls: send the LIST REQUEST message.\n");
	printf("    get [filename]: start the download procedures.\n");
	printf("    put [filename]: start the upload procedures.\n");
	printf("    quit: send the QUIT REQUEST message.\n");
}

int main() {
	print_usage();
	ftp_client_initialize();
	enum FtpState state = FTP_STATE_OPEN_CONN;
	while (1) {
		printf("Client> ");
		char input[1024];
		if (!fgets(input, 1024, stdin)) {
			continue;
		}
		int input_size = strlen(input);
		if (input[input_size - 1] == '\n') {
			input[input_size - 1] = '\0';
			input_size--;
		}
		char *tokens[256];
		int tokens_size = 0;
		char *temp = strtok(input, " ");
		while (temp) {
			tokens[tokens_size++] = temp;
			temp = strtok(NULL, " ");
		}
		switch (state) {
		case FTP_STATE_OPEN_CONN:
			if (tokens_size == 3 && strcmp(tokens[0], "open") == 0) {
				if (ftp_client_connect_server(tokens[1], tokens[2])) {
					printf("ERROR: Cannot connect to server.\n");
					break;
				}
				if (ftp_client_action_open_conn() < 0) {
					printf("ERROR: Open connection rejected.\n");
					break;
				}
				printf("Server connection accepted.\n");
				state = FTP_STATE_AUTH;
				break;
			} else {
				printf("ERROR: Invalid input.\n");
			}
			break;
		case FTP_STATE_AUTH:
			if (tokens_size == 3 && strcmp(tokens[0], "auth") == 0) {
				if (ftp_client_action_auth(tokens[1], tokens[2]) < 0) {
					printf("ERROR: Authentication rejected. Connection closed.\n");
					ftp_client_disconnect_server();
					state = FTP_STATE_OPEN_CONN;
					break;
				}
				printf("Authentication granted.\n");
				state = FTP_STATE_MAIN;
				break;
			} else {
				printf("ERROR: Invalid input.\n");
			}
			break;
		case FTP_STATE_MAIN:
			if (tokens_size == 1 && strcmp(tokens[0], "ls") == 0) {
				char result[1024];
				if (ftp_client_action_list(result, 1024) < 0) {
					printf("ERROR: List files rejected.\n");
					break;
				}
				printf("----- file list start -----\n");
				printf("%s", result);
				printf("----- file list end -----\n");
				break;
			} else if (tokens_size == 2 && strcmp(tokens[0], "get") == 0) {
				if (ftp_client_action_get(tokens[1]) < 0) {
					printf("ERROR: Download file rejected.\n");
					break;
				}
				printf("File downloaded.\n");
				break;
			} else if (tokens_size == 2 && strcmp(tokens[0], "put") == 0) {
				if (ftp_client_action_put(tokens[1]) < 0) {
					printf("ERROR: Upload file rejected.\n");
					break;
				}
				printf("File uploaded.\n");
				break;
			} else if (tokens_size == 1 && strcmp(tokens[0], "quit") == 0) {
				if (ftp_client_action_quit() < 0) {
					printf("ERROR: Quit rejected.\n");
					break;
				}
				printf("Thank you.\n");
				state = FTP_STATE_OPEN_CONN;
				break;
			} else {
				printf("ERROR: Invalid input.\n");
			}
			break;
		}
	}
	return 0;
}