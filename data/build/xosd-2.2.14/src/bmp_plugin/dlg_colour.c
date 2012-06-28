/* XOSD

Copyright (c) 2001 Andre Renaud (andre@ignavus.net)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <gtk/gtk.h>

#include "bmp_osd.h"

GtkWidget *colour_entry;

/*
 * Apply colour changes and close window.
 */
static int
colour_dialog_ok(GtkButton * button, gpointer user_data)
{
  GtkWidget *colour_dialog = user_data;
  char tmp_colour[8];
  double colour[4];

  DEBUG("colour_dialog_ok");
  assert(GTK_IS_COLOR_SELECTION_DIALOG(colour_dialog));

  gtk_color_selection_get_color
    (GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG
                         (colour_dialog)->colorsel), colour);

  sprintf(tmp_colour, "#%2.2x%2.2x%2.2x",
          (int) (colour[0] * 255),
          (int) (colour[1] * 255), (int) (colour[2] * 255));

  gtk_entry_set_text(GTK_ENTRY(colour_entry), tmp_colour);

  gtk_widget_destroy(colour_dialog);

  return 0;
}

/*
 * Create dialog window for colour selection.
 */
int
colour_dialog_window(GtkButton * button, gpointer user_data)
{
  GtkWidget *colour_dialog;
  GtkWidget *cancel_button, *ok_button, *colour_widget;
  gdouble colour[4];
  int red, green, blue;

  DEBUG("colour_dialog_window");
  colour_dialog = gtk_color_selection_dialog_new("XOSD Colour");

  assert(colour_dialog);

  colour_widget = GTK_COLOR_SELECTION_DIALOG(colour_dialog)->colorsel;
  if (osd) {
    xosd_get_colour(osd, &red, &green, &blue);

    colour[0] = (float) red / (float) USHRT_MAX;
    colour[1] = (float) green / (float) USHRT_MAX;
    colour[2] = (float) blue / (float) USHRT_MAX;

    gtk_color_selection_set_color(GTK_COLOR_SELECTION
                                  (GTK_COLOR_SELECTION_DIALOG
                                   (colour_dialog)->colorsel), colour);
  }

  ok_button = GTK_COLOR_SELECTION_DIALOG(colour_dialog)->ok_button;
  cancel_button = GTK_COLOR_SELECTION_DIALOG(colour_dialog)->cancel_button;

  gtk_signal_connect_object(GTK_OBJECT(cancel_button), "clicked",
                            GTK_SIGNAL_FUNC(gtk_widget_destroy),
                            GTK_OBJECT(colour_dialog));

  gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
                     GTK_SIGNAL_FUNC(colour_dialog_ok), colour_dialog);

  gtk_widget_show_all(colour_dialog);
  return 0;
}

/* vim: tabstop=8 shiftwidth=8 noexpandtab
 */
