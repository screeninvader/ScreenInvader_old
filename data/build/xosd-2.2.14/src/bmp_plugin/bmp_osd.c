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

#include "bmp_osd.h"

#include <xmms/plugin.h>
#include <xmms/xmmsctrl.h>
#include <xmms/configfile.h>

static void init(void);
static void cleanup(void);
static gint timeout_func(gpointer);

GeneralPlugin gp = {
  .handle = NULL,
  .filename = NULL,
  .xmms_session = -1,
  .description = "On Screen Display " XOSD_VERSION,
  .init = init,
  .about = NULL,
  .configure = configure,
  .cleanup = cleanup,
};


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

/*
 * Return plugin structure.
 */
GeneralPlugin *
get_gplugin_info(void)
{
  return &gp;
}


/*
 * Initialize plugin.
 */
static void
init(void)
{
  DEBUG("init");

  if (osd) {
    DEBUG("uniniting osd");
    xosd_destroy(osd);
    osd = NULL;
  }

  read_config();

#if 0
  /* Deadlocks, because it is to early. */
  previous.playing = xmms_remote_is_playing(gp.xmms_session);
  previous.paused = xmms_remote_is_paused(gp.xmms_session);
  previous.shuffle = xmms_remote_is_shuffle(gp.xmms_session);
  previous.repeat = xmms_remote_is_repeat(gp.xmms_session);
  previous.pos = xmms_remote_get_playlist_pos(gp.xmms_session);
  previous.volume = xmms_remote_get_main_volume(gp.xmms_session);
  previous.balance = (xmms_remote_get_balance(gp.xmms_session) + 100) / 2;
  previous.title = NULL;
#else
  memset(&previous, 0, sizeof(struct state));
#endif

  DEBUG("calling osd init function");

  osd = xosd_create(2);
  apply_config();
  DEBUG("osd initialized");
  if (osd)
    timeout_tag = gtk_timeout_add(100, timeout_func, NULL);
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
  ConfigFile *cfgfile;
  DEBUG("read_config");

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

  DEBUG("reading configuration data");
  if ((cfgfile = xmms_cfg_open_default_file()) != NULL) {
    xmms_cfg_read_string(cfgfile, "osd", "font", &font);
    xmms_cfg_read_string(cfgfile, "osd", "colour", &colour);
    xmms_cfg_read_int(cfgfile, "osd", "timeout", &timeout);
    xmms_cfg_read_int(cfgfile, "osd", "offset", &offset);
    xmms_cfg_read_int(cfgfile, "osd", "h_offset", &h_offset);
    xmms_cfg_read_int(cfgfile, "osd", "shadow_offset", &shadow_offset);
    xmms_cfg_read_int(cfgfile, "osd", "outline_offset", &outline_offset);
    xmms_cfg_read_int(cfgfile, "osd", "pos", &pos);
    xmms_cfg_read_int(cfgfile, "osd", "align", &align);
    xmms_cfg_read_int(cfgfile, "osd", "show_volume", &show.volume);
    xmms_cfg_read_int(cfgfile, "osd", "show_balance", &show.balance);
    xmms_cfg_read_int(cfgfile, "osd", "show_pause", &show.pause);
    xmms_cfg_read_int(cfgfile, "osd", "show_trackname", &show.trackname);
    xmms_cfg_read_int(cfgfile, "osd", "show_stop", &show.stop);
    xmms_cfg_read_int(cfgfile, "osd", "show_repeat", &show.repeat);
    xmms_cfg_read_int(cfgfile, "osd", "show_shuffle", &show.shuffle);
    xmms_cfg_free(cfgfile);
  }

  DEBUG("getting default font");
  if (font == NULL)
    font = g_strdup(osd_default_font);

  DEBUG("default colour");
  if (colour == NULL)
    colour = g_strdup("green");

  DEBUG("done");
}

/*
 * Write configuration.
 */
void
write_config(void)
{
  ConfigFile *cfgfile;
  DEBUG("write_config");

  if ((cfgfile = xmms_cfg_open_default_file()) != NULL) {
    xmms_cfg_write_string(cfgfile, "osd", "colour", colour);
    xmms_cfg_write_string(cfgfile, "osd", "font", font);
    xmms_cfg_write_int(cfgfile, "osd", "timeout", timeout);
    xmms_cfg_write_int(cfgfile, "osd", "offset", offset);
    xmms_cfg_write_int(cfgfile, "osd", "h_offset", h_offset);
    xmms_cfg_write_int(cfgfile, "osd", "shadow_offset", shadow_offset);
    xmms_cfg_write_int(cfgfile, "osd", "outline_offset", outline_offset);
    xmms_cfg_write_int(cfgfile, "osd", "pos", pos);
    xmms_cfg_write_int(cfgfile, "osd", "align", align);

    xmms_cfg_write_int(cfgfile, "osd", "show_volume", show.volume);
    xmms_cfg_write_int(cfgfile, "osd", "show_balance", show.balance);
    xmms_cfg_write_int(cfgfile, "osd", "show_pause", show.pause);
    xmms_cfg_write_int(cfgfile, "osd", "show_trackname", show.trackname);
    xmms_cfg_write_int(cfgfile, "osd", "show_stop", show.stop);
    xmms_cfg_write_int(cfgfile, "osd", "show_repeat", show.repeat);
    xmms_cfg_write_int(cfgfile, "osd", "show_shuffle", show.shuffle);

    xmms_cfg_write_default_file(cfgfile);
    xmms_cfg_free(cfgfile);
  }

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
 * Convert hexcode to ASCII.
 */
static void
replace_hexcodes(gchar * text)
{
  gchar *head, *tail;
  int conv_underscore, c;
  ConfigFile *cfgfile;
  DEBUG("replace_hexcodes");

  if ((cfgfile = xmms_cfg_open_default_file()) != NULL) {
    xmms_cfg_read_boolean(cfgfile, "xmms", "convert_underscore",
                          &conv_underscore);
    xmms_cfg_free(cfgfile);
  }

  for (head = tail = text; *head; head++, tail++) {
    /* replace underscors with spaces if necessary */
    if (conv_underscore && *head == '_') {
      *tail = ' ';
      continue;
    }
    /* replace hex with character if necessary */
    if (*head == '%' && isxdigit(head[1]) && isxdigit(head[2])) {
      sscanf(head + 1, "%2x", &c);
      *tail = (char) c;
      head += 2;
      continue;
    }
    *tail = *head;
  }
  *tail = '\0';
}

/*
 * Callback funtion to handle delayed display.
 */
static gint
timeout_func(gpointer data)
{
  struct state current;
  char *text = NULL;
  gboolean songchange, showtext, withtime = FALSE;
  gint playlist_length;

  DEBUG("timeout func");

  if (!osd)
    return FALSE;

  GDK_THREADS_ENTER();

  current.playing = xmms_remote_is_playing(gp.xmms_session);
  current.paused = xmms_remote_is_paused(gp.xmms_session);
  current.shuffle = xmms_remote_is_shuffle(gp.xmms_session);
  current.repeat = xmms_remote_is_repeat(gp.xmms_session);
  current.pos = xmms_remote_get_playlist_pos(gp.xmms_session);
  current.volume = xmms_remote_get_main_volume(gp.xmms_session);
  current.balance = (xmms_remote_get_balance(gp.xmms_session) + 100) / 2;

  /* Get the current title only if the playlist is not empty. Otherwise
   * it'll crash. Don't forget to free the title! */
  playlist_length = xmms_remote_get_playlist_length(gp.xmms_session);
  current.title = playlist_length
    ? xmms_remote_get_playlist_title(gp.xmms_session, current.pos) : NULL;

  /* Check for song change. Deleting a song from the playlist only changed the
   * name, but not the position, so compare also by (still hexencoded) name. */
  songchange =
    (previous.pos != current.pos) ||
    (previous.title == NULL && current.title != NULL) ||
    (previous.title != NULL && current.title == NULL) ||
    (previous.title != NULL && current.title != NULL &&
     (g_strcasecmp(previous.title, current.title) != 0));

  /* Possible show something when either song or state changed. */
  showtext = songchange ||
    ((current.playing != previous.playing) ||
     (current.paused != previous.paused));

  /* Determine right text depending on state and state/title change.
   *    +---+          +---+
   *    |   |          |   |
   *    +->PLAY<---->STOP<-+
   *        ^          ^
   *        |          |
   *        +-->PAUSE--+
   */
  if (!current.playing) {       /* {PLAY,PAUSE,STOP} -> STOP */
    text = "Stopped";
    showtext &= show.stop;
  } else if (current.paused) {  /* PLAY -> PAUSE */
    text = "Paused";
    showtext &= show.pause;
    withtime = TRUE;
  } else if (previous.paused && !current.paused && !songchange) {       /* PAUSE -SameSong-> PLAY */
    text = "Unpaused";
    showtext &= show.pause;
    withtime = TRUE;
  } else {                      /* {PLAY,STOP} -> PLAY <-OtherSong- PAUSE */
    text = "Playing";
    showtext &= show.trackname;
  }

  /* Decide what to display, in decreasing priority. */
  if (showtext) {
    xosd_display(osd, 0, XOSD_string, text);
    if (show.trackname && (current.title != NULL)) {
      int len;
      char *title;
      gint playlist_time;

      len = 13 + strlen(current.title) + (withtime ? 11 : 0);
      title = malloc(len);
      playlist_time =
        withtime ? xmms_remote_get_output_time(gp.xmms_session) : 0;
      snprintf(title, len,
               withtime ? "%i/%i: %s (%i:%02i)" : "%i/%i: %s",
               current.pos + 1, playlist_length, current.title,
               playlist_time / 1000 / 60, playlist_time / 1000 % 60);
      replace_hexcodes(title);
      xosd_display(osd, 1, XOSD_string, title);
      free(title);
    } else
      xosd_display(osd, 1, XOSD_string, "");
  } else if (current.volume != previous.volume && show.volume) {
    DEBUG("V: %d->%d\n", previous.volume, current.volume);
    /* xmms returns -1 during a title change. skip this and try again later. */
    if ((previous.volume == -1) || (current.volume == -1))
      goto skip;
    xosd_display(osd, 0, XOSD_string, "Volume");
    xosd_display(osd, 1, XOSD_percentage, current.volume);
  } else if (current.balance != previous.balance && show.balance) {
    DEBUG("B: %d->%d\n", previous.balance, current.balance);
    /* FIXME: Same as above might happen, but with what values? */
    xosd_display(osd, 0, XOSD_string, "Balance");
    xosd_display(osd, 1, XOSD_slider, current.balance);
  } else if (current.repeat != previous.repeat && show.repeat) {
    xosd_display(osd, 0, XOSD_string, "Repeat");
    xosd_display(osd, 1, XOSD_string, current.repeat ? "On" : "Off");
  } else if (current.shuffle != previous.shuffle && show.shuffle) {
    xosd_display(osd, 0, XOSD_string, "Shuffle");
    xosd_display(osd, 1, XOSD_string, current.shuffle ? "On" : "Off");
  }

skip:
  /* copy current state (including title) for future comparison. Free old
   * title first. */
  if (previous.title)
    g_free(previous.title);
  previous = current;

  GDK_THREADS_LEAVE();


  return TRUE;
}

/* vim: tabstop=8 shiftwidth=8 noexpandtab
 */
