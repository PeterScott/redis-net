#ifndef _REQUEST_H
#define _REQUEST_H

struct request{
	char *querybuf;
	int argc;
	char **argv;
	size_t pos;
};

struct request *request_new(char *querybuf);
int  request_parse(struct request *request);
void request_free(struct request *request);
void request_dump(struct request *request);

#endif
