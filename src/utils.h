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

#ifndef _CLDD_UTILS_H
#define _CLDD_UTILS_H

#include "common.h"

BEGIN_C_DECLS

/**
 * Applies the O_NONBLOCK flag to the descriptor using fcntl.
 *
 * @param fd The file descriptor to set non blocking
 * @return   Passes the return value given by fcntl
 */
int set_nonblocking (int fd);

END_C_DECLS

#endif
