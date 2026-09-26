/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ai_workspace_panel_gtk4.c
 *
 * PURPOSE:
 *   Render source collections, reviewed requests and saved drafts while keeping AI execution off the GTK thread.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/ai_workspace.h"
#include <inttypes.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* This presentation Slave Controller borrows the shared workspace. A worker
 * temporarily owns all workspace access during Run; GTK touches only copied
 * request IDs and the thread-safe cancellation token until the worker joins. */
typedef struct AiWorkItem {
    UmiAiWorkspace *workspace;
    UmiAiWorkspaceCancellation *cancellation;
    char jobId[UMI_AI_WORKSPACE_ID_CAPACITY];
    UmiStatus status;
    gint finished;
} AiWorkItem;
struct UmiAiWorkspaceGtkPanel {
    UmiAiWorkspace *workspace; UmiAiRuntime *runtime; UmiAiWorkspaceCancellation *cancellation;
    GtkWidget *root, *forms; GtkLabel *message; GtkNotebook *pages;
    GtkEntry *collectionId, *collectionTitle, *sourceId, *sourceTitle, *firstLine;
    GtkEntry *jobId, *modelId, *actor, *reviewer, *query;
    GtkDropDown *kind, *provider;
    GtkTextBuffer *passage, *prompt, *review, *catalogue, *history, *search;
    GtkButton *approve, *deny, *run, *cancel;
    GPtrArray *signalObjects;
    char selectedId[UMI_AI_WORKSPACE_ID_CAPACITY];
    GThread *worker; AiWorkItem *work; guint pollSource;
    bool closed;
};
static bool Available(UmiAiWorkspaceGtkPanel *panel) { return panel != NULL && !panel->closed && panel->worker == NULL; }
static const char *Explain(UmiStatus status)
{
    switch (status) {
    case UMI_STATUS_OK: return "Completed. Review the saved result below.";
    case UMI_STATUS_BUSY: return "The workspace changed, or a resource is busy. Reload and review the current state.";
    case UMI_STATUS_PERMISSION_DENIED: return "This operation is not permitted. Check the provider, tool permission and review decision.";
    case UMI_STATUS_NOT_FOUND: return "No matching source, job or configured provider was found. Check the selected collection and identifiers.";
    case UMI_STATUS_INVALID_ARGUMENT: return "Check the fields, identifier characters, text length and line number.";
    case UMI_STATUS_CAPACITY_EXCEEDED: return "A text, vector, source or job limit was reached. No larger record was silently truncated.";
    case UMI_STATUS_PARSE_ERROR: return "Stored data or the provider reply failed validation. Inspect the job and sources before trying a new request.";
    case UMI_STATUS_INVALID_STATE: return "This job cannot proceed in its present state. Load its saved history and review what happened.";
    case UMI_STATUS_ALREADY_EXISTS: return "This job ID already has different saved input. Use New job for a different request.";
    case UMI_STATUS_UNAVAILABLE: return "The requested capability is unavailable in this build or configuration. No substitute model was used.";
    case UMI_STATUS_CANCELLED: return "The local request was cancelled. A remote process may already have started generating; it was not retried.";
    case UMI_STATUS_TIMEOUT: return "The local request timed out. Review its failed state before preparing another job.";
    default: return "The operation failed. Check storage and provider configuration; an uncertain result must not be retried automatically.";
    }
}
static void Message(UmiAiWorkspaceGtkPanel *panel, UmiStatus status)
{ gtk_label_set_text(panel->message, Explain(status)); }
static void Tag(GtkWidget *widget, const char *id)
{ g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free); }
static void Connect(UmiAiWorkspaceGtkPanel *panel, GObject *object, const char *signal, GCallback callback)
{
    g_signal_connect(object, signal, callback, panel);
    g_ptr_array_add(panel->signalObjects, g_object_ref(object));
}
static GtkWidget *Button(UmiAiWorkspaceGtkPanel *panel, GtkWidget *row, const char *label,
    const char *id, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(label); Tag(button, id);
    gtk_box_append(GTK_BOX(row), button); Connect(panel, G_OBJECT(button), "clicked", callback); return button;
}
static GtkEntry *Entry(GtkWidget *box, const char *label, const char *value, const char *id, const char *help)
{
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8), *name = gtk_label_new(label), *entry = gtk_entry_new();
    gtk_label_set_xalign(GTK_LABEL(name), 0.0F); gtk_widget_set_size_request(name, 155, -1);
    gtk_widget_set_hexpand(entry, TRUE); gtk_editable_set_text(GTK_EDITABLE(entry), value);
    gtk_widget_set_tooltip_text(entry, help); Tag(entry, id);
    gtk_box_append(GTK_BOX(row), name); gtk_box_append(GTK_BOX(row), entry); gtk_box_append(GTK_BOX(box), row);
    return GTK_ENTRY(entry);
}
static GtkTextBuffer *TextArea(GtkWidget *box, const char *title, const char *id, int height, bool editable)
{
    GtkWidget *label = gtk_label_new(title), *scroll = gtk_scrolled_window_new(), *view = gtk_text_view_new();
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F); gtk_box_append(GTK_BOX(box), label);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), editable); gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), editable);
    gtk_widget_set_size_request(scroll, -1, height); gtk_widget_set_vexpand(scroll, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view); gtk_box_append(GTK_BOX(box), scroll);
    Tag(view, id); return gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
}
static char *BufferText(GtkTextBuffer *buffer)
{
    GtkTextIter begin, end; gtk_text_buffer_get_bounds(buffer, &begin, &end);
    return gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
}
static const char *Value(GtkEntry *entry) { return gtk_editable_get_text(GTK_EDITABLE(entry)); }
static GtkWidget *Page(UmiAiWorkspaceGtkPanel *panel, const char *name)
{
    GtkWidget *scroll = gtk_scrolled_window_new(), *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(box, 12); gtk_widget_set_margin_end(box, 12);
    gtk_widget_set_margin_top(box, 12); gtk_widget_set_margin_bottom(box, 12);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    gtk_notebook_append_page(panel->pages, scroll, gtk_label_new(name)); return box;
}
static void Refresh(UmiAiWorkspaceGtkPanel *panel)
{
    if (!Available(panel)) return;
    UmiAiWorkspaceSnapshot snapshot;
    UmiStatus status = UmiAiWorkspaceSnapshotRead(panel->workspace, &snapshot);
    if (status != UMI_STATUS_OK) { Message(panel, status); return; }
    GString *catalogue = g_string_new(NULL), *history = g_string_new(NULL);
    g_string_append_printf(catalogue, "Saved revision %" PRIu64 " | %zu collections | %zu passages\n\n", snapshot.revision, snapshot.collectionCount, snapshot.sourceCount);
    for (size_t i = 0U; i < snapshot.collectionCount; ++i) {
        UmiAiWorkspaceCollection collection;
        if (UmiAiWorkspaceCollectionAt(panel->workspace, i, &collection) == UMI_STATUS_OK)
            g_string_append_printf(catalogue, "Collection: %s — %s\n", collection.id, collection.title);
    }
    for (size_t i = 0U; i < snapshot.sourceCount; ++i) {
        UmiAiWorkspaceSource source;
        if (UmiAiWorkspaceSourceAt(panel->workspace, i, &source) == UMI_STATUS_OK)
            g_string_append_printf(catalogue, "\n%s / %s — %s\nLines %u–%u | source revision %" PRIu64 "\n%s\n",
                source.collectionId, source.id, source.title, source.firstLine, source.lastLine, source.revision, source.text);
    }
    UmiAiWorkspaceJob *job = g_new0(UmiAiWorkspaceJob, 1U);
    for (size_t i = 0U; i < snapshot.jobCount; ++i) {
        if (UmiAiWorkspaceJobAt(panel->workspace, i, job) == UMI_STATUS_OK)
            g_string_append_printf(history, "%s | %s | %s | %s\n", job->id, UmiAiWorkspaceJobStateText(job->state), job->providerId, job->modelId);
    }
    gtk_text_buffer_set_text(panel->catalogue, catalogue->str, -1);
    gtk_text_buffer_set_text(panel->history, history->str, -1);
    g_string_free(catalogue, TRUE); g_string_free(history, TRUE);
    bool pending = false, approved = false;
    status = UmiAiWorkspaceJobFind(panel->workspace, panel->selectedId, job);
    if (status == UMI_STATUS_OK) {
        GString *review = g_string_new(NULL);
        g_string_append_printf(review,
            "Saved job: %s\nState: %s\nProvider: %s\nRequested model: %s\nWriter label: %s\nReviewer label: %s\n"
            "Permission: %s\n\nThese local labels are not authenticated user identities.\n\nFrozen request:\n%s\n",
            job->id, UmiAiWorkspaceJobStateText(job->state), job->providerId, job->modelId,
            job->requestedBy, job->reviewedBy, job->permission, job->prompt);
        for (size_t i = 0U; i < job->evidenceCount; ++i) {
            const UmiAiWorkspaceSource *source = &job->evidence[i].source;
            g_string_append_printf(review, "\n[S%zu] %s — %s | lines %u–%u | source revision %" PRIu64 "\n%s\n",
                i + 1U, source->id, source->title, source->firstLine, source->lastLine, source->revision, source->text);
        }
        if (job->response.text[0] != '\0') {
            g_string_append(review, "\nReturned draft (read and check before using):\n");
            g_string_append(review, job->response.text);
            if (job->response.finish_reason == UMI_AI_FINISH_LENGTH)
                g_string_append(review, "\n\nThe provider stopped at its output-length limit. This draft may be incomplete.");
        }
        if (job->state == UMI_AI_WORKSPACE_INTERRUPTED)
            g_string_append(review, "\n\nThe previous run has an uncertain outcome. Reloading does not repeat it.");
        gtk_text_buffer_set_text(panel->review, review->str, -1); g_string_free(review, TRUE);
        pending = job->state == UMI_AI_WORKSPACE_REVIEW && !snapshot.recoveryRequired;
        approved = job->state == UMI_AI_WORKSPACE_APPROVED && !snapshot.recoveryRequired;
    } else gtk_text_buffer_set_text(panel->review, "Prepare a job or enter a saved job ID and choose Load job. Nothing is sent merely by viewing this page.", -1);
    gtk_widget_set_sensitive(GTK_WIDGET(panel->approve), pending);
    gtk_widget_set_sensitive(GTK_WIDGET(panel->deny), pending);
    gtk_widget_set_sensitive(GTK_WIDGET(panel->run), approved);
    gtk_widget_set_sensitive(GTK_WIDGET(panel->cancel), pending || approved);
    g_free(job);
}
static void SaveCollection(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; if (!Available(panel)) return;
    UmiStatus status = UmiAiWorkspacePutCollection(panel->workspace, Value(panel->collectionId), Value(panel->collectionTitle));
    Refresh(panel); Message(panel, status);
}
static void SaveSource(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; if (!Available(panel)) return;
    const char *first = Value(panel->firstLine); char *end = NULL; errno = 0;
    unsigned long parsed = strtoul(first, &end, 10);
    if (errno != 0 || first[0] < '0' || first[0] > '9' || *end != '\0' || parsed == 0UL || parsed > UINT32_MAX) {
        Message(panel, UMI_STATUS_INVALID_ARGUMENT); return;
    }
    char *text = BufferText(panel->passage);
    UmiStatus status = UmiAiWorkspacePutSource(panel->workspace, Value(panel->sourceId), Value(panel->collectionId),
        Value(panel->sourceTitle), text, (uint32_t)parsed);
    g_free(text); Refresh(panel); Message(panel, status);
}
static void RemoveSource(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; if (!Available(panel)) return;
    UmiStatus status = UmiAiWorkspaceRemoveSource(panel->workspace, Value(panel->sourceId)); Refresh(panel); Message(panel, status);
}
static void Search(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; if (!Available(panel)) return;
    UmiAiWorkspaceEvidence *results = g_new0(UmiAiWorkspaceEvidence, UMI_AI_MAX_RETRIEVAL_RESULTS); size_t count = 0U;
    UmiStatus status = UmiAiWorkspaceSearch(panel->workspace, Value(panel->collectionId), Value(panel->query),
        NULL, NULL, NULL, results, UMI_AI_MAX_RETRIEVAL_RESULTS, &count);
    GString *text = g_string_new("Lexical query-term retrieval — no embedding model was used.\n\n");
    if (status == UMI_STATUS_OK && count == 0U) g_string_append(text, "No matching passages. Broaden the question or choose another collection.");
    for (size_t i = 0U; i < count; ++i)
        g_string_append_printf(text, "%s | score %.3f | lines %u–%u\n%s\n\n", results[i].source.id, results[i].score,
            results[i].source.firstLine, results[i].source.lastLine, results[i].source.text);
    gtk_text_buffer_set_text(panel->search, text->str, -1); g_string_free(text, TRUE); g_free(results); Message(panel, status);
}
static void NewJob(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; if (!Available(panel)) return;
    char *id = g_strdup_printf("draft.%" G_GINT64_FORMAT, g_get_real_time());
    gtk_editable_set_text(GTK_EDITABLE(panel->jobId), id); panel->selectedId[0] = '\0'; g_free(id); Refresh(panel);
    gtk_label_set_text(panel->message, "A new job ID is ready. Enter the request, then choose Prepare for review.");
}
static void LoadJob(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; if (!Available(panel)) return;
    UmiAiWorkspaceJob *job = g_new0(UmiAiWorkspaceJob, 1U);
    UmiStatus status = UmiAiWorkspaceJobFind(panel->workspace, Value(panel->jobId), job);
    if (status == UMI_STATUS_OK) g_strlcpy(panel->selectedId, job->id, sizeof(panel->selectedId));
    else panel->selectedId[0] = '\0';
    g_free(job); Refresh(panel); Message(panel, status);
}
static void Prepare(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; if (!Available(panel)) return;
    guint selected = gtk_drop_down_get_selected(panel->kind);
    UmiAiWorkspaceJobKind kind = selected == 0U ? UMI_AI_WORKSPACE_GROUNDED_DRAFT : selected == 1U ? UMI_AI_WORKSPACE_DRAFT : UMI_AI_WORKSPACE_TOOL;
    const char *provider = kind == UMI_AI_WORKSPACE_TOOL ? "umicom.workspace.capacities" :
        gtk_drop_down_get_selected(panel->provider) == 0U ? "umicom.extractive-preview" : "umicom.local-chat";
    char *prompt = BufferText(panel->prompt);
    UmiStatus status = UmiAiWorkspacePrepare(panel->workspace, Value(panel->jobId), kind, provider,
        kind == UMI_AI_WORKSPACE_TOOL ? NULL : Value(panel->modelId), kind == UMI_AI_WORKSPACE_GROUNDED_DRAFT ? Value(panel->collectionId) : NULL,
        kind == UMI_AI_WORKSPACE_TOOL ? "{}" : prompt, Value(panel->actor), kind == UMI_AI_WORKSPACE_TOOL ? 0U : 512U);
    if (status == UMI_STATUS_OK) g_strlcpy(panel->selectedId, Value(panel->jobId), sizeof(panel->selectedId));
    g_free(prompt); Refresh(panel); Message(panel, status);
}
static void Approve(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; if (!Available(panel)) return;
    UmiStatus status = UmiAiWorkspaceReview(panel->workspace, panel->selectedId, Value(panel->reviewer), true);
    Refresh(panel); Message(panel, status);
}
static void Deny(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; if (!Available(panel)) return;
    UmiStatus status = UmiAiWorkspaceReview(panel->workspace, panel->selectedId, Value(panel->reviewer), false);
    Refresh(panel); Message(panel, status);
}
static void Cancel(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button;
    if (panel == NULL || panel->closed) return;
    if (panel->worker != NULL) {
        UmiAiWorkspaceCancellationRequest(panel->cancellation);
        gtk_label_set_text(panel->message, "Cancellation requested. Waiting for the local transport to return; no automatic retry will follow."); return;
    }
    UmiStatus status = UmiAiWorkspaceCancel(panel->workspace, panel->selectedId); Refresh(panel); Message(panel, status);
}
static gpointer RunWorker(gpointer data)
{
    AiWorkItem *work = data;
    work->status = UmiAiWorkspaceRun(work->workspace, work->jobId, work->cancellation);
    g_atomic_int_set(&work->finished, 1); return NULL;
}
static gboolean PollWorker(gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data;
    if (panel->closed || panel->work == NULL) { panel->pollSource = 0U; return G_SOURCE_REMOVE; }
    if (!g_atomic_int_get(&panel->work->finished)) return G_SOURCE_CONTINUE;
    (void)g_thread_join(panel->worker); panel->worker = NULL;
    UmiStatus status = panel->work->status; g_free(panel->work); panel->work = NULL; panel->pollSource = 0U;
    gtk_widget_set_sensitive(panel->forms, TRUE); gtk_button_set_label(panel->cancel, "Cancel selected job");
    Refresh(panel); Message(panel, status); return G_SOURCE_REMOVE;
}
static void Run(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; GError *error = NULL;
    if (!Available(panel) || panel->selectedId[0] == '\0') return;
    panel->work = g_new0(AiWorkItem, 1U); panel->work->workspace = panel->workspace;
    panel->work->cancellation = panel->cancellation; g_strlcpy(panel->work->jobId, panel->selectedId, sizeof(panel->work->jobId));
    UmiAiWorkspaceCancellationReset(panel->cancellation);
    gtk_widget_set_sensitive(panel->forms, FALSE); gtk_widget_set_sensitive(GTK_WIDGET(panel->cancel), TRUE);
    gtk_button_set_label(panel->cancel, "Stop current request");
    panel->worker = g_thread_try_new("umicom-ai-request", RunWorker, panel->work, &error);
    if (panel->worker == NULL) {
        g_clear_error(&error); g_free(panel->work); panel->work = NULL;
        gtk_widget_set_sensitive(panel->forms, TRUE); gtk_button_set_label(panel->cancel, "Cancel selected job");
        Refresh(panel); Message(panel, UMI_STATUS_UNAVAILABLE); return;
    }
    panel->pollSource = g_timeout_add(100U, PollWorker, panel);
    if (panel->pollSource == 0U) {
        UmiAiWorkspaceCancellationRequest(panel->cancellation);
        (void)g_thread_join(panel->worker); panel->worker = NULL;
        g_free(panel->work); panel->work = NULL;
        gtk_widget_set_sensitive(panel->forms, TRUE); gtk_button_set_label(panel->cancel, "Cancel selected job");
        Refresh(panel); Message(panel, UMI_STATUS_UNAVAILABLE); return;
    }
    gtk_label_set_text(panel->message, "Running the reviewed request. You can stop it; source edits and other requests are paused until it finishes.");
}
static void Reload(GtkButton *button, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)button; if (!Available(panel)) return;
    UmiStatus status = UmiAiWorkspaceReload(panel->workspace); Refresh(panel); Message(panel, status);
}
static void ProviderChanged(GObject *object, GParamSpec *parameter, gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data; (void)object; (void)parameter; if (!Available(panel)) return;
    gtk_editable_set_text(GTK_EDITABLE(panel->modelId), gtk_drop_down_get_selected(panel->provider) == 0U ? "extractive-preview" : "local-model");
}
UmiStatus UmiAiWorkspaceGtkPanelCreate(UmiAiWorkspace *workspace, UmiAiRuntime *runtime,
    UmiAiWorkspaceCancellation *cancellation, const char *storageDescription,
    const char *applicationId, UmiAiWorkspaceGtkPanel **outPanel)
{
    if (outPanel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outPanel = NULL;
    if (workspace == NULL || runtime == NULL || cancellation == NULL || storageDescription == NULL || applicationId == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiWorkspaceGtkPanel *panel = g_new0(UmiAiWorkspaceGtkPanel, 1U);
    panel->workspace = workspace; panel->runtime = runtime; panel->cancellation = cancellation;
    panel->signalObjects = g_ptr_array_new_with_free_func(g_object_unref);
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); g_object_ref_sink(panel->root); Tag(panel->root, "ai.workspace.root");
    GtkWidget *description = gtk_label_new(storageDescription);
    gtk_label_set_wrap(GTK_LABEL(description), TRUE); gtk_label_set_selectable(GTK_LABEL(description), TRUE);
    gtk_box_append(GTK_BOX(panel->root), description);
    panel->forms = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); gtk_widget_set_vexpand(panel->forms, TRUE);
    Tag(panel->forms, "ai.workspace.forms");
    gtk_box_append(GTK_BOX(panel->root), panel->forms);
    panel->pages = GTK_NOTEBOOK(gtk_notebook_new()); gtk_widget_set_vexpand(GTK_WIDGET(panel->pages), TRUE);
    gtk_box_append(GTK_BOX(panel->forms), GTK_WIDGET(panel->pages));
    GtkWidget *sources = Page(panel, "Sources"), *drafts = Page(panel, "Drafts and review"), *history = Page(panel, "History");
    panel->collectionId = Entry(sources, "Collection ID", "workshop", "ai.collection.id", "Letters, numbers, dots, underscores and hyphens. This groups passages for retrieval.");
    panel->collectionTitle = Entry(sources, "Collection name", "Community workshop", "ai.collection.title", "A readable title for this collection.");
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(sources), row);
    Button(panel, row, "Save collection", "ai.collection.save", G_CALLBACK(SaveCollection));
    panel->sourceId = Entry(sources, "Passage ID", "opening", "ai.source.id", "A stable ID for this selected passage. Saving the same ID replaces its current text and invalidates old reviews.");
    panel->sourceTitle = Entry(sources, "Source title", "Opening notice", "ai.source.title", "Use a title that helps you find the original document.");
    panel->firstLine = Entry(sources, "First source line", "1", "ai.source.line", "The first line number in the original source; the last line is calculated from the pasted text.");
    panel->passage = TextArea(sources, "Selected source passage (up to 1535 UTF-8 bytes)", "ai.source.text", 100, true);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(sources), row);
    Button(panel, row, "Save passage", "ai.source.save", G_CALLBACK(SaveSource));
    Button(panel, row, "Remove current passage", "ai.source.remove", G_CALLBACK(RemoveSource));
    panel->query = Entry(sources, "Search question", "When does the workshop open?", "ai.search.query", "Search is limited to the collection ID above. No model is contacted.");
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(sources), row);
    Button(panel, row, "Inspect retrieval", "ai.search.run", G_CALLBACK(Search));
    panel->search = TextArea(sources, "Retrieval results", "ai.search.results", 100, false);
    panel->catalogue = TextArea(sources, "Saved collections and passages", "ai.source.catalogue", 140, false);
    panel->jobId = Entry(drafts, "Job ID", "", "ai.job.id", "A saved job never changes its input. Choose New job for a different request.");
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(drafts), row);
    Button(panel, row, "New job", "ai.job.new", G_CALLBACK(NewJob)); Button(panel, row, "Load job", "ai.job.load", G_CALLBACK(LoadJob));
    static const char *const kinds[] = {"Grounded draft", "Free draft", "Read workspace limits (tool)", NULL};
    static const char *const providers[] = {"Extractive preview (not an LLM)", "Configured local model", NULL};
    panel->kind = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(kinds)); Tag(GTK_WIDGET(panel->kind), "ai.job.kind");
    panel->provider = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(providers)); Tag(GTK_WIDGET(panel->provider), "ai.job.provider");
    gtk_box_append(GTK_BOX(drafts), gtk_label_new("Request type")); gtk_box_append(GTK_BOX(drafts), GTK_WIDGET(panel->kind));
    gtk_box_append(GTK_BOX(drafts), gtk_label_new("Provider")); gtk_box_append(GTK_BOX(drafts), GTK_WIDGET(panel->provider));
    panel->modelId = Entry(drafts, "Model ID", "extractive-preview", "ai.job.model", "For actual local generation, enter the model name served by your configured endpoint. Models are not downloaded by this workspace.");
    panel->actor = Entry(drafts, "Writer label", "writer", "ai.job.writer", "Local review label, not an authenticated account.");
    panel->reviewer = Entry(drafts, "Reviewer label", "reviewer", "ai.job.reviewer", "A local record of your review decision, not an authentication or security credential.");
    panel->prompt = TextArea(drafts, "Question or drafting instruction", "ai.job.prompt", 90, true);
    gtk_text_buffer_set_text(panel->prompt, "When does the workshop open?", -1);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(drafts), row);
    Button(panel, row, "Prepare for review", "ai.job.prepare", G_CALLBACK(Prepare));
    panel->approve = GTK_BUTTON(Button(panel, row, "Approve selected job", "ai.job.approve", G_CALLBACK(Approve)));
    panel->deny = GTK_BUTTON(Button(panel, row, "Reject selected job", "ai.job.reject", G_CALLBACK(Deny)));
    panel->run = GTK_BUTTON(Button(panel, row, "Run approved job", "ai.job.run", G_CALLBACK(Run)));
    panel->review = TextArea(drafts, "Frozen request, source references and returned draft", "ai.job.review", 230, false);
    panel->history = TextArea(history, "Saved jobs — copy an ID to Load job on the Drafts page", "ai.history", 260, false);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(panel->forms), row);
    Button(panel, row, "Reload shared workspace", "ai.workspace.reload", G_CALLBACK(Reload));
    GtkWidget *notice = gtk_label_new("Editing the form does not change a saved job. Retrieved text and model output never authorise actions. Check the sources before using a draft.");
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE); gtk_box_append(GTK_BOX(panel->root), notice);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(panel->root), row);
    panel->cancel = GTK_BUTTON(Button(panel, row, "Cancel selected job", "ai.job.cancel", G_CALLBACK(Cancel)));
    panel->message = GTK_LABEL(gtk_label_new("Nothing is sent until a prepared job is approved and run."));
    gtk_label_set_wrap(panel->message, TRUE); gtk_box_append(GTK_BOX(panel->root), GTK_WIDGET(panel->message));
    Connect(panel, G_OBJECT(panel->provider), "notify::selected", G_CALLBACK(ProviderChanged));
    NewJob(NULL, panel);
    gtk_notebook_set_current_page(panel->pages, strcmp(applicationId, "org.umicom.rag") == 0 ? 0 : 1);
    *outPanel = panel; return UMI_STATUS_OK;
}
GtkWidget *UmiAiWorkspaceGtkPanelWidget(UmiAiWorkspaceGtkPanel *panel) { return panel != NULL ? panel->root : NULL; }
void UmiAiWorkspaceGtkPanelDestroy(UmiAiWorkspaceGtkPanel *panel)
{
    if (panel == NULL) return;
    panel->closed = true;
    if (panel->pollSource != 0U) { g_source_remove(panel->pollSource); panel->pollSource = 0U; }
    /* The owner always joins once before releasing any borrowed service.
     * Cancellation is cooperative; configured local transport has a timeout.
     * No completion callback is left queued against a freed GTK controller. */
    UmiAiWorkspaceCancellationRequest(panel->cancellation);
    if (panel->worker != NULL) { (void)g_thread_join(panel->worker); panel->worker = NULL; }
    g_free(panel->work);
    for (guint i = 0U; i < panel->signalObjects->len; ++i)
        g_signal_handlers_disconnect_by_data(g_ptr_array_index(panel->signalObjects, i), panel);
    g_ptr_array_unref(panel->signalObjects);
    g_object_unref(panel->root); g_free(panel);
}
