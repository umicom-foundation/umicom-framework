/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_presets/test_set_gtk4.c
 *
 * PURPOSE:
 *   Exercise explicit matching-stage selection, review-only behavior and reentrant GTK form
 * ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "preset_set_fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/developer_dialog.h"
#include <glib/gstdio.h>
#undef CHECK
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
typedef struct SetProbe {
  UmiGtk4DeveloperDialog *dialog;
  GtkWidget *use, *accept, *build, *folder;
  const char *mode;
  unsigned accepted, notifications;
  UmiBuildProfile profile;
} SetProbe;
static UmiStatus AcceptSet(const UmiBuildProfile *profile, int trusted, void *context) {
  (void)trusted;
  SetProbe *probe = context;
  probe->profile = *profile;
  ++probe->accepted;
  return UMI_STATUS_OK;
}
static void DisposeSet(SetProbe *probe) {
  UmiGtk4DeveloperDialog *dialog = probe->dialog;
  probe->dialog = NULL;
  UmiGtk4DeveloperDialogDestroy(dialog);
}
static void ObserveSet(GtkEditable *entry, gpointer context) {
  SetProbe *probe = context;
  ++probe->notifications;
  g_signal_handlers_disconnect_by_data(entry, probe);
  if (strcmp(probe->mode, "close-notification") == 0)
    DisposeSet(probe);
  else if (strcmp(probe->mode, "edit-notification") == 0)
    gtk_editable_set_text(GTK_EDITABLE(probe->build), "owner-edit");
  else if (strcmp(probe->mode, "project-notification") == 0)
    gtk_editable_set_text(GTK_EDITABLE(probe->folder), "another-project");
  else {
    g_signal_emit_by_name(probe->use, "clicked");
    g_signal_emit_by_name(probe->accept, "clicked");
  }
}
static GtkWindow *SetForm(GtkWindow *parent) {
  GListModel *windows = gtk_window_get_toplevels();
  for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
    GtkWindow *window = g_list_model_get_item(windows, i);
    if (gtk_window_get_transient_for(window) == parent &&
        gtk_widget_get_visible(GTK_WIDGET(window)))
      return window;
    g_object_unref(window);
  }
  return NULL;
}
static int SetReady(GtkWidget *read) {
  gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
  while (!gtk_widget_get_sensitive(read) && g_get_monotonic_time() < deadline) {
    (void)g_main_context_iteration(NULL, FALSE);
    g_usleep(1000U);
  }
  return gtk_widget_get_sensitive(read);
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1], *cases[] = {"select",
                                          "apply",
                                          "ambiguous",
                                          "incomplete",
                                          "missing",
                                          "retained",
                                          "stale",
                                          "close-notification",
                                          "edit-notification",
                                          "project-notification",
                                          "reentrant-notification"};
  bool known = false;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(name, cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int failed = 0;
  SetProbe probe = {0};
  probe.mode = name;
  GtkWindow *parent = NULL, *form = NULL;
  GtkWidget *read = NULL, *picker = NULL, *entries[3] = {0};
  char *directory = NULL, *file = NULL;
  gpointer rootWeak = NULL;
  directory = g_dir_make_tmp("umicom-preset-set-XXXXXX", NULL);
  CHECK(directory != NULL);
  file = g_build_filename(directory, "CMakePresets.json", NULL);
  const char *json = preset_set_document;
  if (strcmp(name, "ambiguous") == 0)
    json = "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}],"
           "\"buildPresets\":[{\"name\":\"a\",\"configurePreset\":\"configure-notes\"},{\"name\":"
           "\"b\",\"configurePreset\":\"configure-notes\"}],"
           "\"testPresets\":[{\"name\":\"verify-notes\",\"configurePreset\":\"configure-notes\"}]}";
  if (strcmp(name, "incomplete") == 0)
    json = "{\"version\":6,\"include\":[\"other.json\"],\"configurePresets\":[{\"name\":"
           "\"configure-notes\"}]}";
  if (strcmp(name, "missing") == 0)
    json = "{\"version\":6,\"configurePresets\":[{\"name\":\"configure-notes\"}]}";
  CHECK(g_file_set_contents(file, json, -1, NULL));
  UmiBuildProfile initial;
  umi_build_profile_init(&initial);
  CHECK(g_strlcpy(initial.source_directory, directory, sizeof initial.source_directory) <
        sizeof initial.source_directory);
  strcpy(initial.configure_preset, "old-configure");
  strcpy(initial.build_preset, "old-build");
  strcpy(initial.test_preset, "old-test");
  parent = GTK_WINDOW(gtk_window_new());
  g_object_ref(parent);
  CHECK(UmiGtk4BuildSettingsDialogCreate(parent, &initial, 0, AcceptSet, &probe, &probe.dialog) ==
        UMI_STATUS_OK);
  form = SetForm(parent);
  CHECK(form != NULL);
  rootWeak = gtk_window_get_child(form);
  g_object_add_weak_pointer(G_OBJECT(rootWeak), &rootWeak);
  const char *ids[] = {"developer.presets.read",           "developer.presets.choices",
                       "developer.presets.use-set",        "developer.dialog.accept",
                       "developer.build.configure-preset", "developer.build.build-preset",
                       "developer.build.test-preset",      "developer.build.field.1"};
  GtkWidget **destinations[] = {&read,       &picker,     &probe.use,  &probe.accept,
                                &entries[0], &entries[1], &entries[2], &probe.folder};
  for (size_t i = 0U; i < sizeof ids / sizeof ids[0]; ++i) {
    *destinations[i] = umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(form), ids[i]);
    CHECK(*destinations[i] != NULL);
    g_object_ref(*destinations[i]);
  }
  probe.build = entries[1];
  CHECK(!gtk_widget_get_sensitive(probe.use));
  g_signal_emit_by_name(read, "clicked");
  CHECK(SetReady(read) && gtk_widget_get_sensitive(probe.use));
  CHECK(probe.accepted == 0U);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(picker), 0U);
  if (strcmp(name, "stale") == 0) {
    gtk_editable_set_text(GTK_EDITABLE(probe.folder), "another-project");
    gtk_editable_set_text(GTK_EDITABLE(probe.folder), directory);
  }
  if (strstr(name, "notification") != NULL)
    g_signal_connect(entries[0], "changed", G_CALLBACK(ObserveSet), &probe);
  if (strcmp(name, "retained") == 0)
    DisposeSet(&probe);
  g_signal_emit_by_name(probe.use, "clicked");
  CHECK(probe.accepted == 0U);
  if (strcmp(name, "close-notification") == 0) {
    CHECK(probe.dialog == NULL && probe.notifications == 1U);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[1])), "old-build") == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[2])), "old-test") == 0);
  } else if (strcmp(name, "edit-notification") == 0 || strcmp(name, "project-notification") == 0) {
    CHECK(probe.notifications == 1U);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[1])),
                 strcmp(name, "edit-notification") == 0 ? "owner-edit" : "old-build") == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[2])), "old-test") == 0);
  } else if (strcmp(name, "ambiguous") == 0 || strcmp(name, "incomplete") == 0 ||
             strcmp(name, "missing") == 0 || strcmp(name, "stale") == 0 ||
             strcmp(name, "retained") == 0) {
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[0])), "old-configure") == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[1])), "old-build") == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[2])), "old-test") == 0);
  } else {
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[0])), "configure-notes") == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[1])), "compile-notes") == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[2])), "verify-notes") == 0);
    if (strcmp(name, "reentrant-notification") == 0)
      CHECK(probe.notifications == 1U);
    if (strcmp(name, "apply") == 0) {
      g_signal_emit_by_name(probe.accept, "clicked");
      CHECK(probe.accepted == 1U);
      CHECK(strcmp(probe.profile.configure_preset, "configure-notes") == 0);
      CHECK(strcmp(probe.profile.build_preset, "compile-notes") == 0 &&
            strcmp(probe.profile.test_preset, "verify-notes") == 0);
    }
  }
cleanup:
  if (entries[0] != NULL)
    g_signal_handlers_disconnect_by_data(entries[0], &probe);
  DisposeSet(&probe);
  if (rootWeak != NULL) {
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (rootWeak != NULL && g_get_monotonic_time() < deadline) {
      (void)g_main_context_iteration(NULL, FALSE);
      g_usleep(1000U);
    }
    if (rootWeak != NULL) {
      g_object_remove_weak_pointer(G_OBJECT(rootWeak), &rootWeak);
      failed = 1;
    }
  }
  if (probe.use != NULL) {
    unsigned accepted = probe.accepted;
    g_signal_emit_by_name(probe.use, "clicked");
    if (probe.accepted != accepted)
      failed = 1;
  }
  GtkWidget *retained[] = {read,       picker,     probe.use,  probe.accept,
                           entries[0], entries[1], entries[2], probe.folder};
  for (size_t i = 0U; i < sizeof retained / sizeof retained[0]; ++i)
    if (retained[i] != NULL)
      g_object_unref(retained[i]);
  if (form != NULL)
    g_object_unref(form);
  if (parent != NULL) {
    gtk_window_destroy(parent);
    g_object_unref(parent);
  }
  if (file != NULL)
    (void)g_remove(file);
  if (directory != NULL)
    (void)g_rmdir(directory);
  g_free(file);
  g_free(directory);
  return failed;
}
