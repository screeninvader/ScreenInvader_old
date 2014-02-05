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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "dri2util.h"

static Bool WireToEvent(Display *dpy, XExtDisplayInfo *info,
		XEvent *event, xEvent *wire)
{
	switch ((wire->u.u.type & 0x7f) - info->codes->first_event) {

	case DRI2_BufferSwapComplete:
	{
//		xDRI2BufferSwapComplete *awire = (xDRI2BufferSwapComplete *)wire;
		MSG("BufferSwapComplete");
		return True;
	}
	case DRI2_InvalidateBuffers:
	{
//		xDRI2InvalidateBuffers *awire = (xDRI2InvalidateBuffers *)wire;
		MSG("InvalidateBuffers");
//		dri2InvalidateBuffers(dpy, awire->drawable);
		return False;
	}
	default:
		/* client doesn't support server event */
		break;
	}

	return False;
}

static Status EventToWire(Display *dpy, XExtDisplayInfo *info,
		XEvent *event, xEvent *wire)
{
   switch (event->type) {
   default:
      /* client doesn't support server event */
      break;
   }

   return Success;
}

static const DRI2EventOps ops = {
		.WireToEvent = WireToEvent,
		.EventToWire = EventToWire,
};

int dri2_connect(Display *dpy, int driverType, char **driver)
{
	int eventBase, errorBase, major, minor;
	char *device;
	drm_magic_t magic;
	Window root;
	int fd;

	if (!DRI2InitDisplay(dpy, &ops)) {
		ERROR_MSG("DRI2InitDisplay failed");
		return -1;
	}

	if (!DRI2QueryExtension(dpy, &eventBase, &errorBase)) {
		ERROR_MSG("DRI2QueryExtension failed");
		return -1;
	}

	MSG("DRI2QueryExtension: eventBase=%d, errorBase=%d", eventBase, errorBase);

	if (!DRI2QueryVersion(dpy, &major, &minor)) {
		ERROR_MSG("DRI2QueryVersion failed");
		return -1;
	}

	MSG("DRI2QueryVersion: major=%d, minor=%d", major, minor);

	root = RootWindow(dpy, DefaultScreen(dpy));

	if (!DRI2Connect(dpy, root, driverType, driver, &device)) {
		ERROR_MSG("DRI2Connect failed");
		return -1;
	}

	MSG("DRI2Connect: driver=%s, device=%s", *driver, device);

	fd = open(device, O_RDWR);
	if (fd < 0) {
		ERROR_MSG("open failed");
		return fd;
	}

	if (drmGetMagic(fd, &magic)) {
		ERROR_MSG("drmGetMagic failed");
		return -1;
	}

	if (!DRI2Authenticate(dpy, root, magic)) {
		ERROR_MSG("DRI2Authenticate failed");
		return -1;
	}

	return fd;
}

#ifdef HAVE_NOUVEAU
extern Backend nouveau_backend;
#endif

#ifdef HAVE_OMAP
extern Backend omap_backend;
#endif

Backend * get_backend(const char *driver)
{
#ifdef HAVE_NOUVEAU
	if (!strcmp(driver, "nouveau")) {
		return &nouveau_backend;
	}
#endif

#ifdef HAVE_OMAP
	if (!strcmp(driver, "omap")) {
		return &omap_backend;
	}
#endif

	ERROR_MSG("no suitable backend DRM driver found");

	return NULL;
}
