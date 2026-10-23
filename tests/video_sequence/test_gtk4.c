/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_gtk4.c
 * PURPOSE: Exercise recipe controls and asynchronous ownership without requiring media codecs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#undef CHECK
#include "../../adapters/gtk4/video_sequence_private.h"
#include "fixture.h"
#include "umicom/platform/directory.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/creative_workspace.h"
/* The logical widget tree includes controls in collapsed expanders. */
static GtkWidget *Find(GtkWidget *root, const char *id) {
  return umi_gtk4_automation_find_tagged_widget(root, id);
}
static void Click(GtkWidget *root, const char *id) {
  GtkWidget *button = Find(root, id);
  CHECK(button != NULL);
  g_signal_emit_by_name(button, "clicked");
}
static void Field(GtkWidget *root, const char *id, const char *value) {
  GtkWidget *entry = Find(root, id);
  CHECK(entry != NULL);
  gtk_editable_set_text(GTK_EDITABLE(entry), value);
}
static void Wait(UmiVideoSequencePanel *panel) {
  gint64 deadline = g_get_monotonic_time() + 10000000;
  while (panel->busy) {
    CHECK(g_get_monotonic_time() < deadline);
    (void)g_main_context_iteration(NULL, FALSE);
    g_usleep(1000U);
  }
}
typedef struct Reentry {
  GtkWidget *root;
  unsigned calls;
} Reentry;
/* A synchronous observer can click even when a widget is insensitive. The
 * editor must retain its captured operation until result publication finishes. */
static void Notify(GObject *object, GParamSpec *property, gpointer data) {
  (void)object;
  (void)property;
  Reentry *observer = data;
  ++observer->calls;
  Click(observer->root, "video.remove");
  Click(observer->root, "video.save");
}
typedef struct Retirement {
  GtkWidget *root;
  bool onCompletion, fired;
  unsigned updatesAfterRetirement;
} Retirement;
static void RetireOnSensitivity(GObject *object, GParamSpec *property, gpointer data) {
  (void)property;
  Retirement *observer = data;
  if (!observer->fired && (bool)gtk_widget_get_sensitive(GTK_WIDGET(object)) == observer->onCompletion) {
    observer->fired = true;
    UmiVideoSequenceGtk4Retire(observer->root);
  }
}
static void CountAfterRetirement(GObject *object, GParamSpec *property, gpointer data) {
  (void)object; (void)property;
  Retirement *observer = data;
  if (observer->fired) ++observer->updatesAfterRetirement;
}
static void RetireOnBuffer(GtkTextBuffer *buffer, gpointer data) {
  (void)buffer;
  Retirement *observer = data;
  if (!observer->fired) {
    observer->fired = true;
    UmiVideoSequenceGtk4Retire(observer->root);
  }
}
int main(int argc, char **argv) {
  if (argc != 2 && argc != 3)
    return 2;
  (void)g_setenv("GTK_A11Y", "test", TRUE);
  (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
  if (!gtk_init_check())
    return 77;
  if (strcmp(argv[1], "composition") == 0) {
    UmiCreativeGtkPanel *host = NULL;
    CHECK(UmiCreativeGtkPanelCreate(NULL, "media", &host) == UMI_STATUS_OK);
    GtkWidget *video = Find(UmiCreativeGtkPanelWidget(host), "video-sequence");
    CHECK(video != NULL);
    g_object_ref(video);
    UmiCreativeGtkPanelDestroy(host);
    CHECK(UmiVideoSequenceGtkState(video)->closed);
    g_object_unref(video);
    return 0;
  }
  GtkWidget *root = UmiVideoSequenceGtk4Create();
  CHECK(root != NULL);
  g_object_ref_sink(root);
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(root);
  char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], changed[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  FixturePath(path, directory, "recipe.json");
  FixturePath(changed, directory, "missing.json");
  Field(root, "video.id", "first");
  Field(root, "video.path", VIDEO_PATH);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->end), 5000.0);
  Click(root, "video.add");
  CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
  Field(root, "video.recipe", path);
  if (strcmp(argv[1], "captions-export") == 0 || strcmp(argv[1], "soundtrack-export") == 0 ||
      strcmp(argv[1], "render-export") == 0) {
    CHECK(argc == 3);
    /* The fixture verifies dispatch and byte ownership, not codec support.
     * Actual encoder qualification decodes video, audio and caption streams. */
    Field(root, "video.program", argv[2]);
    const char *action, *outputId;
    if (strcmp(argv[1], "captions-export") == 0) {
      Field(root, "video.caption-video", VIDEO_PATH);
      Field(root, "video.caption-source", VIDEO_PATH);
      action = "video.captions";
      outputId = "video.caption-output";
    } else if (strcmp(argv[1], "soundtrack-export") == 0) {
      Field(root, "video.soundtrack-video", VIDEO_PATH);
      Field(root, "video.soundtrack-audio", VIDEO_PATH);
      action = "video.soundtrack";
      outputId = "video.soundtrack-output";
    } else {
      action = "video.render";
      outputId = "video.output";
    }
    Field(root, outputId, path);
    Click(root, action);
    CHECK(panel->busy);
    Field(root, outputId, changed);
    Field(root, "video.program", changed);
    Click(root, action);
    Wait(panel);
    size_t size = 0U;
    unsigned char *bytes = FixtureRead(path, &size);
    CHECK(size == 16U && memcmp(bytes + 4U, "ftyp", 4U) == 0);
    free(bytes);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(panel->status)), "Render finished"));
    CHECK(panel->dirty && panel->cancel == NULL);
    CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
    UmiFileInfo unexpected;
    CHECK(umi_directory_stat(changed, &unexpected) == UMI_STATUS_NOT_FOUND);
  } else if (strcmp(argv[1], "cancel-open") == 0) {
    Click(root, "video.save"); Wait(panel);
    Click(root, "video.remove");
    CHECK(panel->dirty && UmiVideoSequenceEditorRead(panel->editor)->clip_count == 0U);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), TRUE);
    Click(root, "video.open");
    CHECK(panel->busy && panel->cancel != NULL);
    /* No GTK iteration occurs before cancellation. Worker speed cannot make
     * acceptance precede this request, even when parsing has already finished. */
    umi_cancellation_token_request(panel->cancel);
    Wait(panel);
    CHECK(panel->dirty && panel->cancel == NULL);
    CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 0U);
    Click(root, "video.undo");
    CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
  } else if (strcmp(argv[1], "retire-dispatch") == 0 || strcmp(argv[1], "retire-complete") == 0) {
    Retirement observer = {root, strcmp(argv[1], "retire-complete") == 0, false, 0U};
    GtkWidget *form = g_object_ref(panel->form), *cancelButton = g_object_ref(panel->cancelButton);
    gulong retiring = g_signal_connect(form, "notify::sensitive", G_CALLBACK(RetireOnSensitivity), &observer);
    gulong checking = g_signal_connect(cancelButton, "notify::sensitive", G_CALLBACK(CountAfterRetirement), &observer);
    Click(root, "video.save"); Wait(panel);
    CHECK(observer.fired && observer.updatesAfterRetirement == 0U);
    CHECK(panel->closed && panel->cancel == NULL);
    UmiFileInfo info;
    CHECK(umi_directory_stat(path, &info) == (observer.onCompletion ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND));
    g_signal_handler_disconnect(form, retiring); g_signal_handler_disconnect(cancelButton, checking);
    g_object_unref(form); g_object_unref(cancelButton);
  } else if (strcmp(argv[1], "retire-refresh") == 0) {
    Retirement observer = {root, true, false, 0U};
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->summary)));
    GtkWidget *selector = g_object_ref(panel->selector);
    gulong retiring = g_signal_connect(buffer, "changed", G_CALLBACK(RetireOnBuffer), &observer);
    gulong checking = g_signal_connect(selector, "notify::model", G_CALLBACK(CountAfterRetirement), &observer);
    Click(root, "video.save"); Wait(panel);
    CHECK(observer.fired && panel->closed && observer.updatesAfterRetirement == 0U);
    CHECK(!panel->busy && !panel->refreshing && panel->cancel == NULL);
    g_signal_handler_disconnect(buffer, retiring); g_signal_handler_disconnect(selector, checking);
    g_object_unref(buffer); g_object_unref(selector);
  } else if (strcmp(argv[1], "saved-undo") == 0) {
    Click(root, "video.save");
    Wait(panel);
    CHECK(!panel->dirty);
    Click(root, "video.remove");
    CHECK(panel->dirty);
    Click(root, "video.undo");
    CHECK(!panel->dirty && UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
    Click(root, "video.redo");
    CHECK(panel->dirty && UmiVideoSequenceEditorRead(panel->editor)->clip_count == 0U);
  } else if (strcmp(argv[1], "completion-reentry") == 0) {
    Click(root, "video.save");
    Reentry observer = {root, 0U};
    gulong notify = g_signal_connect(panel->status, "notify::label", G_CALLBACK(Notify), &observer);
    Wait(panel);
    g_signal_handler_disconnect(panel->status, notify);
    CHECK(observer.calls != 0U);
    CHECK(!panel->busy && !panel->dirty && panel->cancel == NULL);
    CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
    size_t size = 0U;
    unsigned char *bytes = FixtureRead(path, &size);
    CHECK(size > 0U);
    free(bytes);
  } else if (strcmp(argv[1], "edits") == 0) {
    Field(root, "video.right-id", "right");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->split), 2000.0);
    Click(root, "video.split");
    CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 2U);
    Click(root, "video.undo");
    CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
    Click(root, "video.redo");
    CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 2U);
  } else if (strcmp(argv[1], "approval") == 0) {
    Click(root, "video.open");
    CHECK(!panel->busy && panel->dirty &&
          UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
  } else if (strcmp(argv[1], "failed-open") == 0) {
    Field(root, "video.recipe", changed);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), TRUE);
    Click(root, "video.open");
    Wait(panel);
    CHECK(panel->dirty && UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
  } else if (strcmp(argv[1], "retained-control") == 0) {
    GtkWidget *button = Find(root, "video.remove");
    g_object_ref(button);
    UmiVideoSequenceGtk4Retire(root);
    g_signal_emit_by_name(button, "clicked");
    CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
    g_object_unref(root);
    g_signal_emit_by_name(button, "clicked");
    g_object_unref(button);
    return 0;
  } else if (strcmp(argv[1], "save-open") == 0 || strcmp(argv[1], "captured") == 0 ||
             strcmp(argv[1], "close-busy") == 0) {
    Click(root, "video.save");
    CHECK(panel->busy);
    if (strcmp(argv[1], "close-busy") == 0) {
      UmiVideoSequenceGtk4Retire(root);
      Wait(panel);
      CHECK(panel->closed && panel->cancel == NULL);
    } else {
      /* Programmatic entry mutation bypasses widget sensitivity. The
       * accepted worker must still write only the captured filename. */
      Field(root, "video.recipe", changed);
      Wait(panel);
      size_t size = 0U;
      unsigned char *bytes = FixtureRead(path, &size);
      CHECK(size > 0U);
      free(bytes);
      CHECK(!panel->dirty);
      Click(root, "video.remove");
      CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 0U);
      Field(root, "video.recipe", path);
      gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), TRUE);
      Click(root, "video.open");
      Wait(panel);
      CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U && !panel->dirty);
      CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->id)), "first") == 0);
      CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->path)), VIDEO_PATH) == 0);
    }
  } else
    return 2;
  UmiVideoSequenceGtk4Retire(root);
  g_object_unref(root);
  return 0;
}
