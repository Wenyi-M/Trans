#include "Trans.h"

extern int errno ;

int Readn(int fd , void * buf , int len)
{//尽量从fd中读取len字节，返回实际读的字节数目
	Print("Enter Readn Func...") ;
	int ret ;
	int buflen = len ;//记录可用的缓存区
	char * ptr = (char *) buf ;
	while(buflen > 0)
	{//缓存区未满
		ret = read(fd , ptr , buflen) ;
		if(ret == -1)
		{
			if(errno == EINTR)
				continue ;
			else PrintErr("Readn read error")
		}
		printf("\tRead From %d , Need Read %d Byte , Actually Read %d \n" , fd , buflen , ret) ;
		//printf("buflen : %d , ret : %d\n" , buflen , ret) ;
		if(ret == 0) break ;//读取完毕
		ptr += ret ;
		buflen -= ret ;
	}
	Print("Leave Readn Func...\n") ;
	return len - buflen ;//返回值大于等于0，若返回值小于len则表示读取完毕
}

int Writen(int fd , void * buf , int len)
{//向fd中写入len字节，返回实际写入的字节，除非出错否则一定写完
	Print("Enter Writen Func...") ;
	int ret ;
	int buflen = len ;//记录未写字节数目
	char * ptr = (char *) buf ;
	while(buflen > 0)
	{//仍有未写字节
		ret = write(fd , ptr , buflen) ;
		if(ret == -1)
		{
			if(errno == EINTR)
				continue ;
			else PrintErr("Writen write error")
		}
		printf("\tWrite To %d , Need Write %d Byte , Actually Write %d \n" , fd , buflen , ret) ;
		//printf("buflen : %d , ret : %d\n" , buflen , ret) ;
		ptr += ret ;
		buflen -= ret ;
	}
	Print("Leave Writen Func...\n") ;
	return len - buflen ;//返回值大于等于0 ， 返回实际写的字节
}
