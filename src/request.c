/*This is a simple REDIS-PROTOCOL Parser using DFA(Deterministic finite-state machine,
  http://en.wikipedia.org/wiki/Deterministic_finite-state_machine)
  * nessDB will use this parser to support Redis-Protocol
  *
  * Copyright (c) 2011, BohuTANG <overred.shuttler at gmail dot com>
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  *   * Redistributions of source code must retain the above copyright notice,
  *     this list of conditions and the following disclaimer.
  *   * Redistributions in binary form must reproduce the above copyright
  *     notice, this list of conditions and the following disclaimer in the
  *     documentation and/or other materials provided with the distribution.
  *   * Neither the name of Redis nor the names of its contributors may be used
  *     to endorse or promote products derived from this software without
  *     specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  * POSSIBILITY OF SUCH DAMAGE.
  */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "request.h"

#define BUF_SIZE (1024*10)

enum {
    STATE_CONTINUE,
    STATE_FAIL
};

struct request *request_new(char *querybuf) {
    struct request *req;
    req=calloc(1, sizeof(struct request));
    req->querybuf = querybuf;
    return req;
}


int req_state_len(struct request *req,char *sb) {
    int term=0, first=0;
    char c;
    int i = req->pos;
    int pos=i;

    while((c = req->querybuf[i]) != '\0') {
        first++;
        pos++;
        switch(c) {
        case '\r':
            term = 1;
            break;
				  
        case '\n':
            if (term) {
                req->pos = pos;
                return STATE_CONTINUE;
            }
            else
                return STATE_FAIL;
        default:
            if (first == 1) {
                /* the first symbol is not '*' or '$'*/
                if (c != '$' && c != '*' && c != '\r' && c != '\n')
                    return STATE_FAIL;
            } else {
                /* the symbol must be numeral*/
                if (c >= '0' && c <= '9')
                    *sb++ = c;
                else
                    return STATE_FAIL;
            }
            break;
        }
        i++;
    }

    return STATE_FAIL;
}


int request_parse(struct request *req) {
    int i;
    char sb[BUF_SIZE] = {0};

    if (req_state_len(req, sb) != STATE_CONTINUE) {
        fprintf(stderr, "argc format ***ERROR***,packet:%s", sb);
        return 0;
    }
    req->argc = atoi(sb);

    req->argv  =(char**)calloc(req->argc, sizeof(char*));
    for (i = 0; i < req->argc; i++) {
        int argv_len;
        char *v;

        /*parse argv len*/
        memset(sb, 0, BUF_SIZE);
        if (req_state_len(req, sb) != STATE_CONTINUE) {
            fprintf(stderr, "argv's length format ***ERROR***,packet:%s", sb);
            return 0;
        }
        argv_len=atoi(sb);

        /*get argv*/
        v=(char*)malloc(sizeof(char) * argv_len);
        memcpy(v, req->querybuf+(req->pos), argv_len);
        req->argv[i] = v;	
        req->pos += argv_len+2;
    }
    return 1;
}

void request_dump(struct request *req) {
    int i;
    if (req == NULL)
        return;

    printf("request-dump--->");
    printf("argc:<%d>\n", req->argc);
    for (i = 0; i < req->argc; i++) {
        printf("		argv[%d]:<%s>\n",i,req->argv[i]);
    }
    printf("\n");
}

void request_free(struct request *req) {
    int i;
    if (req) {
        for (i = 0; i < req->argc; i++) {
            if (req->argv[i])
                free(req->argv[i]);
        }
        free(req->argv);
        free(req);
    }
}
