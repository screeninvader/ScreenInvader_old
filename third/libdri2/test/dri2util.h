/*
 * Copyright © 2011 Texas Instruments, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Soft-
 * ware"), to deal in the Software without restriction, including without
 * limitation the rights to use, copy, modify, merge, publish, distribute,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, provided that the above copyright
 * notice(s) and this permission notice appear in all copies of the Soft-
 * ware and that both the above copyright notice(s) and this permission
 * notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABIL-
 * ITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN
 * THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSE-
 * QUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFOR-
 * MANCE OF THIS SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization of
 * the copyright holder.
 *
 * Authors:
 *   Rob Clark (rob@ti.com)
 */

#ifndef _DRI2TEST_H_
#define _DRI2TEST_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <xf86drm.h>
#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/extensions/dri2proto.h>
#include "X11/extensions/dri2.h"

#define MSG(fmt, ...) \
		do { fprintf(stderr, fmt "\n", ##__VA_ARGS__); } while (0)
#define ERROR_MSG(fmt, ...) \
		do { fprintf(stderr, "ERROR: " fmt "\n", ##__VA_ARGS__); } while (0)

static inline void print_hex(int len, const unsigned char *val)
{
	char buf[33];
	int i, j;
	for (i = 0; i < len; i += j) {
		for (j = 0; (j < 16) && ((i + j) < len); ++j)
			sprintf(&buf[j * 2], "%02x", val[i + j]);
		fprintf(stderr, "\t%s\n", buf);
	}
}


typedef struct {
	DRI2Buffer *dri2buf;
	void *hdl;
} Buffer;

typedef struct {
	void   (*setup)(int fd);
	void * (*init)(DRI2Buffer *dri2buf);
	char * (*prep)(void *hdl);
	void   (*fini)(void *hdl);
} Backend;

int dri2_connect(Display *dpy, int driverType, char **driver);
Backend * get_backend(const char *driver);

#endif /* _DRI2TEST_H_ */
