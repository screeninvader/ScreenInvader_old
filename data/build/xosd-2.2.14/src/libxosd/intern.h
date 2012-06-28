#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#ifndef timerclear /* {{{ */
#define       timerisset(tvp)\
	((tvp)->tv_sec || (tvp)->tv_usec)
#define       timercmp(tvp, uvp, cmp)\
	((tvp)->tv_sec cmp (uvp)->tv_sec ||\
	 ((tvp)->tv_sec == (uvp)->tv_sec &&\
	 (tvp)->tv_usec cmp (uvp)->tv_usec))
#define       timerclear(tvp)\
	((tvp)->tv_sec = (tvp)->tv_usec = 0)
#endif /* }}} */
#include <sys/select.h>

#include <assert.h>
#include <pthread.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>
#ifdef HAVE_XINERAMA
#  include <X11/extensions/Xinerama.h>
#endif

#include "xosd.h"

/* gcc -O2 optimizes debugging away if Dnone is chosen. {{{ */
static const enum DEBUG_LEVEL {
  Dnone = 0,          /* Nothing */
  Dfunction = (1<<0), /* Function enter/exit */
  Dlocking = (1<<1),  /* Locking */
  Dselect = (1<<2),   /* select() processing */
  Dtrace = (1<<3),    /* Programm progess */
  Dvalue = (1<<4),    /* Computed values */
  Dupdate = (1<<5),   /* Display update processing */
  Dall = -1           /* Everything */
} _xosd_debug_level = Dnone;
#define DEBUG(lvl, fmt, ...) \
  do { \
    if (_xosd_debug_level & lvl) \
      fprintf (stderr, "%s:%-4d %ld@%s: " fmt "\n", __FILE__, __LINE__, \
          pthread_self(), __PRETTY_FUNCTION__ ,## __VA_ARGS__); \
  } while (0)
#define FUNCTION_START(lvl) \
  do { \
    if (_xosd_debug_level & Dfunction && _xosd_debug_level & lvl) \
      fprintf (stderr, "%s:%-4d %ld<%s\n", __FILE__, __LINE__, \
          pthread_self(), __PRETTY_FUNCTION__); \
  } while (0)
#define FUNCTION_END(lvl) \
  do { \
    if (_xosd_debug_level & Dfunction && _xosd_debug_level & lvl) \
      fprintf (stderr, "%s:%-4d %ld>%s\n",  __FILE__, __LINE__, \
          pthread_self(), __PRETTY_FUNCTION__); \
  } while (0)
/* }}} */

enum LINE { LINE_blank, LINE_text, LINE_percentage, LINE_slider };
union xosd_line
{
  enum LINE type;
  struct xosd_text {
    enum LINE type;
    int width;
    char *string;
  } text;
  struct xosd_bar {
    enum LINE type;
    int value;
  } bar;
};

struct xosd
{
  pthread_t event_thread;       /* CONST handles X events */

  pthread_mutex_t mutex;        /* CONST serialize X11 and structure */
  pthread_cond_t cond_wait;     /* CONST signal X11 done */
  int pipefd[2];                /* CONST signal X11 needed */

  pthread_mutex_t mutex_sync;   /* CONST mutual exclusion event notify */
  pthread_cond_t cond_sync;     /* CONST signal events */

  Display *display;             /* CONST x11 */
  int screen;                   /* CONST x11 */
  Window window;                /* CONST x11 */
  unsigned int depth;           /* CONST x11 */
  Pixmap mask_bitmap;           /* CACHE (font,offset) XShape mask */
  Pixmap line_bitmap;           /* CACHE (font,offset) offscreen bitmap */
  Visual *visual;               /* CONST x11 */

  XFontSet fontset;             /* CACHE (font) */
  XRectangle *extent;           /* CACHE (font) */

  GC gc;                        /* CONST x11 */
  GC mask_gc;                   /* CONST x11 white on black to set XShape mask */
  GC mask_gc_back;              /* CONST x11 black on white to clear XShape mask */

  int screen_width;             /* CONST x11 */
  int screen_height;            /* CONST x11 */
  int screen_xpos;              /* CONST x11 */
  int height;                   /* CACHE (font) */
  int line_height;              /* CACHE (font) */
  xosd_pos pos;                 /* CONF */
  xosd_align align;             /* CONF */
  int hoffset;                  /* CONF */
  int voffset;                  /* CONF */
  int shadow_offset;            /* CONF */
  XColor shadow_colour;         /* CONF */
  unsigned long shadow_pixel;   /* CACHE (shadow_colour) */
  int outline_offset;           /* CONF */
  XColor outline_colour;        /* CONF */
  unsigned long outline_pixel;  /* CACHE (outline_colour) */
  int bar_length;               /* CONF */

  int generation;               /* DYN count of map/unmap */
  int done;                     /* DYN */
  enum {
    UPD_none = 0,       /* Nothing changed */
    UPD_hide = (1<<0),  /* Force hiding */
    UPD_show = (1<<1),  /* Force display */
    UPD_timer = (1<<2), /* Start timer */
    UPD_pos = (1<<3),   /* Reposition window */
    UPD_lines = (1<<4), /* Redraw content */
    UPD_mask = (1<<5),  /* Update mask */
    UPD_size = (1<<6),  /* Change font and window size */
    UPD_content = UPD_mask | UPD_lines,
    UPD_font = UPD_size | UPD_mask | UPD_lines | UPD_pos
  } update;                     /* DYN */

  unsigned long pixel;          /* CACHE (pixel) */
  XColor colour;                /* CONF */

  union xosd_line *lines;       /* CONF */
  int number_lines;             /* CONF */

  int timeout;                  /* CONF delta time */
  struct timeval timeout_start; /* DYN Absolute start of timeout */
};

static const int XOSD_MAX_PRINTF_BUF_SIZE=2000;

/* vim: foldmethod=marker tabstop=2 shiftwidth=2 expandtab
 */
