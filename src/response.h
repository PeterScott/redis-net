#ifndef _RESPONSE_H
#define _RESPONSE_H
struct response{
	int argc;
	int status;
	char *ackbuf;
	char **argv;
};

struct response *response_new(int argc,int status);
void response_detch(struct response *response,char *ackbuf);
void response_dump(struct response *response);
void response_free(struct response *response);

#endif
