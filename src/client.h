/**
 * Copyright (C) 2010 Geoff Johnson <geoff.jay@gmail.com>
 *
 * This file is part of cldd.
 *
 * cldd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _CLDD_CLIENT_H
#define _CLDD_CLIENT_H

#include "common.h"

BEGIN_C_DECLS

#include "cldd.h"

extern const char sendbuf[MAXLINE];

typedef struct _client client;

struct _client {
    int fd_mgmt;
    int fd_strm;
//    struct sockaddr_in sa;
    struct sockaddr sa;
    socklen_t sa_len;
    char hbuf[NI_MAXHOST];
    char sbuf[NI_MAXSERV];
    /* for stats logging */
    int nreq;
    int ntot;
    bool quit;
};

client * client_new (void);
void client_process_cmd (client *c);
bool client_compare (const void * _a, const void * _b);
void client_free (void *a);

END_C_DECLS

#endif
