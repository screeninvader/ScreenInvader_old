/* XOSD

Copyright (c) 2001 Andre Renaud (andre@ignavus.net)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <ctype.h>
#include <gtk/gtk.h>

#include "osd.h"

static void init(void);
static void cleanup(void);
static gint timeout_func(char *);


struct state
{
  gboolean playing;
  gboolean paused;
  gboolean shuffle;
  gboolean repeat;
  int pos;
  int volume;
  int balance;
  gchar *title;
};
static struct state previous;
struct show show;

xosd *osd = NULL;
static guint timeout_tag;

gchar *font;
gchar *colour;

gint timeout;
gint offset;
gint h_offset;
gint shadow_offset;
gint outline_offset;
gint pos;
gint align;

int main(int argc, char *argv[])
{
  DEBUG("init");
  previous.title=NULL;
  if (osd) {
    DEBUG("uniniting osd");
    xosd_destroy(osd);
    osd = NULL;
  }

  read_config();

  memset(&previous, 0, sizeof(struct state));

  DEBUG("calling osd init function");

  osd = xosd_create(2);
  apply_config();
  DEBUG("osd initialized");
   
  if (osd) {
    DEBUG("osd initialized");
    while(1) {
      timeout_func(argv[1]);
      sleep(3);
    }
  }
  return 0;
}

/*
 * Free memory and release resources.
 */
static void
cleanup(void)
{
  DEBUG("cleanup");
  if (osd && timeout_tag)
    gtk_timeout_remove(timeout_tag);
  timeout_tag = 0;

  if (font) {
    g_free(font);
    font = NULL;
  }

  if (colour) {
    g_free(colour);
    colour = NULL;
  }

  if (previous.title) {
    g_free(previous.title);
    previous.title = NULL;
  }

  if (osd) {
    DEBUG("hide");
    xosd_hide(osd);
    DEBUG("uninit");
    xosd_destroy(osd);
    DEBUG("done with osd");
    osd = NULL;
  }
}

/*
 * Read configuration and initialize variables.
 */
void
read_config(void)
{

  show.volume = 1;
  show.balance = 1;
  show.pause = 1;
  show.trackname = 1;
  show.stop = 1;
  show.repeat = 1;
  show.shuffle = 1;

  g_free(colour);
  g_free(font);
  colour = NULL;
  font = NULL;
  timeout = 3;
  offset = 50;
  h_offset = 0;
  shadow_offset = 1;
  outline_offset = 0;
  pos = XOSD_bottom;
  align = XOSD_left;

  DEBUG("getting default font");
  if (font == NULL)
    font = g_strdup("-*-*-*-*-*-*-40-*-*-*-*-*-*-*");

  printf("%s", "whatthefuck");
	printf("%s", font);

	DEBUG("default colour");
  if (colour == NULL)
    colour = g_strdup("white");

  DEBUG("done");
}

void
apply_config(void)
{
  DEBUG("apply_config");
  if (osd) {
    if (xosd_set_font(osd, font) == -1)
      DEBUG("invalid font %s", font);

    xosd_set_colour(osd, colour);
    xosd_set_timeout(osd, timeout);
    xosd_set_shadow_offset(osd, shadow_offset);
    xosd_set_outline_offset(osd, outline_offset);
    xosd_set_pos(osd, pos);
    xosd_set_align(osd, align);
    xosd_set_vertical_offset(osd, offset);
    xosd_set_horizontal_offset(osd, h_offset);
  }
  DEBUG("done");
}


/*
 * Callback funtion to handle delayed display.
 */
static gint
timeout_func(char* text)
{
  DEBUG("timeout func");

  if (!osd)
    return FALSE;
    xosd_display(osd, 1, XOSD_string, text);

skip:

  GDK_THREADS_LEAVE();


  return TRUE;
}

/* vim: tabstop=8 shiftwidth=8 noexpandtab
 */
