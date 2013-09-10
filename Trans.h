#ifndef TRANS_H
#define TRANS_H

#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024
#define LISTENLEN 20
#define SERPORT 9006
#define MAXNAMLEN 99

#define PrintErr(x) { \
	printf("Error : %s !\n" , (x)) ; \
	exit(-1) ; \
}

#define Print(x) printf("%s\n" , (x))

int Writen(int fd , void * buf , int len) ;
int Readn(int fd , void * buf , int len) ;
#endif // TRANS_H
