/* Hacked version of osd_cat to allow display of percentage and slider bars
   Modifed by Phil Gees (philgees@hotmail.com)
   Original code by:
   Andre Renaud <andre@ignavus.net>
   Tim Wright <tim@ignavus.net>
   
   Modified without permission.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xosd.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <sys/time.h>

static struct option long_options[] = {
  {"help", 0, NULL, 'h'},
  {"font", 1, NULL, 'f'},
  {"color", 1, NULL, 'c'},
  {"colour", 1, NULL, 'c'},
  {"indent", 1, NULL, 'i'},
  {"delay", 1, NULL, 'd'},
  {"offset", 1, NULL, 'o'},
  {"pos", 1, NULL, 'p'},
  {"align", 1, NULL, 'A'},
  {"shadow", 1, NULL, 's'},
  {"shadowcolour", 1, NULL, 'S'},
  {"age", 2, NULL, 'a'},
  {"lines", 1, NULL, 'l'},
  {"wait", 0, NULL, 'w'},
  {"outline", 1, NULL, 'O'},
  {"outlinecolour", 1, NULL, 'u'},
  {"barmode", 1, NULL, 'b'},
  {"percentage", 1, NULL, 'P'},
  {"text", 1, NULL, 'T'},
  {NULL, 0, NULL, 0}
};

FILE *fp;
xosd *osd;
char buffer[1024];

char *font = NULL;
char *colour = "red";
char *text = NULL;
enum { bar_none=0, bar_percentage, bar_slider } barmode = bar_none;
int percentage = 50;
int outline_offset = 0;
char *outline_colour = NULL;
char *shadow_colour = NULL;
int delay = 5;
int forcewait = 0;
xosd_pos pos = XOSD_top;
int voffset = 0;
int hoffset = 0;
int shadow = 0;
int scroll_age = 0;
struct timeval old_age, new_age;
int screen_line = 0;
int lines = 5;
xosd_align align = XOSD_left;

int
main(int argc, char *argv[])
{
  if (setlocale(LC_ALL, "") == NULL || !XSupportsLocale())
    fprintf(stderr, "Locale not available, expect problems with fonts.\n");

  while (1) {
    int option_index = 0;
    int c =
      getopt_long(argc, argv, "l:A:a::f:c:d:o:i:s:p:O:S:u:b:P:T:hw",
                  long_options,
                  &option_index);
    if (c == -1)
      break;
    switch (c) {
    case 'a':
      scroll_age = optarg ? atoi(optarg) : delay;
      break;
    case 'w':
      forcewait = 1;
      break;
    case 'A':
      if (strcasecmp(optarg, "left") == 0) {
        align = XOSD_left;
      } else if (strcasecmp(optarg, "right") == 0) {
        align = XOSD_right;
      } else if (strcasecmp(optarg, "center") == 0) {
        align = XOSD_center;
      } else if (strcasecmp(optarg, "centre") == 0) {
        align = XOSD_center;
      } else {
        fprintf(stderr, "Unknown alignment: %s\n", optarg);
        return EXIT_FAILURE;
      }
      break;
    case 'p':
      if (strcasecmp(optarg, "top") == 0) {
        pos = XOSD_top;
      } else if (strcasecmp(optarg, "middle") == 0) {
        pos = XOSD_middle;
      } else if (strcasecmp(optarg, "bottom") == 0) {
        pos = XOSD_bottom;
      } else {
        fprintf(stderr, "Unknown alignment: %s\n", optarg);
        return EXIT_FAILURE;
      }
      break;
    case 'f':
      font = optarg;
      break;
    case 'c':
      colour = optarg;
      break;
    case 'd':
      delay = atoi(optarg);
      break;
    case 'o':
      voffset = atoi(optarg);
      break;
    case 'O':
      outline_offset = atoi(optarg);
      break;

    case 'S':
      shadow_colour = optarg;
      break;
    case 'u':
      outline_colour = optarg;
      break;

    case 'i':
      hoffset = atoi(optarg);
      break;
    case 's':
      shadow = atoi(optarg);
      break;
    case 'l':
      lines = atoi(optarg);
      if (lines <= 0) {
        fprintf(stderr, "Illegal number of lines: %d\n", lines);
        return EXIT_FAILURE;
      }
      break;

    case 'b':
      if (strcasecmp(optarg, "percentage") == 0) {
        barmode = bar_percentage;
      } else if (strcasecmp(optarg, "slider") == 0) {
        barmode = bar_slider;
      } else {
        fprintf(stderr, "Unknown barmode: %s.\n", optarg);
        return EXIT_FAILURE;
      }
      break;
    case 'P':
      percentage = atoi(optarg);
      break;
    case 'T':
      text = optarg;
      break;

    case '?':
    case 'h':
    default:
      fprintf(stderr, "Usage: %s [OPTION] [FILE]...\n", argv[0]);
      fprintf(stderr, "Version: %s\n", XOSD_VERSION);
      fprintf(stderr,
          "Display FILE, or standard input, on top of display.\n"
          "\n"
          "  -h, --help          Show this help\n"
          "  -p, --pos=(top|middle|bottom)\n"
          "                      Display at top/middle/bottom of screen. Top is default\n"
          "  -o, --offset=OFFSET Vertical Offset\n"
          "  -A, --align=(left|right|center)\n"
          "                      Display at left/right/center of screen.Left is default\n"
          "  -i, --indent=OFFSET Horizontal Offset\n"
          "  -f, --font=FONT     Use font (default: %s)\n",
          osd_default_font);
      fprintf(stderr, "  -c, --colour=COLOUR Use colour\n"
          "  -s, --shadow=OFFSET Offset of shadow, default is 0 which is no shadow\n"
          "  -S, --shadowcolour=COLOUR\n"
          "                      Colour of shadow, default is black\n"
          "  -O, --outline=WIDTH\n"
          "                      Offset of outline, default is 0 which is no outline\n"
          "  -u, --outlinecolour=COLOUR\n"
          "                      Colour of outline, default is black\n"
          "  -a, --age[=TIME]    Time in seconds before old scroll lines are discarded\n"
          "                      If no time is given, the current DELAY is used.\n"
          "  -l, --lines=N       Scroll using n lines. Default is 5.\n"
          "  -d, --delay=TIME    Show for specified time\n"
          "  -w, --wait          Delay display even when new lines are ready\n"
          "\n"
          "  -b, --barmode=(percentage|slider)\n"
          "                      Lets you display a percentage or slider bar instead of just text.\n"
          "                      Options may be 'percentage' or 'slider'.\n"
          "                      Disregards any text or files when used.\n"
          "                      When this option is used, the following options are also valid.\n"
          "  -P, --percentage=PERCENTAGE\n"
          "                      The length of the percentage bar / slider position (0 to 100).\n"
          "  -T, --text=TEXT     The text to get displayed above the percentage bar.\n"
          "\n\n"
          "With no FILE, or when FILE is -, read standard input.\n");
      return EXIT_SUCCESS;
    }
  }

  if (barmode) {
    osd = xosd_create( (text && *text) ? 2 : 1);
  } else {
    if ((optind < argc) && strncmp(argv[optind], "-", 2)) {
      if ((fp = fopen(argv[optind], "r")) == NULL) {
        fprintf(stderr, "Unable to open: %s\n", argv[optind]);
        return EXIT_FAILURE;
      }
    } else
      fp = stdin;

    osd = xosd_create(lines);
  }

  if (!osd) {
    fprintf(stderr, "Error initializing osd: %s\n", xosd_error);
    return EXIT_FAILURE;
  }
  xosd_set_shadow_offset(osd, shadow);
  if (shadow_colour) xosd_set_shadow_colour(osd, shadow_colour);
  xosd_set_outline_offset(osd, outline_offset);
  if (outline_colour) xosd_set_outline_colour(osd, outline_colour);
  if (colour) xosd_set_colour(osd, colour);
  xosd_set_timeout(osd, delay);
  xosd_set_pos(osd, pos);
  xosd_set_vertical_offset(osd, voffset);
  xosd_set_horizontal_offset(osd, hoffset);
  xosd_set_align(osd, align);
  if (font && xosd_set_font(osd, font)) {
    /* This is critical, because fontset=NULL, will segfault later! */
    fprintf(stderr, "ABORT: %s\n", xosd_error);
    return EXIT_FAILURE;
  }

  switch (barmode) {
    case bar_percentage:
      if (text) xosd_display(osd, 0, XOSD_string, text);
      xosd_display(osd, text ? 1 : 0, XOSD_percentage, percentage);
      break;
    case bar_slider:
      if (text) xosd_display(osd, 0, XOSD_string, text);
      xosd_display(osd, text ? 1 : 0, XOSD_slider, percentage);
      break;
    case bar_none:
      /* Not really needed, but at least we aren't throwing around an unknown value */
      old_age.tv_sec = 0;

      if (scroll_age)
        gettimeofday(&old_age, 0);

      while (!feof(fp)) {
        if (fgets(buffer, sizeof(buffer) - 1, fp)) {
          char *newline = strchr(buffer, '\n');
          if (newline)
            newline[0] = '\0';

          /* Enforce delay even when new lines are available */
          if (forcewait && xosd_is_onscreen(osd))
            xosd_wait_until_no_display(osd);

          /* If more than scroll_age time passes after the last time somethings
           * was displayed, clear the full display by scolling off all lines at
           * once. */
          if (scroll_age) {
            gettimeofday(&new_age, 0);
            if ((new_age.tv_sec - old_age.tv_sec) > scroll_age) {
              if (lines > 1)
                xosd_scroll(osd, lines);
              screen_line = 0;
            }
          }
          /* else scroll of the first line if the display is full. */
          if (screen_line >= lines) {
            if (lines > 1)
              xosd_scroll(osd, 1);
            screen_line = lines - 1;
          }

          xosd_display(osd, screen_line++, XOSD_string, buffer);

          old_age.tv_sec = new_age.tv_sec;
        } else if (!feof(fp)) {
          fprintf(stderr, "Error occured reading input file: %s\n",
              strerror(errno));
          exit(1);
        }
      }
      fclose(fp);
      break;
  }

  if (xosd_is_onscreen(osd))
    xosd_wait_until_no_display(osd);
  xosd_destroy(osd);

  return EXIT_SUCCESS;
}
