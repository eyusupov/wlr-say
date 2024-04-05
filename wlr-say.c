#include <stdbool.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>
#include <pango/pango-types.h>
#include <wayland-client.h>
#include "wlr-layer-shell-unstable-v1.h"

struct zwlr_layer_shell_v1 *shell;

const int SLEEP_SECONDS = 3;

// TODO - this is only to make wlr shell protocol link:
const struct wl_interface xdg_popup_interface;

void add_global(void *data, struct wl_registry *wl_registry, uint32_t name, const char *interface, uint32_t version) {
  if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
    shell = wl_registry_bind(wl_registry, name, &zwlr_layer_shell_v1_interface, version);
  }
}

void config_surface(void *data,
      struct zwlr_layer_surface_v1 *zwlr_layer_surface_v1,
      uint32_t serial,
      uint32_t width,
      uint32_t height) {
  zwlr_layer_surface_v1_ack_configure(zwlr_layer_surface_v1, serial);
}

gboolean autoquit(gpointer data) {
  gtk_widget_destroy(GTK_WIDGET(data));
  return false;
}

int main(int argc, char **argv) {
	gtk_init(&argc, &argv);

  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect (win, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  g_timeout_add_seconds(SLEEP_SECONDS, autoquit, win);

  gtk_widget_set_name(win, "wlr-say");
  gtk_widget_realize(win);

  struct wl_display *wl = gdk_wayland_display_get_wl_display(gdk_display_get_default());
  struct wl_registry *registry = wl_display_get_registry(wl);

  struct wl_registry_listener wl_registry_listener = {
    .global = add_global,
    .global_remove = NULL
  };
  wl_registry_add_listener(registry, &wl_registry_listener, NULL);

  GdkWindow* gdk_win = gtk_widget_get_window(win);

  gdk_wayland_window_set_use_custom_surface(gdk_win);
  struct wl_surface *wl_s = gdk_wayland_window_get_wl_surface(gdk_win);

  enum zwlr_layer_shell_v1_layer wlr_layer = ZWLR_LAYER_SHELL_V1_LAYER_TOP;
  struct zwlr_layer_surface_v1 *wlr_s;

  wl_display_roundtrip(wl);

  wlr_s = zwlr_layer_shell_v1_get_layer_surface(shell, wl_s, NULL, wlr_layer, "wlr-say");

  GtkWidget *label = gtk_label_new(argv[1]);
  GtkCssProvider *css_provider = gtk_css_provider_new();

  gtk_css_provider_load_from_path(css_provider, "style.css", NULL);
  gtk_style_context_add_provider(gtk_widget_get_style_context(win), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_style_context_add_provider(gtk_widget_get_style_context(label), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  GtkRequisition min, nat;
  gtk_widget_set_visible(label, true);
  gtk_widget_get_preferred_size (label, &min, &nat);

  gtk_window_set_default_size(GTK_WINDOW(win), nat.width, nat.height);

  struct zwlr_layer_surface_v1_listener wlr_surface_listsener = {
    .configure = config_surface,
    .closed = NULL
  };
  zwlr_layer_surface_v1_add_listener(wlr_s, &wlr_surface_listsener, NULL);

  zwlr_layer_surface_v1_set_size(wlr_s, nat.width, nat.height);
  zwlr_layer_surface_v1_set_anchor(wlr_s, 0);

  wl_surface_commit(wl_s);
  wl_display_roundtrip(wl);

  gtk_container_add(GTK_CONTAINER(win), label);
  gtk_widget_show_all(win);
  gtk_main();
}