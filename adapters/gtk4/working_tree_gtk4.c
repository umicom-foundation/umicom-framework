/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/working_tree_gtk4.c
 * PURPOSE: Keep repository inspection responsive and release workers after the panel closes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/threading.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/vcs/working_tree_gtk4.h"
#include "umicom/vcs/working_tree_job.h"
#include "umicom/vcs/working_tree_review.h"
#include <gtk/gtk.h>
#include <string.h>

#define REVIEW_PAGE_SIZE 50U
#define REVIEW_STATE_KEY "umicom-working-tree-review"

typedef struct WorkingTreeReview
{
    unsigned references;
    gboolean closed;
    gboolean collected;
    char root[UMI_VCS_PATH_CAPACITY];
    UmiTaskQueue *queue;
    UmiVcsWorkingTreeJob *job;
    UmiVcsWorkingTree *tree;
    GtkWidget *inspect;
    GtkWidget *stop;
    GtkWidget *previous;
    GtkWidget *next;
    GtkWidget *status;
    GtkWidget *text;
    size_t offset;
} WorkingTreeReview;

/* Only the GTK thread changes references. The worker owns no widget or review pointer. */
static void ReviewRelease(gpointer data)
{
    WorkingTreeReview *review = data;
    if (--review->references != 0U)
        return;
    UmiVcsWorkingTreeDestroy(review->tree);
    g_free(review);
}

/* A retained control must not restart a job after its panel has left the visible hierarchy. */
static WorkingTreeReview *ActiveReview(GtkWidget *root)
{
    WorkingTreeReview *review = g_object_get_data(G_OBJECT(root), REVIEW_STATE_KEY);
    return review != NULL && !review->closed && gtk_widget_get_mapped(root) ? review : NULL;
}

/* Display saved observations as plain text; malformed UTF-8 path bytes never become markup. */
static void ReviewSetText(WorkingTreeReview *review, const char *text)
{
    char *display = g_utf8_make_valid(text, -1);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(review->text)), display, -1);
    g_free(display);
}

/* Page by observation index to keep large repositories from creating thousands of widgets. */
static void ReviewRender(WorkingTreeReview *review)
{
    UmiVcsWorkingTreeSummary summary;
    char summary_text[UMI_VCS_WORKING_TREE_SUMMARY_TEXT_CAPACITY];
    char *entry_text;
    GString *text;
    size_t end;
    if (review->tree == NULL)
        return;
    if (UmiVcsWorkingTreeDescribe(review->tree, &summary) != UMI_STATUS_OK ||
        UmiVcsWorkingTreeSummaryText(review->tree, summary_text, sizeof(summary_text)) !=
            UMI_STATUS_OK)
        return;
    if (review->offset >= summary.entries)
        review->offset = 0U;
    end = review->offset + REVIEW_PAGE_SIZE;
    if (end > summary.entries)
        end = summary.entries;
    text = g_string_new(summary_text);
    g_string_append_printf(text, "\n\nPaths %zu through %zu of %zu\n",
                           summary.entries != 0U ? review->offset + 1U : 0U, end, summary.entries);
    entry_text = g_malloc(UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY);
    for (size_t index = review->offset; index < end; ++index)
    {
        UmiStatus status = UmiVcsWorkingTreeEntryText(review->tree, index, entry_text,
                                                      UMI_VCS_WORKING_TREE_ENTRY_TEXT_CAPACITY);
        if (status != UMI_STATUS_OK)
        {
            g_string_append_printf(text, "\nPath %zu could not be formatted: %s\n", index + 1U,
                                   umi_status_text(status));
            continue;
        }
        g_string_append_c(text, '\n');
        g_string_append(text, entry_text);
        g_string_append_c(text, '\n');
    }
    ReviewSetText(review, text->str);
    gtk_widget_set_sensitive(review->previous, review->offset != 0U);
    gtk_widget_set_sensitive(review->next, end < summary.entries);
    g_free(entry_text);
    g_string_free(text, TRUE);
}

/* Stop is a request, not a synchronous wait or an assertion that Git already exited. */
static void ReviewRequestStop(WorkingTreeReview *review)
{
    if (review->job != NULL)
        (void)UmiVcsWorkingTreeJobCancel(review->job);
    if (review->queue != NULL)
        (void)UmiTaskQueueRequestShutdown(review->queue, 1, 1);
}

/* Widget destruction drops its ownership, while the polling source retains worker cleanup. */
static void ReviewRootReleased(gpointer data)
{
    WorkingTreeReview *review = data;
    review->closed = TRUE;
    ReviewRequestStop(review);
    ReviewRelease(review);
}

/* Observe completion and reap native workers without blocking the event loop on a join. */
static gboolean ReviewPoll(gpointer data)
{
    WorkingTreeReview *review = data;
    UmiVcsWorkingTreeJobSnapshot snapshot;
    UmiStatus status;
    if (!review->collected)
    {
        status = UmiVcsWorkingTreeJobRead(review->job, &snapshot);
        if (status != UMI_STATUS_OK)
            return G_SOURCE_CONTINUE;
        if (snapshot.state == UMI_TASK_CREATED || snapshot.state == UMI_TASK_QUEUED ||
            snapshot.state == UMI_TASK_RUNNING)
            return G_SOURCE_CONTINUE;
        UmiVcsWorkingTree *tree = NULL;
        status = UmiVcsWorkingTreeJobTake(review->job, &tree);
        if (!review->closed)
        {
            if (status == UMI_STATUS_OK)
            {
                UmiVcsWorkingTreeDestroy(review->tree);
                review->tree = tree;
                tree = NULL;
                review->offset = 0U;
                ReviewRender(review);
                gtk_label_set_text(GTK_LABEL(review->status),
                                   "Observation complete. Refresh after repository changes.");
            }
            else
            {
                char message[256];
                (void)g_snprintf(message, sizeof(message),
                                 "Inspection %s. Any previous observation below is unchanged.",
                                 umi_status_text(status));
                gtk_label_set_text(GTK_LABEL(review->status), message);
            }
            gtk_widget_set_sensitive(review->stop, FALSE);
        }
        UmiVcsWorkingTreeDestroy(tree);
        (void)UmiVcsWorkingTreeJobDestroy(review->job);
        review->job = NULL;
        review->collected = TRUE;
        (void)UmiTaskQueueRequestShutdown(review->queue, 0, 0);
    }
    status = UmiTaskQueueTryFinishShutdown(review->queue);
    if (status != UMI_STATUS_OK)
        return G_SOURCE_CONTINUE;
    status = UmiTaskQueueReleaseStopped(&review->queue);
    if (status != UMI_STATUS_OK)
        return G_SOURCE_CONTINUE;
    if (!review->closed)
        gtk_widget_set_sensitive(review->inspect, TRUE);
    return G_SOURCE_REMOVE;
}

/* New observations replace the display only on success; a failed refresh cannot hide older
 * evidence. */
static void ReviewInspect(GtkButton *button, gpointer data)
{
    GtkWidget *root = data;
    WorkingTreeReview *review = ActiveReview(root);
    UmiVcsWorkingTreeRequest request = {0};
    UmiTaskQueueConfig config = {1U, 1U};
    UmiStatus status;
    (void)button;
    if (review == NULL || review->queue != NULL)
        return;
    request.repository_root = review->root;
    status = UmiVcsWorkingTreeJobCreate(&request, &review->job);
    if (status == UMI_STATUS_OK)
        status = umi_task_queue_create(&config, &review->queue);
    if (status == UMI_STATUS_OK)
        status = UmiVcsWorkingTreeJobSubmit(review->job, review->queue);
    if (status != UMI_STATUS_OK)
    {
        (void)UmiVcsWorkingTreeJobDestroy(review->job);
        review->job = NULL;
        gtk_label_set_text(GTK_LABEL(review->status), umi_status_text(status));
        /* Even a rejected submission can leave native queue workers to reap. */
        if (review->queue == NULL)
            return;
        review->collected = TRUE;
        (void)UmiTaskQueueRequestShutdown(review->queue, 1, 1);
    }
    else
    {
        review->collected = FALSE;
        gtk_widget_set_sensitive(review->stop, TRUE);
        gtk_label_set_text(GTK_LABEL(review->status),
                           "Inspecting local Git state. The previous observation may be stale.");
    }
    gtk_widget_set_sensitive(review->inspect, FALSE);
    ++review->references;
    g_timeout_add_full(G_PRIORITY_DEFAULT, 25U, ReviewPoll, review, ReviewRelease);
}

/* Hiding the panel cancels its read without disabling later explicit inspection after remapping. */
static void ReviewUnmap(GtkWidget *root, gpointer data)
{
    WorkingTreeReview *review = data;
    (void)root;
    ReviewRequestStop(review);
}

/* Stop remains independent from pagination and never performs Git mutations. */
static void ReviewStop(GtkButton *button, gpointer data)
{
    WorkingTreeReview *review = ActiveReview(data);
    (void)button;
    if (review == NULL)
        return;
    ReviewRequestStop(review);
    gtk_label_set_text(GTK_LABEL(review->status),
                       "Stop requested. Waiting for the inspection process to finish.");
}

/* Move only within the immutable observation currently displayed. */
static void ReviewPage(GtkButton *button, gpointer data)
{
    WorkingTreeReview *review = ActiveReview(data);
    if (review == NULL || review->tree == NULL)
        return;
    if (GTK_WIDGET(button) == review->previous)
        review->offset =
            review->offset >= REVIEW_PAGE_SIZE ? review->offset - REVIEW_PAGE_SIZE : 0U;
    else
        review->offset += REVIEW_PAGE_SIZE;
    ReviewRender(review);
}

/* Stable automation IDs let acceptance tests locate controls without depending on translated
 * labels. */
static GtkWidget *ReviewButton(GtkWidget *bar, const char *label, const char *id)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    (void)umi_gtk4_automation_tag_widget(button, id);
    gtk_box_append(GTK_BOX(bar), button);
    return button;
}

/* The panel owns only presentation and scheduling; all Git semantics remain in toolkit-neutral
 * Framework. */
UmiStatus UmiVcsWorkingTreeGtk4Create(const char *repository_root, void **out_widget)
{
    WorkingTreeReview *review;
    GtkWidget *root, *bar, *scroll, *location;
    if (repository_root == NULL || repository_root[0] == '\0' || out_widget == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(repository_root) >= UMI_VCS_PATH_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Never substitute a blocking join on a GTK event handler. */
    if (!UmiThreadCanTryJoin())
        return UMI_STATUS_NOT_IMPLEMENTED;
    review = g_new0(WorkingTreeReview, 1);
    review->references = 1U;
    memcpy(review->root, repository_root, strlen(repository_root) + 1U);
    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    review->inspect = ReviewButton(bar, "Inspect repository", "vcs.review.inspect");
    review->stop = ReviewButton(bar, "Stop", "vcs.review.stop");
    review->previous = ReviewButton(bar, "Previous paths", "vcs.review.previous");
    review->next = ReviewButton(bar, "Next paths", "vcs.review.next");
    review->status = gtk_label_new(
        "Inspect reads local Git status. No files are staged, committed, fetched or pushed.");
    review->text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(review->text), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(review->text), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(review->text), GTK_WRAP_WORD_CHAR);
    gtk_label_set_wrap(GTK_LABEL(review->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(review->status), 0.0F);
    (void)umi_gtk4_automation_tag_widget(review->status, "vcs.review.status");
    (void)umi_gtk4_automation_tag_widget(review->text, "vcs.review.output");
    gtk_widget_set_sensitive(review->stop, FALSE);
    gtk_widget_set_sensitive(review->previous, FALSE);
    gtk_widget_set_sensitive(review->next, FALSE);
    scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_size_request(scroll, -1, 280);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), review->text);
    /* The captured root stays visible even if the host application changes workspace. */
    char *root_display = g_utf8_make_valid(review->root, -1);
    location = gtk_label_new(root_display);
    g_free(root_display);
    gtk_label_set_selectable(GTK_LABEL(location), TRUE);
    gtk_label_set_xalign(GTK_LABEL(location), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(location), TRUE);
    gtk_box_append(GTK_BOX(root), location);
    gtk_box_append(GTK_BOX(root), bar);
    gtk_box_append(GTK_BOX(root), review->status);
    gtk_box_append(GTK_BOX(root), scroll);
    g_object_set_data_full(G_OBJECT(root), REVIEW_STATE_KEY, review, ReviewRootReleased);
    g_signal_connect_object(review->inspect, "clicked", G_CALLBACK(ReviewInspect), root, 0);
    g_signal_connect_object(review->stop, "clicked", G_CALLBACK(ReviewStop), root, 0);
    g_signal_connect_object(review->previous, "clicked", G_CALLBACK(ReviewPage), root, 0);
    g_signal_connect_object(review->next, "clicked", G_CALLBACK(ReviewPage), root, 0);
    g_signal_connect(root, "unmap", G_CALLBACK(ReviewUnmap), review);
    *out_widget = root;
    return UMI_STATUS_OK;
}
