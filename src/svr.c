#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include "anet.h"
#include "ae.h"
#include "request.h"

#define GET "get"
#define SET "set"

struct server{
	char *bindaddr;
	int port;
	int fd;

	aeEventLoop *el;

	char neterr[1024];
};

struct server _svr;

void _ReadHandler(aeEventLoop *el, int fd, void *privdata, int mask)
{
	char buf[1024];
	char sent_buf[1024];
	int nread;

	nread=read(fd,buf,1024);
	if(nread==-1)
		return;

	if(nread==0){
		return;
	}else{
		int ret;
		struct request *req=request_new(buf);
		ret=request_parse(req);
		if(ret){
			request_dump(req);
			if(strcmp(req->argv[0],GET)){
				/*to do nessDB's get*/
			}else if(strcmp(req->argv[0],SET)){
				/*to do nessDB's set*/
			}
			/*and others operations*/
		}
		request_free(req);
		
		sprintf(sent_buf,"%s\r\n","+OK");
		write(fd,sent_buf,strlen(sent_buf));
	}
}

void _AcceptHandler(aeEventLoop *el, int fd, void *privdata, int mask) 
{
	printf("accept:%d\n",fd);

	int cport, cfd;
    	char cip[128];
   	cfd = anetTcpAccept(_svr.neterr,fd,cip,&cport);
	if (cfd == AE_ERR) {
		printf("accept....\n");
		return;
	}

	aeCreateFileEvent(_svr.el,cfd,AE_READABLE,_ReadHandler,NULL);
}

int main()
{
	_svr.bindaddr="127.0.0.1";
	_svr.port=6379;
	
	_svr.el=aeCreateEventLoop();
	_svr.fd=anetTcpServer(_svr.neterr,_svr.port,_svr.bindaddr);

	//handler
 	if (aeCreateFileEvent(_svr.el, _svr.fd, AE_READABLE,_AcceptHandler, NULL) == AE_ERR) 
		printf("creating file event");

	aeMain(_svr.el);
	printf("oops,exit\n");
	aeDeleteEventLoop(_svr.el);
	return 1;
}
