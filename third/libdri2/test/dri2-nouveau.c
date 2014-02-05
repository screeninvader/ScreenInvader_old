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

#include <nouveau_bo.h>
#include <nouveau_drmif.h>

struct nouveau_device *dev;

static void setup(int fd)
{
	int ret = nouveau_device_open_existing(&dev, 0, fd, 0);
	if (ret) {
		ERROR_MSG("nouveau_device_open_existing failed: %d", ret);
	}
}

static void * init(DRI2Buffer *dri2buf)
{
	struct nouveau_bo *bo = NULL;
	int ret = nouveau_bo_handle_ref(dev, dri2buf->names[0], &bo);
	if (ret) {
		ERROR_MSG("nouveau_bo_handle_ref failed: %d", ret);
		return NULL;
	}
	return bo;
}

static char * prep(void *hdl)
{
	struct nouveau_bo *bo = hdl;
	int ret = nouveau_bo_map(bo, NOUVEAU_BO_RDWR);
	if (ret) {
		ERROR_MSG("nouveau_bo_map failed: %d", ret);
		return NULL;
	}
	return bo->map;
}

static void fini(void *hdl)
{
	struct nouveau_bo *bo = hdl;
	nouveau_bo_unmap(bo);
}

Backend nouveau_backend = {
		.setup = setup,
		.init = init,
		.prep = prep,
		.fini = fini,
};
