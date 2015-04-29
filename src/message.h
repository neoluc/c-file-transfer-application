#ifndef MYFTP_MESSAGE
#define MYFTP_MESSAGE

#include <string.h>
#include "socket_helper.h"

struct message_s {
	unsigned char protocol[6];
	unsigned char type;
	unsigned char status;
	unsigned int length;
} __attribute__ ((packed));

extern const unsigned char MESSAGE_PROTOCOL[6];

extern const unsigned char MESSAGE_TYPE_OPEN_CONN_REQUEST;
extern const unsigned char MESSAGE_TYPE_OPEN_CONN_REPLY;
extern const unsigned char MESSAGE_TYPE_AUTH_REQUEST;
extern const unsigned char MESSAGE_TYPE_AUTH_REPLY;
extern const unsigned char MESSAGE_TYPE_LIST_REQUEST;
extern const unsigned char MESSAGE_TYPE_LIST_REPLY;
extern const unsigned char MESSAGE_TYPE_GET_REQUEST;
extern const unsigned char MESSAGE_TYPE_GET_REPLY;
extern const unsigned char MESSAGE_TYPE_PUT_REQUEST;
extern const unsigned char MESSAGE_TYPE_PUT_REPLY;
extern const unsigned char MESSAGE_TYPE_QUIT_REQUEST;
extern const unsigned char MESSAGE_TYPE_QUIT_REPLY;
extern const unsigned char MESSAGE_TYPE_FILE_DATA;

int msg_message_init(struct message_s *, unsigned char, unsigned char, unsigned int);
int msg_send_message(int, struct message_s *);
int msg_send_message_params(int, unsigned char, unsigned char, unsigned int);
int msg_recv_message(int, struct message_s *);
int msg_send_payload(int, char *, int);
int msg_recv_payload(int, char *, int);

#endif