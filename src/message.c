#include "message.h"

extern const unsigned char MESSAGE_PROTOCOL[6] = {0xe3, 'm', 'y', 'f', 't', 'p'};

extern const unsigned char MESSAGE_TYPE_OPEN_CONN_REQUEST = 0xA1;
extern const unsigned char MESSAGE_TYPE_OPEN_CONN_REPLY = 0xA2;
extern const unsigned char MESSAGE_TYPE_AUTH_REQUEST = 0xA3;
extern const unsigned char MESSAGE_TYPE_AUTH_REPLY = 0xA4;
extern const unsigned char MESSAGE_TYPE_LIST_REQUEST = 0xA5;
extern const unsigned char MESSAGE_TYPE_LIST_REPLY = 0xA6;
extern const unsigned char MESSAGE_TYPE_GET_REQUEST = 0xA7;
extern const unsigned char MESSAGE_TYPE_GET_REPLY = 0xA8;
extern const unsigned char MESSAGE_TYPE_PUT_REQUEST = 0xA9;
extern const unsigned char MESSAGE_TYPE_PUT_REPLY = 0xAA;
extern const unsigned char MESSAGE_TYPE_QUIT_REQUEST = 0xAB;
extern const unsigned char MESSAGE_TYPE_QUIT_REPLY = 0xAC;
extern const unsigned char MESSAGE_TYPE_FILE_DATA = 0xFF;

int msg_message_init(struct message_s *message, unsigned char type, unsigned char status, unsigned int length) {
	memcpy(message->protocol, MESSAGE_PROTOCOL, 6);
	message->type = type;
	message->length = length;
	message->status = status;
	return 0;
}

int msg_send_message(int fd, struct message_s *message) {
	if (sh_send(fd, message, sizeof(struct message_s)) < 0) {
		return -1;
	}
	return 0;
}

int msg_send_message_params(int fd, unsigned char type, unsigned char status, unsigned int length) {
	struct message_s message;
	msg_message_init(&message, type, status, length);
	return msg_send_message(fd, &message);
}

int msg_recv_message(int fd, struct message_s *message) {
	if (sh_recv(fd, message, sizeof(struct message_s)) < 0) {
		return -1;
	}
	if (memcmp(message->protocol, MESSAGE_PROTOCOL, 6) != 0) {
		return -1;
	}
	return 0;
}

int msg_send_payload(int fd, char *payload, int payload_size) {
	if (sh_send(fd, payload, payload_size) < 0) {
		return -1;
	}
	return 0;
}

int msg_recv_payload(int fd, char *payload, int payload_size) {
	if (sh_recv(fd, payload, payload_size) < 0) {
		return -1;
	}
	return 0;
}
