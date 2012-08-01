#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "xosd.h"

void
printerror()
{
  fprintf(stderr, "ERROR: %s\n", xosd_error);
}

int
main(int argc, char *argv[])
{
  xosd *osd;
  int a;

  osd = xosd_create(2);

  if (!osd) {
    printerror();
    return 1;
  }

  if (0 != xosd_set_outline_offset(osd, 1)) {
    printerror();
  }

  if (0 != xosd_set_font(osd, (char *) osd_default_font)) {
    printerror();
  }

  if (0 != xosd_set_timeout(osd, 2)) {
    printerror();
  }

  for (a = 0; a <= 100; a++) {
    if (-1 == xosd_display(osd, 0, XOSD_percentage, a))
      printerror();
    if (-1 == xosd_display(osd, 1, XOSD_percentage, 100 - a))
      printerror();
    usleep(100);
  }
  for (a = 100; a >= 0; a--) {
    if (-1 == xosd_display(osd, 0, XOSD_percentage, a))
      printerror();
    if (-1 == xosd_display(osd, 1, XOSD_percentage, 100 - a))
      printerror();
    usleep(100);
  }

  for (a = 0; a <= 100; a++) {
    if (-1 == xosd_display(osd, 0, XOSD_slider, a))
      printerror();
    if (-1 == xosd_display(osd, 1, XOSD_slider, 100 - a))
      printerror();
    usleep(100);
  }
  for (a = 100; a >= 0; a--) {
    if (-1 == xosd_display(osd, 0, XOSD_slider, a))
      printerror();
    if (-1 == xosd_display(osd, 1, XOSD_slider, 100 - a))
      printerror();
    usleep(100);
  }
  if (-1 == xosd_display(osd, 1, XOSD_string, ""))
    printerror();


  xosd_set_bar_length(osd, 14);


  if (-1 == xosd_display(osd, 0, XOSD_percentage, 80)) {
    printerror();
  }

  if (0 != xosd_wait_until_no_display(osd)) {
    printerror();
  }

  if (-1 == xosd_display(osd, 0, XOSD_slider, 36)) {
    printerror();
  }

  if (0 != xosd_wait_until_no_display(osd)) {
    printerror();
  }

  if (-1 == xosd_display(osd, 0, XOSD_string, "Blah")) {
    printerror();
  }

  if (0 != xosd_wait_until_no_display(osd)) {
    printerror();
  }

  sleep(2);

  if (-1 == xosd_display(osd, 0, XOSD_string, "blah2")) {
    printerror();
  }

  sleep(1);

  if (-1 == xosd_display(osd, 1, XOSD_string, "wibble")) {
    printerror();
  }

  sleep(1);

  if (0 != xosd_scroll(osd, 1)) {
    printerror();
  }

  if (-1 == xosd_display(osd, 1, XOSD_string, "bloggy")) {
    printerror();
  }

  sleep(1);

  if (0 != xosd_scroll(osd, 1)) {
    printerror();
  }

  if (0 != xosd_scroll(osd, 1)) {
    printerror();
  }

  if (0 != xosd_destroy(osd)) {
    printerror();
  }


  return EXIT_SUCCESS;
}
