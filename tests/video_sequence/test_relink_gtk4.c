/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_relink_gtk4.c
 * PURPOSE: Exercise native relinking, selection stability and retained-control admission.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../adapters/gtk4/video_sequence_private.h"
#include "umicom/ui/gtk4/automation.h"
#define NEW_PATH VIDEO_DIRECTORY "/moved clip.mp4"
#define OTHER_PATH VIDEO_DIRECTORY "/other clip.mp4"
static GtkWidget *Find(GtkWidget *root, const char *id) {
  GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, id);
  CHECK(widget != NULL);
  return widget;
}
static void Click(GtkWidget *root, const char *id) {
  g_signal_emit_by_name(Find(root, id), "clicked");
}
static void Field(GtkWidget *root, const char *id, const char *value) {
  gtk_editable_set_text(GTK_EDITABLE(Find(root, id)), value);
}
static void Selected(UmiVideoSequencePanel *panel, const char *id) {
  const UmiVideoSequence *value = UmiVideoSequenceEditorRead(panel->editor);
  guint position = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector));
  CHECK((size_t)position < value->clip_count);
  CHECK(strcmp(value->clips[position].id, id) == 0);
  CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->id)), id) == 0);
}
static void Reenter(GObject *object, GParamSpec *spec, gpointer root) {
  (void)object;
  (void)spec;
  Click(GTK_WIDGET(root), "video.remove");
  Click(GTK_WIDGET(root), "video.relink");
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  (void)g_setenv("GTK_A11Y", "test", TRUE);
  (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
  if (!gtk_init_check())
    return 77;
  GtkWidget *root = UmiVideoSequenceGtk4Create();
  CHECK(root != NULL);
  g_object_ref_sink(root);
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(root);
  UmiVideoSequence initial = Sequence();
  CHECK(UmiVideoSequenceSplit(&initial, "first", 3000U, "second") == UMI_STATUS_OK);
  UmiVideoSequenceClip third = initial.clips[1];
  strcpy(third.id, "third");
  strcpy(third.path, OTHER_PATH);
  CHECK(UmiVideoSequencePut(&initial, &third, false) == UMI_STATUS_OK);
  CHECK(UmiVideoSequenceEditorApply(panel->editor, &initial) == UMI_STATUS_OK);
  panel->saved = initial;
  panel->dirty = false;
  UmiVideoSequenceGtkRefresh(panel);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->selector), 1U);
  Field(root, "video.relink-path", NEW_PATH);
  if (strcmp(argv[1], "selected") == 0 || strcmp(argv[1], "shared") == 0 ||
      strcmp(argv[1], "reentry") == 0 || strcmp(argv[1], "draft") == 0) {
    bool all = strcmp(argv[1], "shared") == 0;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->relinkAll), all);
    gulong observer = 0U;
    if (strcmp(argv[1], "reentry") == 0)
      observer = g_signal_connect(panel->selector, "notify::model", G_CALLBACK(Reenter), root);
    if (strcmp(argv[1], "draft") == 0) {
      Field(root, "video.id", "not-applied");
      Field(root, "video.path", OTHER_PATH);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->begin), 4999.0);
    }
    Click(root, "video.relink");
    if (observer != 0U)
      g_signal_handler_disconnect(panel->selector, observer);
    const UmiVideoSequence *value = UmiVideoSequenceEditorRead(panel->editor);
    CHECK(value->clip_count == 3U && panel->dirty);
    CHECK(strcmp(value->clips[0].path, all ? NEW_PATH : VIDEO_PATH) == 0);
    CHECK(strcmp(value->clips[1].path, NEW_PATH) == 0);
    CHECK(strcmp(value->clips[2].path, OTHER_PATH) == 0);
    CHECK(value->clips[1].begin_ms == 3000U && value->clips[1].end_ms == 5000U);
    Selected(panel, "second");
    Click(root, "video.undo");
    CHECK(!panel->dirty && UmiVideoSequenceEqual(UmiVideoSequenceEditorRead(panel->editor), &initial));
    Selected(panel, "second");
    Click(root, "video.redo");
    CHECK(panel->dirty);
    Selected(panel, "second");
  } else if (strcmp(argv[1], "invalid") == 0) {
    Field(root, "video.relink-path", "relative.mp4");
    Click(root, "video.relink");
    CHECK(!panel->dirty && UmiVideoSequenceEqual(UmiVideoSequenceEditorRead(panel->editor), &initial));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(panel->status)), "Relink not applied") != NULL);
    Selected(panel, "second");
  } else if (strcmp(argv[1], "busy") == 0 || strcmp(argv[1], "retired") == 0) {
    if (strcmp(argv[1], "busy") == 0)
      panel->busy = true;
    else
      UmiVideoSequenceGtk4Retire(root);
    Click(root, "video.relink");
    CHECK(!panel->dirty && UmiVideoSequenceEqual(UmiVideoSequenceEditorRead(panel->editor), &initial));
    panel->busy = false;
  } else if (strcmp(argv[1], "selection-move") == 0) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->position), 3.0);
    Click(root, "video.move");
    Selected(panel, "second");
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector)) == 2U);
    Click(root, "video.undo");
    Selected(panel, "second");
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector)) == 1U);
    Click(root, "video.redo");
    Selected(panel, "second");
  } else if (strcmp(argv[1], "selection-remove") == 0) {
    Click(root, "video.remove");
    Selected(panel, "third");
    Click(root, "video.remove");
    Selected(panel, "first");
    Click(root, "video.remove");
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector)) == GTK_INVALID_LIST_POSITION);
  } else if (strcmp(argv[1], "empty") == 0) {
    UmiVideoSequence empty = {0};
    CHECK(UmiVideoSequenceInit("Empty", 1280U, 720U, &empty) == UMI_STATUS_OK);
    CHECK(UmiVideoSequenceEditorApply(panel->editor, &empty) == UMI_STATUS_OK);
    UmiVideoSequenceGtkRefresh(panel);
    Click(root, "video.relink");
    CHECK(UmiVideoSequenceEqual(UmiVideoSequenceEditorRead(panel->editor), &empty));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(panel->status)), "Select an added clip") != NULL);
  } else
    return 2;
  UmiVideoSequenceGtk4Retire(root);
  g_object_unref(root);
  return 0;
}
