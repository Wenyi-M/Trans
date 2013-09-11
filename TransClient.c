/*
	功能：客户端，上传、下载文件
*/
#include "Trans.h"

void Upload(int fd , const char * filename)
{//向服务器上传文件
	int filefd = open(filename , O_RDONLY) ;
	if(filefd < 0) PrintErr("Open file error")

	char buffer[BUFSIZE] ;//格式为#标识#文件名#数据
	buffer[0] = '#' , buffer[1] = '1' , buffer[2] = '#' ;
	int filen = strlen(filename) ;
	strncpy(buffer + 3 , filename , filen) ;
	buffer[3+filen] = '\0' ;

	Print(buffer) ;

	Print("Upload Start ......") ;

	int ret ;
	buffer[3+filen] = '#' ;
	char * ptr = buffer + 4 + filen ;
	int ptrlen = BUFSIZE - (ptr - buffer) ;
	while((ret = Readn(filefd , ptr , ptrlen)) > 0)
	{
		Writen(fd , buffer , ptr + ret - buffer) ;
		ptr = buffer ;
		ptrlen = BUFSIZE ;
	}
	shutdown(fd , SHUT_WR) ;

	//输出服务器反馈信息
	ret = Readn(fd , buffer , BUFSIZE) ;
	buffer[ret] = '\0' ;
	Print(buffer) ;

	//shutdown(fd , SHUT_WR) ;
	Print("Upload End......") ;

	close(filefd) ;
}

void Download(int fd , const char *filename)
{//从服务器下载文件
	int filefd = creat(filename , S_IRUSR | S_IWUSR) ;
	if(filefd < 0) PrintErr("Create file error")

	char buffer[BUFSIZE] ;

	buffer[0] = '#' , buffer[1] = '2' , buffer[2] = '#' ;
	int filen = strlen(filename) ;
	strncpy(buffer + 3 , filename , filen) ;
	buffer[3+filen] = '#' , buffer[4+filen] = '\0' ;

	Print("Download Start......") ;
	Writen(fd , buffer , strlen(buffer)) ;//向服务器
	shutdown(fd , SHUT_WR) ;

	int ret ;
	while((ret = Readn(fd , buffer , BUFSIZE)) > 0)
	{
		Writen(filefd , buffer , ret) ;
		if(ret < BUFSIZE) break ;
	}

	//shutdown(fd , SHUT_WR) ;
	Print("Download End ......") ;

	close(filefd) ;
}
int main(int argc , char * argv[])
{
	if(argc < 2) PrintErr("Command args Error")

	struct sockaddr_in ser_addr ;
	
	struct _file {
		int opt ;	//存储不同的选项，1表示上传，-1表示下载，0表示退出
		char filename[MAXNAMLEN+1] ;
	} file ;
	
	while(1)
	{
		bzero(&file , sizeof(file)) ;
		char method[MAXNAMLEN] ;
		printf("Please Input : Upload 、Download with filename , type Exit will do you want :\n") ;
		scanf("%s", method) ;
		if(strcmp("Upload" , method) == 0)
			file.opt = 1 ;
		else if(strcmp("Download" , method) == 0)
			file.opt = 2 ;
		else if(strcmp("Exit" , method) == 0)
			break ;
		else 
		{
			Print("Wrong Command") ;
			if(!feof(stdin) && !ferror(stdin))
			{
				char c ;
				while((c = getchar()) != '\n' && c != EOF) ;
			}
			continue ;
		}
		
		scanf("%s" , file.filename) ;
		
		bzero(&ser_addr , sizeof(ser_addr)) ;
		ser_addr.sin_family = AF_INET ;
		inet_aton(argv[1] , &ser_addr.sin_addr) ;
		ser_addr.sin_port = htons(SERPORT) ;

		int sockfd = socket(AF_INET , SOCK_STREAM , 0) ;
		if(sockfd < 0) PrintErr("socket call error")
	
		pid_t child ;
		if((child = fork()) < 0)
			PrintErr("Fork error")
		else if(child == 0)
		{
			int ret = connect(sockfd , (struct sockaddr *) &ser_addr , sizeof(ser_addr)) ;
			if(ret < 0) PrintErr("connect call error")

			if(file.opt == 1) Upload(sockfd , file.filename) ;
			else if(file.opt == 2) Download(sockfd , file.filename) ;
			close(sockfd) ;
			exit(0) ;
		}
		
		close(sockfd) ;
		int status ;
		waitpid(child , &status , 0) ;
		if(!WIFEXITED(status))
			break ;
	}
	return 0 ;
}


