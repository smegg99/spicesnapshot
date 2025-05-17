// capture.c
#include "capture.h"

#include <spice-client-gtk-3.0/spice-client-gtk.h>

#include <gtk/gtk.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

typedef struct {
  SpiceDisplay * display;
  const char * output;
}
SnapshotData;

static void
on_display_mark(SpiceDisplayChannel * dchan G_GNUC_UNUSED,
  gint mark,
  gpointer user_data) {
  SnapshotData * sd = (SnapshotData * ) user_data;
  if (!mark)
    return;

  GdkPixbuf * pb = spice_display_get_pixbuf(sd -> display);
  if (pb) {
    gdk_pixbuf_save(pb, sd -> output, "png", NULL, NULL);
    g_object_unref(pb);
  }
  gtk_main_quit();
}

static void
on_channel_new(SpiceSession * session G_GNUC_UNUSED,
  SpiceChannel * chan,
  gpointer user_data) {
  if (SPICE_IS_DISPLAY_CHANNEL(chan)) {
    g_signal_connect(chan,
      "display-mark",
      G_CALLBACK(on_display_mark),
      user_data);
  }
}

int capture_spice_snapshot(const char * ip,
  int port,
  const char * output,
    const char * password) {
  gtk_init(NULL, NULL);

  gchar * uri = g_strdup_printf("spice://%s:%d", ip, port);
  SpiceSession * session = spice_session_new();
  if (password) {
    g_object_set(session, "password", password, NULL);
  }
  g_object_set(session, "uri", uri, NULL);
  g_free(uri);

  GtkWidget * window = gtk_offscreen_window_new();
  gtk_widget_set_size_request(window, 1024, 768);

  GtkWidget * display_widget = GTK_WIDGET(spice_display_new(session, 0));
  gtk_widget_set_size_request(display_widget, 1024, 768);
  gtk_container_add(GTK_CONTAINER(window), display_widget);

  gtk_widget_realize(window);
  gtk_widget_show_all(window);

  SpiceDisplay * display = SPICE_DISPLAY(display_widget);

  SnapshotData sd = {
    display,
    output
  };

  g_signal_connect(session,
    "channel-new",
    G_CALLBACK(on_channel_new), &
    sd);

  spice_session_connect(session);
  gtk_main();

  g_object_unref(session);
  return 0;
}