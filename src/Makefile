all: client server
client:
	colorgcc -o ../client/client ftp_client.h ftp_client.c main_client.c socket_helper.h socket_helper.c message.c message.h ftp.h ftp.c
server:
	colorgcc -pthread -o ../server/server ftp_server.h ftp_server.c main_server.c socket_helper.h socket_helper.c message.c message.h ftp.h ftp.c