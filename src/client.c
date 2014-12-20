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

#include <common.h>

#include "cldd.h"
#include "client.h"
#include "error.h"

const char sendbuf[MAXLINE] =
        "012345678901234567890123456789012345678901234567890123456789012\n";

client *
client_new (void)
{
    client *c = malloc (sizeof (client));
    c->ntot = 0;
    c->nreq = 0;
    c->quit = false;
    return c;
}

bool
client_compare (const void * _a, const void * _b)
{
    const client *a = (const client *) _a;
    const client *b = (const client *) _b;

    if (a->fd == b->fd)
        return true;
    else
        return false;
}

void
client_free (void *a)
{
    client *c = (client *)a;
    free (c);
}
