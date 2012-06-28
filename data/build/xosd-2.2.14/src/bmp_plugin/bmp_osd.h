#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <xosd.h>

#if 0
#define DEBUG(fmt, a...) fprintf (stderr, "%s:%d %s: " fmt "\n", __FILE__ , __LINE__ , __PRETTY_FUNCTION__ , ##a)
#else
#define DEBUG(fmt, a...)
#endif

struct show
{
  gboolean volume;
  gboolean balance;
  gboolean pause;
  gboolean trackname;
  gboolean stop;
  gboolean repeat;
  gboolean shuffle;
};
extern struct show show;

extern gchar *font;
extern gchar *colour;
extern gint timeout;
extern gint offset;
extern gint h_offset;
extern gint shadow_offset;
extern gint outline_offset;
extern gint pos;
extern gint align;

/* dlg_font.c */
extern GtkWidget *font_entry;
extern int font_dialog_window(GtkButton * button, gpointer user_data);
/* dlg_colour.c */
extern GtkWidget *colour_entry;
extern int colour_dialog_window(GtkButton * button, gpointer user_data);
/* dlg_config.c */
extern void configure(void);
extern int colour_dialog_window(GtkButton * button, gpointer user_data);
/* xmms_osd.c */
extern xosd *osd;
extern void read_config(void);
extern void write_config(void);
extern void apply_config(void);

/* vim: tabstop=8 shiftwidth=8 noexpandtab
 */
