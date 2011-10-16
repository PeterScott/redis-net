#include <stdio.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include "ae.h"
#include "anet.h"

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
	int nread;

	nread=read(fd,buf,1024);
	printf("read buf:%s\n",buf);
}

void _AcceptHandler(aeEventLoop *el, int fd, void *privdata, int mask) 
{
	printf("accept:%s\n",_svr.neterr);

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
	_svr.port=8080;
	
	_svr.el=aeCreateEventLoop();
	_svr.fd=anetTcpServer(_svr.neterr,_svr.port,_svr.bindaddr);

	//handler
 	if (aeCreateFileEvent(_svr.el, _svr.fd, AE_READABLE,_AcceptHandler, NULL) == AE_ERR) 
		printf("creating file event");

	aeMain(_svr.el);
	aeDeleteEventLoop(_svr.el);
	return 1;
}
