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

GtkWidget *font_entry;

/*
 * Apply font change and close dialog.
 */
static int
font_dialog_ok(GtkButton * button, gpointer user_data)
{
  GtkWidget *font_dialog = user_data;
  char *tmp_font;
  DEBUG("font_dialog_ok");

  assert(GTK_IS_FONT_SELECTION_DIALOG(font_dialog));

  tmp_font = gtk_font_selection_dialog_get_font_name
    (GTK_FONT_SELECTION_DIALOG(font_dialog));

  gtk_entry_set_text(GTK_ENTRY(font_entry), tmp_font);

  gtk_widget_destroy(font_dialog);

  return 0;
}

/*
 * Apply font change and close dialog.
 */
static int
font_dialog_apply(GtkButton * button, gpointer user_data)
{
  GtkWidget *font_dialog = user_data;
  char *tmp_font;
  DEBUG("font_dialog_apply");

  assert(GTK_IS_FONT_SELECTION_DIALOG(font_dialog));

  tmp_font = gtk_font_selection_dialog_get_font_name
    (GTK_FONT_SELECTION_DIALOG(font_dialog));

  gtk_entry_set_text(GTK_ENTRY(font_entry), tmp_font);

  return 0;
}

/*
 * Create dialog window for font selection.
 */
int
font_dialog_window(GtkButton * button, gpointer user_data)
{
  GtkWidget *font_dialog;
  GtkWidget *vbox;
  GtkWidget *cancel_button, *apply_button, *ok_button;
  GList *children;

  DEBUG("font_dialog_window");
  font_dialog = gtk_font_selection_dialog_new("XOSD Font");

  assert(font_dialog);

  if (font)
    gtk_font_selection_dialog_set_font_name
      (GTK_FONT_SELECTION_DIALOG(font_dialog), font);

  children = gtk_container_children(GTK_CONTAINER(font_dialog));

  vbox = GTK_WIDGET(children->data);

  children = gtk_container_children(GTK_CONTAINER(vbox));

  vbox = GTK_WIDGET(children->next->data);
  children = gtk_container_children(GTK_CONTAINER(vbox));
  ok_button = GTK_WIDGET(children->data);

  apply_button = GTK_WIDGET(children->next->data);

  cancel_button = GTK_WIDGET(children->next->next->data);

  gtk_signal_connect_object(GTK_OBJECT(cancel_button), "clicked",
                            GTK_SIGNAL_FUNC(gtk_widget_destroy),
                            GTK_OBJECT(font_dialog));

  gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
                     GTK_SIGNAL_FUNC(font_dialog_ok), font_dialog);
  gtk_signal_connect(GTK_OBJECT(apply_button), "clicked",
                     GTK_SIGNAL_FUNC(font_dialog_apply), font_dialog);


  gtk_widget_show_all(font_dialog);
  return 0;
}

/* vim: tabstop=8 shiftwidth=8 noexpandtab
 */
