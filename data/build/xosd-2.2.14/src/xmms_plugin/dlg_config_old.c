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

#include "xmms_osd.h"

#include <xmms/configfile.h>

static GtkToggleButton
  * vol_on, *bal_on,
  *pause_on, *trackname_on, *stop_on, *repeat_on, *shuffle_on;
static GtkWidget *configure_win;
static GtkObject *timeout_obj, *offset_obj, *shadow_obj, *outline_obj;
static GtkWidget *timeout_spin, *offset_spin, *shadow_spin, *outline_spin;
static GtkWidget *pos_top, *pos_middle, *pos_bottom;
static GtkWidget *align_left, *align_right, *align_center;

/*
 * Return state of check button.
 */
static gboolean
isactive(GtkToggleButton * item)
{
  DEBUG("is active");
  return gtk_toggle_button_get_active(item) ? 1 : 0;
}

/*
 * Apply changed from configuration dialog.
 */
static void
configure_apply_cb(gpointer data)
{
  show.volume = isactive(vol_on);
  show.balance = isactive(bal_on);
  show.pause = isactive(pause_on);
  show.trackname = isactive(trackname_on);
  show.stop = isactive(stop_on);
  show.repeat = isactive(repeat_on);
  show.shuffle = isactive(shuffle_on);


  if (colour)
    g_free(colour);
  if (font)
    g_free(font);

  colour = g_strdup(gtk_entry_get_text(GTK_ENTRY(colour_entry)));
  font = g_strdup(gtk_entry_get_text(GTK_ENTRY(font_entry)));
  timeout = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(timeout_spin));
  offset = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(offset_spin));
  shadow_offset =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(shadow_spin));
  outline_offset =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(outline_spin));

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pos_top)))
    pos = XOSD_top;
  else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pos_middle)))
    pos = XOSD_middle;
  else
    pos = XOSD_bottom;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(align_left)))
    align = XOSD_left;
  else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(align_center)))
    align = XOSD_center;
  else
    align = XOSD_right;

  apply_config();
  write_config();
}

/*
 * Apply changes and close configuration dialog.
 */
static void
configure_ok_cb(gpointer data)
{
  DEBUG("configure_ok_cb");
  configure_apply_cb(data);

  gtk_widget_destroy(configure_win);
  configure_win = NULL;
}

/*
 * Add item to configuration dialog.
 */
static void
show_item(GtkWidget * vbox, const char *description, int selected,
          GtkToggleButton ** on)
{
  /* GtkWidget  *hbox, *label; */
  /*GSList *group = NULL; */

  /*hbox = gtk_hbox_new (FALSE, 5); */

  /*gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0); */

  DEBUG("show_item");

  *on = (GtkToggleButton *) gtk_check_button_new_with_label(description);
  gtk_box_pack_start(GTK_BOX(vbox), (GtkWidget *) * on, FALSE, FALSE, 0);

  /*label = gtk_label_new (description);
     gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

     *on = gtk_radio_button_new_with_label (NULL, "Yes");
     group = gtk_radio_button_group (GTK_RADIO_BUTTON (*on));
     *off = gtk_radio_button_new_with_label (group, "No");

     gtk_box_pack_start (GTK_BOX (hbox), *on, FALSE, FALSE, 0);
     gtk_box_pack_start (GTK_BOX (hbox), *off, FALSE, FALSE, 0);
   */

  gtk_toggle_button_set_active(*on, selected);
}

/*
 * Create dialog window for configuration.
 */
void
configure(void)
{
  GtkWidget *vbox, *bbox, *ok, *cancel, *apply, *hbox, *label,
    *button, *unit_label, *hbox2, *vbox2, *sep;
  GtkWidget *table;

  GSList *group = NULL;
  GSList *group2 = NULL;

  DEBUG("configure");
  if (configure_win)
    return;

  read_config();

  configure_win = gtk_window_new(GTK_WINDOW_DIALOG);

  gtk_signal_connect(GTK_OBJECT(configure_win), "destroy",
                     GTK_SIGNAL_FUNC(gtk_widget_destroyed), &configure_win);

  gtk_window_set_title(GTK_WINDOW(configure_win),
                       "OSD " XOSD_VERSION " Configuration");

  vbox = gtk_vbox_new(FALSE, 12);
  gtk_container_add(GTK_CONTAINER(configure_win), vbox);
  gtk_container_set_border_width(GTK_CONTAINER(configure_win), 12);

  /* --=mjs=-- The Main table to pack everything into */
  table = gtk_table_new(6, 3, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(table), 12);
  gtk_table_set_col_spacings(GTK_TABLE(table), 12);
  gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 0);

  /* Font selector. */
  label = gtk_label_new("Font:");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1,
                   GTK_FILL, GTK_FILL, 0, 0);
  font_entry = gtk_entry_new();
  if (font)
    gtk_entry_set_text(GTK_ENTRY(font_entry), font);
  gtk_table_attach(GTK_TABLE(table), font_entry, 1, 2, 0, 1,
                   GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);

  button = gtk_button_new_with_label("Set...");
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
                     GTK_SIGNAL_FUNC(font_dialog_window), NULL);
  gtk_table_attach(GTK_TABLE(table), button, 2, 3, 0, 1,
                   GTK_FILL, GTK_FILL, 0, 0);

  /* Colour Selector */
  label = gtk_label_new("Colour:");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2,
                   GTK_FILL, GTK_FILL, 0, 0);
  colour_entry = gtk_entry_new();
  if (colour)
    gtk_entry_set_text(GTK_ENTRY(colour_entry), colour);
  gtk_table_attach(GTK_TABLE(table), colour_entry, 1, 2, 1, 2,
                   GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  button = gtk_button_new_with_label("Set...");
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
                     GTK_SIGNAL_FUNC(colour_dialog_window), NULL);
  gtk_table_attach(GTK_TABLE(table), button, 2, 3, 1, 2,
                   GTK_FILL, GTK_FILL, 0, 0);

  /* Timeout */
  label = gtk_label_new("Timeout:");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3,
                   GTK_FILL, GTK_FILL, 0, 0);
  hbox = gtk_hbox_new(FALSE, 6);
  gtk_table_attach(GTK_TABLE(table), hbox, 1, 2, 2, 3,
                   GTK_FILL, GTK_FILL, 0, 0);
  timeout_obj = gtk_adjustment_new(timeout, -1, 60, 1, 1, 1);
  timeout_spin = gtk_spin_button_new(GTK_ADJUSTMENT(timeout_obj), 1.0, 0);
  if (timeout)
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(timeout_spin),
                              (gfloat) timeout);
  gtk_box_pack_start(GTK_BOX(hbox), timeout_spin, FALSE, FALSE, 0);
  unit_label = gtk_label_new("seconds");
  gtk_misc_set_alignment(GTK_MISC(unit_label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(unit_label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start(GTK_BOX(hbox), unit_label, FALSE, FALSE, 0);

  /* Vertical Offset */
  label = gtk_label_new("Vertical Offset:");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4,
                   GTK_FILL, GTK_FILL, 0, 0);
  hbox = gtk_hbox_new(FALSE, 6);
  gtk_table_attach(GTK_TABLE(table), hbox, 1, 2, 3, 4,
                   GTK_FILL, GTK_FILL, 0, 0);
  offset_obj = gtk_adjustment_new(timeout, 0, 60, 1, 1, 1);
  offset_spin = gtk_spin_button_new(GTK_ADJUSTMENT(offset_obj), 1.0, 0);
  if (offset)
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(offset_spin), (gfloat) offset);
  gtk_box_pack_start(GTK_BOX(hbox), offset_spin, FALSE, FALSE, 0);
  unit_label = gtk_label_new("pixels");
  gtk_misc_set_alignment(GTK_MISC(unit_label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(unit_label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start(GTK_BOX(hbox), unit_label, FALSE, FALSE, 0);

  /* Shadow Offset */
  label = gtk_label_new("Shadow Offset:");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5,
                   GTK_FILL, GTK_FILL, 0, 0);
  hbox = gtk_hbox_new(FALSE, 6);
  gtk_table_attach(GTK_TABLE(table), hbox, 1, 2, 4, 5,
                   GTK_FILL, GTK_FILL, 0, 0);
  shadow_obj = gtk_adjustment_new(timeout, 0, 60, 1, 1, 1);
  shadow_spin = gtk_spin_button_new(GTK_ADJUSTMENT(shadow_obj), 1.0, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(shadow_spin),
                            (gfloat) shadow_offset);
  gtk_box_pack_start(GTK_BOX(hbox), shadow_spin, FALSE, FALSE, 0);
  unit_label = gtk_label_new("pixels");
  gtk_misc_set_alignment(GTK_MISC(unit_label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(unit_label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start(GTK_BOX(hbox), unit_label, FALSE, FALSE, 0);

  /* Outline Offset */
  label = gtk_label_new("Outline Offset:");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 5, 6,
                   GTK_FILL, GTK_FILL, 0, 0);
  hbox = gtk_hbox_new(FALSE, 6);
  gtk_table_attach(GTK_TABLE(table), hbox, 1, 2, 5, 6,
                   GTK_FILL, GTK_FILL, 0, 0);
  outline_obj = gtk_adjustment_new(timeout, 0, 60, 1, 1, 1);
  outline_spin = gtk_spin_button_new(GTK_ADJUSTMENT(outline_obj), 1.0, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(outline_spin),
                            (gfloat) outline_offset);
  gtk_box_pack_start(GTK_BOX(hbox), outline_spin, FALSE, FALSE, 0);
  unit_label = gtk_label_new("pixels");
  gtk_misc_set_alignment(GTK_MISC(unit_label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(unit_label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start(GTK_BOX(hbox), unit_label, FALSE, FALSE, 0);

  /* Position */
  label = gtk_label_new("Position:");
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
  gtk_table_attach(GTK_TABLE(table), label, 0, 1, 6, 7,
                   GTK_FILL, GTK_FILL, 0, 0);
  vbox2 = gtk_vbox_new(FALSE, 6);
  gtk_table_attach(GTK_TABLE(table), vbox2, 1, 2, 6, 7,
                   GTK_FILL, GTK_FILL, 0, 0);
  pos_top = gtk_radio_button_new_with_label(NULL, "Top");
  group = gtk_radio_button_group(GTK_RADIO_BUTTON(pos_top));
  pos_middle = gtk_radio_button_new_with_label(group, "Middle");
  pos_bottom =
    gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pos_top),
                                                "Bottom");
  gtk_box_pack_start(GTK_BOX(vbox2), pos_top, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), pos_middle, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), pos_bottom, FALSE, FALSE, 0);

  if (pos == XOSD_top)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pos_top), TRUE);
  else if (pos == XOSD_middle)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pos_middle), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pos_bottom), TRUE);

  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  label = gtk_label_new("Alginment:");
  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
  align_left = gtk_radio_button_new_with_label(NULL, "Left");
  group2 = gtk_radio_button_group(GTK_RADIO_BUTTON(align_left));
  align_center = gtk_radio_button_new_with_label(group2, "Center");
  align_right =
    gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON
                                                (align_left), "Right");
  gtk_box_pack_start(GTK_BOX(hbox), align_left, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), align_center, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), align_right, FALSE, FALSE, 0);

  if (align == XOSD_left)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(align_left), TRUE);
  else if (align == XOSD_center)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(align_center), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(align_right), TRUE);

  /*
     hbox = gtk_hbox_new (FALSE, 5);

     gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
     label = gtk_label_new ("Volume :");
     gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
     vol_on = gtk_radio_button_new_with_label (NULL, "Yes");
     group = gtk_radio_button_group (GTK_RADIO_BUTTON (vol_on));
     vol_off = gtk_radio_button_new_with_label (group, "No");
     gtk_box_pack_start (GTK_BOX (hbox), vol_on, FALSE, FALSE, 0);
     gtk_box_pack_start (GTK_BOX (hbox), vol_off, FALSE, FALSE, 0);

     if (show.volume == 1)
     gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (vol_on), TRUE);
     else
     gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (vol_off), TRUE);
   */

  sep = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, FALSE, 0);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
  label = gtk_label_new("Show:");
  gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, FALSE, 0);

  hbox2 = gtk_hbox_new(FALSE, 2);
  gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);

  vbox2 = gtk_vbox_new(FALSE, 4);
  gtk_box_pack_start(GTK_BOX(hbox2), vbox2, FALSE, FALSE, 0);

  show_item(vbox2, "Volume", show.volume, &vol_on);
  show_item(vbox2, "Balance", show.balance, &bal_on);
  show_item(vbox2, "Pause", show.pause, &pause_on);
  show_item(vbox2, "Track Name", show.trackname, &trackname_on);
  vbox2 = gtk_vbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(hbox2), vbox2, FALSE, FALSE, 0);
  show_item(vbox2, "Stop", show.stop, &stop_on);
  show_item(vbox2, "Repeat", show.repeat, &repeat_on);
  show_item(vbox2, "Shuffle", show.shuffle, &shuffle_on);

  sep = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, FALSE, 0);

  bbox = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing(GTK_BUTTON_BOX(bbox), 5);
  gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE, 0);

  ok = gtk_button_new_with_label("Ok");
  gtk_signal_connect(GTK_OBJECT(ok), "clicked",
                     GTK_SIGNAL_FUNC(configure_ok_cb), NULL);
  GTK_WIDGET_SET_FLAGS(ok, GTK_CAN_DEFAULT);
  gtk_box_pack_start(GTK_BOX(bbox), ok, TRUE, TRUE, 0);
  gtk_widget_grab_default(ok);

  apply = gtk_button_new_with_label("Apply");
  gtk_signal_connect(GTK_OBJECT(apply), "clicked",
                     GTK_SIGNAL_FUNC(configure_apply_cb), NULL);
  GTK_WIDGET_SET_FLAGS(apply, GTK_CAN_DEFAULT);
  gtk_box_pack_start(GTK_BOX(bbox), apply, TRUE, TRUE, 0);

  cancel = gtk_button_new_with_label("Cancel");
  gtk_signal_connect_object(GTK_OBJECT(cancel), "clicked",
                            GTK_SIGNAL_FUNC(gtk_widget_destroy),
                            GTK_OBJECT(configure_win));
  GTK_WIDGET_SET_FLAGS(cancel, GTK_CAN_DEFAULT);
  gtk_box_pack_start(GTK_BOX(bbox), cancel, TRUE, TRUE, 0);


  gtk_widget_show_all(configure_win);
}

/* vim: tabstop=8 shiftwidth=8 noexpandtab
*/
