/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/test_async_owner.c
 *
 * PURPOSE:
 *   Release the host during private-file work and require all GTK finalization on its owner thread.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "native_fixture.h"
#include "../kanban_publication_plan/fixture.h"
#include "umicom/kanban/publication_plan_storage.h"
#include "umicom/ui/gtk4/kanban_publication_store.h"
typedef struct Owner {
    GThread *thread;
    UmiKanbanBoard *board;
    unsigned context_destroyed, widget_destroyed;
} Owner;
static void DestroyContext(gpointer data)
{
    Owner *owner = data;
    REQUIRE(g_thread_self() == owner->thread);
    ++owner->context_destroyed;
}
static void DestroyWidget(gpointer data, GObject *object)
{
    (void)object;
    Owner *owner = data;
    REQUIRE(g_thread_self() == owner->thread);
    ++owner->widget_destroyed;
}
static UmiStatus CaptureCompletion(gpointer data, const UmiKanbanCompletionReview *review,
    UmiKanbanCompletionPlan **out)
{
    Owner *owner = data;
    UmiKanbanCompletionRecord record; Record(&record, "alpha");
    memcpy(record.rationale, review->rationale, sizeof record.rationale);
    memcpy(record.evidence_sha256, review->evidence_sha256, sizeof record.evidence_sha256);
    record.decision = review->decision;
    return Capture(owner->board, &record, 1U, out);
}
static UmiStatus ApplyCompletion(gpointer data, const UmiKanbanCompletionPlan *plan)
{
    (void)data; (void)plan;
    REQUIRE(0); return UMI_STATUS_INTERNAL_ERROR;
}
static UmiStatus CapturePublic(gpointer data, UmiKanbanPublicationPlan **out)
{
    *out = Plan(((Owner *)data)->board);
    return UMI_STATUS_OK;
}
static UmiStatus ApplyPublic(gpointer data, const UmiKanbanPublicationPlan *plan)
{
    (void)data; (void)plan;
    REQUIRE(0); return UMI_STATUS_INTERNAL_ERROR;
}
static GtkWidget *Tagged(GtkWidget *root, const char *tag)
{
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, tag);
    REQUIRE(widget); return widget;
}
int main(int argc, char **argv)
{
    const char *const cases[] = {"completion-load", "completion-save", "completion-missing",
        "publication-load", "publication-save", "publication-missing"};
    REQUIRE(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    if (!gtk_init_check()) return 77;
    const bool completion = !strncmp(argv[1], "completion-", 11U);
    const bool save = strstr(argv[1], "-save") != NULL;
    const bool missing = strstr(argv[1], "-missing") != NULL;
    Owner owner = {g_thread_self(), completion ? Ready() : PlanBoard(), 0U, 0U};
    char directory[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
    FixtureDirectory(directory); FixturePath(file, directory, "decision.umicompletion");
    if (!save && !missing) {
        if (completion) {
            UmiKanbanCompletionRecord record; Record(&record, "alpha");
            UmiKanbanCompletionPlan *plan = NULL;
            REQUIRE(Capture(owner.board, &record, 1U, &plan) == UMI_STATUS_OK);
            REQUIRE(UmiKanbanCompletionPlanWriteNew(file, plan) == UMI_STATUS_OK);
            UmiKanbanCompletionPlanDestroy(plan);
        } else {
            UmiKanbanPublicationPlan *plan = Plan(owner.board);
            REQUIRE(UmiKanbanPublicationPlanSave(directory, "decision.umipublic", plan) == UMI_STATUS_OK);
            UmiKanbanPublicationPlanDestroy(plan);
        }
    }
    GtkWidget *root = completion
        ? UmiKanbanCompletionGtk4Create(CaptureCompletion, ApplyCompletion, &owner, DestroyContext)
        : UmiKanbanPublicPlanGtk4Create(CapturePublic, ApplyPublic, &owner, DestroyContext);
    REQUIRE(root); g_object_ref_sink(root); g_object_weak_ref(G_OBJECT(root), DestroyWidget, &owner);
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_child(window, root); gtk_window_present(window); NativeDrain();
    if (completion) {
        FillReview(root, file); ClickReview(root, save ? "save" : "load");
        REQUIRE(UmiKanbanCompletionGtk4Pending(root)); UmiKanbanCompletionGtk4Retire(root);
    } else {
        gtk_editable_set_text(GTK_EDITABLE(Tagged(root, "kanban.plan.directory")), directory);
        gtk_editable_set_text(GTK_EDITABLE(Tagged(root, "kanban.plan.filename")), "decision.umipublic");
        g_signal_emit_by_name(Tagged(root, save ? "kanban.plan.save" : "kanban.plan.load"), "clicked");
        REQUIRE(UmiKanbanPublicPlanGtk4Pending(root)); UmiKanbanPublicPlanGtk4Retire(root);
    }
    /* Drop every host-held root reference before dispatching worker completion.
     * A retained test reference here would conceal the finalizer-thread defect. */
    gtk_window_destroy(window); g_object_unref(root); root = NULL;
    gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while ((!owner.context_destroyed || !owner.widget_destroyed) && g_get_monotonic_time() < end) {
        (void)UmiTestDispatchSlice(NULL, end, 32U); g_usleep(1000U);
    }
    REQUIRE(owner.context_destroyed == 1U && owner.widget_destroyed == 1U);
    if (save) {
        if (completion) {
            UmiKanbanCompletionPlan *plan = NULL;
            REQUIRE(UmiKanbanCompletionPlanLoadFile(file, &plan) == UMI_STATUS_OK);
            UmiKanbanCompletionPlanDestroy(plan);
        } else {
            UmiKanbanPublicationPlan *plan = NULL;
            REQUIRE(UmiKanbanPublicationPlanLoad(directory, "decision.umipublic", &plan) == UMI_STATUS_OK);
            UmiKanbanPublicationPlanDestroy(plan);
        }
    }
    umi_kanban_board_destroy(owner.board);
    return 0;
}
