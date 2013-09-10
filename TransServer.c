/*
	名称：并发服务器
	功能：上传和下载，支持多个客户端同时登录
*/
#include "Trans.h"

void Upload(int fd , const char * filename , char * buffer , int pos)
{//向服务器上传文件
	int filefd = creat(filename , S_IRUSR | S_IWUSR) ;
	if(filefd < 0) PrintErr("Create file error")

	Print("Upload Start......") ;

	char * ptr = buffer + pos + 1 ;
	int ptrlen = BUFSIZE - (ptr - buffer) ;
	Writen(filefd , ptr , ptrlen) ;

	int ret ;
	while((ret = Readn(fd , buffer , BUFSIZE)) > 0)
	{
		Writen(filefd , buffer , ret) ;
		if(ret < BUFSIZE) break ;
	}
	//shutdown(fd , SHUT_WR) ;

	char res[] = "Upload Succeffully" ;
	Writen(fd , res , strlen(res)) ;
	shutdown(fd , SHUT_WR) ;

	Print("Upload End......") ;
	close(filefd) ;
}

void Download(int fd , const char *filename , char * buffer , int pos)
{//从服务器下载文件
	int filefd = open(filename , O_RDONLY) ;
	if(filefd < 0) PrintErr("Open file error")

	Print("Download Start......") ;
	//shutdown(fd , SHUT_WR) ;
	int ret ;
	while((ret = Readn(filefd , buffer , BUFSIZE)) > 0)
	{
		Writen(fd , buffer , ret) ;
		if(ret < BUFSIZE) break ;
	}

	shutdown(fd , SHUT_WR) ;

	Print("Download End......") ;
	close(filefd) ;
}

/*int FileNameRead(int fd , void * buf , int len)
{//确定请求方式和文件名，将其写入buf中
	int ret ;
	int buflen = len ;
	char * ptr = (char *) buf ;
	while((ret = Readn(fd , ptr , buflen)) > 0)
	{
		ptr += ret ;
		buflen -= ret ;
		if(ret < buflen) break ;//已经读取完毕
	}
	return len - buflen ;//返回实际读取的长度
}*/

int main(int argc , char * argv[])
{
	struct sockaddr_in ser_addr ;

	bzero(&ser_addr , sizeof(ser_addr)) ;
	ser_addr.sin_family = AF_INET ;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY) ;
	ser_addr.sin_port = htons(SERPORT) ;

	int lisfd = socket(AF_INET , SOCK_STREAM , 0) ;
	if(lisfd < 0) PrintErr("socket call error")

	int ret = bind(lisfd , (struct sockaddr *) &ser_addr , sizeof(ser_addr)) ;
	if(ret < 0) PrintErr("bind call error")

	ret = listen(lisfd , LISTENLEN) ;
	if(ret < 0) PrintErr("listen call error")

	while(1)
	{
		struct sockaddr_in cli_addr ;
		socklen_t clilen = sizeof(cli_addr) ;
		bzero(&cli_addr , clilen) ;

		int connfd = accept(lisfd , (struct sockaddr *) &cli_addr , &clilen) ;
		if(connfd < 0) PrintErr("accept call error")

		char clinfo[99] = {0} ;//打印连接的客户端的IP地址
		puts("Connection From :") ;
		puts(inet_ntop(AF_INET , &cli_addr.sin_addr , clinfo , sizeof(clinfo))) ;

		pid_t child_pid = fork() ;//并发
		if(child_pid < 0) PrintErr("fork call error")
		else if(child_pid == 0)
		{//子进程
			close(lisfd) ;//子进程关闭监听套接字

			struct _file {
				int opt ;	//存储不同的选项，1表示上传，-1表示下载，0表示退出
				char filename[MAXNAMLEN+1] ;
			} file ;
			char buffer[BUFSIZE] ;

			//有三种不同的请求：上传、下载、退出
			bzero(&file , sizeof(file)) ;

			Readn(connfd , buffer , BUFSIZE) ;
			int i = 0 , j = 0 ;
			if(buffer[i] != '#') Print("Wrong Format") ;
			file.opt = buffer[++i] - '0' ;
			if(buffer[++i] != '#') Print("Wrong Format") ;
			while(buffer[++i] != '#')
			  file.filename[j++] =  buffer[i] ;
			file.filename[i] = '\0' ;

			Print(file.filename) ;

			switch(file.opt)
			{
				case 1  :
						Upload(connfd , file.filename , buffer , i) ;
						break ;
				case 2 :
						Download(connfd , file.filename , buffer , i) ;
						break ;
				default :
						PrintErr("No Input , Time to exit")
			}
			close(connfd) ;
			exit(0) ;
		}
		else close(connfd) ;//关闭父进程的连接套接字
	}
	return 0 ;
}
