/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_publication_plan/test_native.c
 *
 * PURPOSE:
 *   Verify worker admission, explicit approval and late-result retirement for saved public plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "../support/main_context.h"
#include "fixture.h"
#include "umicom/kanban/publication_plan_storage.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/kanban_publication_store.h"
typedef struct Context {
  UmiKanbanBoard *board;
  GtkWidget *panel;
  unsigned captures, adopts, destroyed;
  bool deny, retire;
} Context;
static UmiStatus Capture(gpointer data, UmiKanbanPublicationPlan **out) {
  Context *c = data;
  ++c->captures;
  if (c->retire)
    UmiKanbanPublicPlanGtk4Retire(c->panel);
  *out = Plan(c->board);
  return UMI_STATUS_OK;
}
static UmiStatus Adopt(gpointer data, const UmiKanbanPublicationPlan *plan) {
  Context *c = data;
  ++c->adopts;
  if (c->deny)
    return UMI_STATUS_PERMISSION_DENIED;
  UmiKanbanPublicationPlan *reviewed = NULL;
  UmiStatus status = UmiKanbanPublicationPlanReview(plan, c->board,
                                                    umi_kanban_board_revision(c->board), &reviewed);
  UmiKanbanPublicationPlanDestroy(reviewed);
  return status;
}
static void Destroy(gpointer data) { ++((Context *)data)->destroyed; }
static GtkWidget *Find(GtkWidget *root, const char *tag) {
  GtkWidget *w = umi_gtk4_automation_find_tagged_widget(root, tag);
  REQUIRE(w != NULL);
  return w;
}
static void Click(GtkWidget *root, const char *tag) {
  g_signal_emit_by_name(Find(root, tag), "clicked");
}
/* Retain the former drain for review. Bounded dispatch lets the fixture
 * observe pending completion even when GTK keeps scheduling native events. */
#if 0
static void Drain(void) {
  for (unsigned i = 0U; i < 30U; ++i) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000U);
  }
}
static void Wait(GtkWidget *root) {
  gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
  while (UmiKanbanPublicPlanGtk4Pending(root) && g_get_monotonic_time() < end) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000U);
  }
  REQUIRE(!UmiKanbanPublicPlanGtk4Pending(root));
  Drain();
}
#endif
static void Drain(void) {
  for (unsigned i = 0U; i < 30U; ++i) {
    (void)UmiTestDispatchSlice(NULL, g_get_monotonic_time() + 1000, 32U);
    g_usleep(1000U);
  }
}
static void Wait(GtkWidget *root) {
  gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
  while (UmiKanbanPublicPlanGtk4Pending(root) && g_get_monotonic_time() < end) {
    (void)UmiTestDispatchSlice(NULL, end, 32U);
    g_usleep(1000U);
  }
  REQUIRE(!UmiKanbanPublicPlanGtk4Pending(root));
  Drain();
}
/* An always-ready source reproduces the starvation hazard without relying on
 * display timing. The fixture must return after its explicit dispatch budget. */
static gboolean KeepReady(gpointer data)
{
  unsigned *calls = data;
  ++*calls;
  return G_SOURCE_CONTINUE;
}
static void CheckDispatchBudget(void)
{
  GMainContext *context = g_main_context_new();
  gint64 deadline = g_get_monotonic_time() + G_TIME_SPAN_SECOND;
  REQUIRE(UmiTestDispatchSlice(context, deadline, 8U) == 0U);
  unsigned calls = 0U;
  GSource *source = g_idle_source_new();
  g_source_set_callback(source, KeepReady, &calls, NULL);
  REQUIRE(g_source_attach(source, context) != 0U);
  REQUIRE(UmiTestDispatchSlice(context, deadline, 0U) == 0U);
  REQUIRE(UmiTestDispatchSlice(context, 0, 8U) == 0U);
  REQUIRE(calls == 0U);
  REQUIRE(UmiTestDispatchSlice(context, deadline, 8U) == 8U);
  REQUIRE(calls == 8U);
  g_source_destroy(source);
  g_source_unref(source);
  REQUIRE(UmiTestDispatchSlice(context, deadline, 8U) == 0U);
  g_main_context_unref(context);
}
int main(int argc, char **argv) {
  REQUIRE(argc == 2);
  const char *name = argv[1],
             *cases[] = {"normal",       "no-approval",    "load-only",     "save",
                         "denied",       "busy",           "retired",       "retained",
                         "close-load",   "hide-show",      "captured-path", "discard",
                         "invalid-file", "capture-retire", "null", "dispatch-budget", "retire-focus", "retire-other-focus"};
  CaseKnown(name, cases, sizeof cases / sizeof cases[0]);
  if (!strcmp(name, "dispatch-budget")) {
    CheckDispatchBudget();
    return 0;
  }
  if (!gtk_init_check())
    return 77;
  Context context = {0};
  context.board = PlanBoard();
  char directory[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  UmiKanbanPublicationPlan *plan = Plan(context.board);
  REQUIRE(UmiKanbanPublicationPlanSave(directory, "reviewed.umipublic", plan) == UMI_STATUS_OK);
  UmiKanbanPublicationPlanDestroy(plan);
  context.panel = UmiKanbanPublicPlanGtk4Create(Capture, Adopt, &context, Destroy);
  REQUIRE(context.panel != NULL);
  g_object_ref_sink(context.panel);
  if (!strcmp(name, "null")) {
    REQUIRE(UmiKanbanPublicPlanGtk4Create(NULL, Adopt, NULL, NULL) == NULL);
    REQUIRE(!UmiKanbanPublicPlanGtk4Pending(NULL));
  }
  GtkWindow *window = GTK_WINDOW(gtk_window_new());
  GtkWidget *layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  GtkWidget *outside = gtk_entry_new();
  gtk_box_append(GTK_BOX(layout), context.panel);
  gtk_box_append(GTK_BOX(layout), outside);
  gtk_window_set_child(window, layout);
  gtk_window_present(window);
  Drain();
  GtkWidget *path = Find(context.panel, "kanban.plan.directory");
  gtk_editable_set_text(GTK_EDITABLE(path), directory);
  GtkWidget *file = Find(context.panel, "kanban.plan.filename");
  GtkWidget *load = Find(context.panel, "kanban.plan.load");
  g_object_ref(load);
  GtkTextBuffer *buffer = g_object_ref(
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(context.panel, "kanban.plan.preview"))));
  bool destroyed = false;
  if (!strcmp(name, "save") || !strcmp(name, "capture-retire")) {
    gtk_editable_set_text(GTK_EDITABLE(file), "saved.umipublic");
    context.retire = !strcmp(name, "capture-retire");
    Click(context.panel, "kanban.plan.save");
    Wait(context.panel);
    REQUIRE(context.captures == 1U);
    UmiKanbanPublicationPlan *saved = NULL;
    UmiStatus status = UmiKanbanPublicationPlanLoad(directory, "saved.umipublic", &saved);
    REQUIRE((status == UMI_STATUS_OK) == !context.retire);
    UmiKanbanPublicationPlanDestroy(saved);
  } else {
    if (!strcmp(name, "busy"))
      UmiKanbanPublicPlanGtk4SetBusy(context.panel, TRUE);
    if (!strcmp(name, "retired") || !strcmp(name, "retire-focus") ||
        !strcmp(name, "retire-other-focus")) {
      GtkWidget *target = !strcmp(name, "retire-other-focus") ? outside : path;
      REQUIRE(gtk_widget_grab_focus(target));
      GtkWidget *focus = gtk_root_get_focus(GTK_ROOT(window));
      REQUIRE(focus != NULL &&
              (focus == target || gtk_widget_is_ancestor(focus, target)));
      UmiKanbanPublicPlanGtk4Retire(context.panel);
      if (target == outside)
        REQUIRE(gtk_root_get_focus(GTK_ROOT(window)) == focus);
      else
        REQUIRE(gtk_root_get_focus(GTK_ROOT(window)) == NULL);
    }
    if (!strcmp(name, "retained")) {
      gtk_window_destroy(window);
      destroyed = true;
    }
    if (!strcmp(name, "invalid-file"))
      gtk_editable_set_text(GTK_EDITABLE(file), "missing.umipublic");
    g_signal_emit_by_name(load, "clicked");
    if (!strcmp(name, "close-load")) {
      gtk_window_destroy(window);
      destroyed = true;
    }
    if (!strcmp(name, "hide-show")) {
      gtk_widget_set_visible(context.panel, FALSE);
      gtk_widget_set_visible(context.panel, TRUE);
    }
    if (!strcmp(name, "captured-path"))
      gtk_editable_set_text(GTK_EDITABLE(file), "not-the-loaded-file.umipublic");
    Wait(context.panel);
    bool blank = !strcmp(name, "busy") || !strcmp(name, "retired") ||
                 !strcmp(name, "retire-focus") || !strcmp(name, "retire-other-focus") ||
                 !strcmp(name, "retained") ||
                 !strcmp(name, "close-load") || !strcmp(name, "hide-show") ||
                 !strcmp(name, "invalid-file");
    REQUIRE((gtk_text_buffer_get_char_count(buffer) == 0) == blank);
    REQUIRE(context.adopts == 0U && context.captures == 0U);
    if (!blank) {
      if (!strcmp(name, "discard")) {
        Click(context.panel, "kanban.plan.discard");
        REQUIRE(gtk_text_buffer_get_char_count(buffer) == 0);
      } else if (strcmp(name, "load-only")) {
        context.deny = !strcmp(name, "denied");
        if (strcmp(name, "no-approval"))
          gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(context.panel, "kanban.plan.approval")),
                                      TRUE);
        Click(context.panel, "kanban.plan.adopt");
        REQUIRE(context.adopts == (!strcmp(name, "no-approval") ? 0U : 1U));
        REQUIRE(!gtk_check_button_get_active(
            GTK_CHECK_BUTTON(Find(context.panel, "kanban.plan.approval"))));
      }
    }
  }
  if (!destroyed)
    gtk_window_destroy(window);
  g_object_unref(load);
  g_object_unref(buffer);
  g_object_unref(context.panel);
  Drain();
  REQUIRE(context.destroyed == 1U);
  umi_kanban_board_destroy(context.board);
  return 0;
}
