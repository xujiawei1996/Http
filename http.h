#ifndef _HTTPD_H_
#define _HTTPD_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<pthread.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include <sys/types.h>      
#include <sys/stat.h>
#include <fcntl.h>
#include<string.h>
#include<unistd.h>


#define SIZE 1024

#define NOTICE 0
#define WARNING 1
#define FATAL 2

int startup(char* ip,int port);
void *handler(void * arg);
void echo_error(int fd,int errno_num);
int echo_www(int fd,const char*path,int size);
int exe_cgi(int fd,const char *method,\
			const char* path,const char*query_string);


#endif
