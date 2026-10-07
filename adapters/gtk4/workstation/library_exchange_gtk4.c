/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/library_exchange_gtk4.c
 * PURPOSE: Read bounded layout files asynchronously and apply only an explicit frozen review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/library_exchange.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/text_comparison.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

struct UmiGtk4WorkspaceLibraryExchange {
    GtkWidget *root, *export_button, *import_button, *apply_button, *confirm, *summary, *status;
    GtkWidget *details, *saved_button;
    UmiGtk4WorkspaceLibrarySavedReviewHandler saved_handler;
    void *saved_context;
    bool saved_review;
/* GtkFileDialog uses the supported GTK 4.10 asynchronous contract.
 * The existing busy gate and cancellable continue to own one file operation.
 * The superseded implementation is retained below for engineering review. */
#if 0
    GtkFileChooserNative *chooser;
#endif
    /* One owned dialog participates in the existing busy/cancellation lifetime. */
    GtkFileDialog *chooser;
    GCancellable *cancel;
    GInputStream *input;
    GByteArray *incoming;
    char *outgoing;
    size_t outgoing_size;
    UmiUiWorkspaceLibraryImport *review;
    UmiGtk4WorkspaceLibraryExportHandler export_handler;
    UmiGtk4WorkspaceLibraryImportHandler import_handler;
    UmiGtk4WorkspaceLibraryImportApplyHandler apply_handler;
    void *context;
    unsigned references;
    bool destroyed, busy, exporting;
};

/* Each outstanding asynchronous operation retains this controller, not the
 * product owner. Destruction disconnects the owner before any later callback. */
static UmiGtk4WorkspaceLibraryExchange *retain(UmiGtk4WorkspaceLibraryExchange *ui)
{ ++ui->references; return ui; }
static void release(UmiGtk4WorkspaceLibraryExchange *ui)
{
    if (--ui->references != 0U) return;
    g_clear_object(&ui->input); g_clear_object(&ui->cancel);
    if (ui->incoming != NULL) g_byte_array_unref(ui->incoming);
    free(ui->outgoing); umi_ui_workspace_library_import_destroy(ui->review);
    g_object_unref(ui->root); g_free(ui);
}
static void controls(UmiGtk4WorkspaceLibraryExchange *ui)
{
    if (ui->destroyed) return;
    gtk_widget_set_sensitive(ui->saved_button, !ui->busy && ui->saved_handler != NULL);
    gtk_widget_set_sensitive(ui->export_button, !ui->busy);
    gtk_widget_set_sensitive(ui->import_button, !ui->busy);
    gtk_widget_set_sensitive(ui->confirm, !ui->busy && ui->review != NULL);
    gtk_widget_set_sensitive(ui->apply_button, !ui->busy && ui->review != NULL &&
        gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->confirm)));
}
static void message(UmiGtk4WorkspaceLibraryExchange *ui, const char *text)
{ if (!ui->destroyed) gtk_label_set_text(GTK_LABEL(ui->status), text); }
static void forget_review(UmiGtk4WorkspaceLibraryExchange *ui)
{
    umi_ui_workspace_library_import_destroy(ui->review); ui->review = NULL;
    ui->saved_review = false;
    if (!ui->destroyed) {
        gtk_label_set_text(GTK_LABEL(ui->summary), "");
        if (!ui->destroyed) {
            GtkWidget *child = gtk_widget_get_first_child(ui->details);
            if (child) gtk_box_remove(GTK_BOX(ui->details), child);
        }
        gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->confirm), FALSE);
    }
}
static void finish_io(UmiGtk4WorkspaceLibraryExchange *ui)
{
    g_clear_object(&ui->input);
    if (ui->incoming != NULL) { g_byte_array_unref(ui->incoming); ui->incoming = NULL; }
    free(ui->outgoing); ui->outgoing = NULL; ui->outgoing_size = 0U;
    ui->busy = false; controls(ui);
}
static void confirmed(GtkCheckButton *button, gpointer data)
{ (void)button; controls(data); }

/* Present the full ordered name list with explicit scope limits. The opaque
 * review keeps all geometry even though this text is only a list comparison. */
/* Full immutable geometry replaces the count-only list so users can inspect what importing will replace.
 * The previous implementation is retained for engineering review. */
#if 0
static void show_review(UmiGtk4WorkspaceLibraryExchange *ui)
{
    const UmiUiWorkspaceLibraryPreview *preview = umi_ui_workspace_library_import_summary(ui->review);
    GString *text = g_string_new("Imported layouts (the current named list will be replaced):\n");
    for (size_t index = 0U; index < preview->saved.layout_count; ++index) {
        const UmiUiWorkspaceLibraryRow *row = &preview->saved.rows[index];
        g_string_append_printf(text, "%s%s — %zu panels\n", row->active ? "Active: " : "", row->name, row->window_count);
    }
    g_string_append_printf(text, "Added: %zu. Removed: %zu. Changed summaries: %zu.\n"
        "Panel positions are included but not compared here. Documents, orders and settings are not included.",
        preview->comparison.added_count, preview->comparison.removed_count, preview->comparison.changed_count);
    gtk_label_set_text(GTK_LABEL(ui->summary), text->str); g_string_free(text, TRUE);
}
#endif
/* Format the complete copied layout using locale-independent coordinates.
 * Deliberately omit revisions: archive import advances counters even when a
 * user's arrangement is identical. Stable IDs and ordered rows explain moves,
 * duplicates and hidden panels without inspecting live product widgets. */
static GString *layout_review_text(const UmiUiWorkspaceLibraryImport *review, bool imported)
{
    const UmiUiWorkspaceLibraryPreview *preview = umi_ui_workspace_library_import_summary(review);
    GString *text = g_string_new("Coordinates are fractions of the workspace (0 to 1).\n");
    for (size_t index = 0U; index < umi_ui_workspace_library_import_layout_count(review, imported); ++index) {
        const UmiUiWorkspaceLayout *layout = umi_ui_workspace_library_import_layout(review, imported, index);
        bool active = false;
        for (size_t row = 0U; row < preview->comparison.row_count; ++row) {
            const UmiUiWorkspaceLibraryRow *summary = imported ? &preview->comparison.rows[row].after
                : &preview->comparison.rows[row].before;
            if (strcmp(summary->layout_id, layout->layout_id) == 0) active = summary->active;
        }
        g_string_append_printf(text, "\nLayout: %s\nID: %s\nActive: %s\nLocked: %s\nPanels: %zu\n",
            layout->name, layout->layout_id, active ? "yes" : "no", layout->locked ? "yes" : "no", layout->window_count);
        for (size_t row = 0U; row < layout->window_count; ++row) {
            const UmiUiWorkspaceWindow *panel = &layout->windows[row];
            char x[G_ASCII_DTOSTR_BUF_SIZE], y[G_ASCII_DTOSTR_BUF_SIZE];
            char width[G_ASCII_DTOSTR_BUF_SIZE], height[G_ASCII_DTOSTR_BUF_SIZE];
            g_ascii_dtostr(x, sizeof x, panel->x); g_ascii_dtostr(y, sizeof y, panel->y);
            g_ascii_dtostr(width, sizeof width, panel->width); g_ascii_dtostr(height, sizeof height, panel->height);
            g_string_append_printf(text, "\n  Panel: %s\n  ID: %s\n  Tool: %s\n"
                "  Left: %s\n  Top: %s\n  Width: %s\n  Height: %s\n"
                "  Placement: %s\n  Stack: %s\n  Linked context: %s\n  Legacy group: %s\n"
                "  Visible: %s\n  Floating: %s\n  Maximised: %s\n  Closable: %s\n"
                "  Pinned: %s\n  Resizable: %s\n  Layer: %" PRId32 "\n",
                panel->title, panel->window_id, panel->tool_id, x, y, width, height,
                panel->placement_id, panel->stack_id, panel->context_group_id, panel->group_id,
                panel->visible ? "yes" : "no", panel->floating ? "yes" : "no",
                panel->maximised ? "yes" : "no", panel->closable ? "yes" : "no",
                panel->pinned ? "yes" : "no", panel->resizable ? "yes" : "no", panel->z_order);
        }
    }
    return text;
}
static UmiStatus show_review(UmiGtk4WorkspaceLibraryExchange *ui)
{
    GString *before = layout_review_text(ui->review, false);
    GString *after = layout_review_text(ui->review, true);
    GtkWidget *comparison = NULL;
    /* A saved checkpoint uses the same full geometry renderer; its source
     * label distinguishes disk recovery from an imported file. The earlier
     * fixed-label call is retained for review. */
#if 0
    const UmiStatus status = UmiGtk4TextComparisonCreate(before->str, before->len,
        after->str, after->len, "Current named layouts", "Imported named layouts", &comparison);
#endif
    const UmiStatus status = UmiGtk4TextComparisonCreate(before->str, before->len,
        after->str, after->len, "Current named layouts",
        ui->saved_review ? "Saved named layouts" : "Imported named layouts", &comparison);
    g_string_free(before, TRUE); g_string_free(after, TRUE);
    if (status != UMI_STATUS_OK) return status;
    /* Creating the shared view may notify GTK observers. A closed product
     * must not publish a replacement review into an abandoned controller. */
    if (ui->destroyed) { g_object_ref_sink(comparison); g_object_unref(comparison); return UMI_STATUS_CANCELLED; }
    gtk_widget_set_size_request(comparison, 640, 320);
    gtk_box_append(GTK_BOX(ui->details), comparison);
    if (ui->destroyed) return UMI_STATUS_CANCELLED;
    gtk_label_set_text(GTK_LABEL(ui->summary), "Compare the complete current and imported arrangements below. "
        "Hidden panels and inactive layouts are included. Documents, orders and application settings are not included.");
    if (!ui->destroyed && ui->saved_review) {
        const UmiUiWorkspaceLibraryPreview *preview = umi_ui_workspace_library_import_summary(ui->review);
        gtk_label_set_text(GTK_LABEL(ui->summary), preview->report.checkpoint.recovered_last_good
            ? "Compare the complete current arrangements with the last valid saved copy. The latest copy could not be used. Apply uses exactly this review and does not repair storage."
            : "Compare the complete current and saved arrangements. Apply uses exactly this review, even if storage changes later. Documents, orders and application settings are not included.");
    }
    return ui->destroyed ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}

/* Both file input and embedded host input finish through one review path.
 * The caller holds a controller reference and the busy gate during callbacks. */
static UmiStatus review_content(UmiGtk4WorkspaceLibraryExchange *ui, const void *bytes, size_t size)
{
    if (bytes == NULL || size == 0U || size >= UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY) {
        message(ui, "Select a nonempty layout file within the supported size limit.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    const UmiStatus status = ui->import_handler(bytes, size, &ui->review, ui->context);
    if (ui->destroyed) return UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK && ui->review != NULL) {
        /* A failed presentation must never enable an unreviewed Apply action.
         * The previous summary-only success path is retained below. */
#if 0
        show_review(ui); message(ui, "Review the imported list, then confirm and apply. Nothing has changed yet.");
        return UMI_STATUS_OK;
#endif
        const UmiStatus shown = show_review(ui);
        if (shown != UMI_STATUS_OK) {
            forget_review(ui); message(ui, "The complete review could not be displayed. Nothing was applied.");
            return shown;
        }
        message(ui, "Review both arrangements, then confirm and apply. Nothing has changed yet.");
        return UMI_STATUS_OK;
    }
    forget_review(ui); message(ui, "This layout file cannot be imported into the current workspace.");
    return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
}
UmiStatus umi_gtk4_ws_library_exchange_review_bytes(UmiGtk4WorkspaceLibraryExchange *ui,
    const void *bytes, size_t size)
{
    if (ui == NULL || ui->destroyed) return UMI_STATUS_INVALID_ARGUMENT;
    if (ui->busy) return UMI_STATUS_BUSY;
    retain(ui); ui->busy = true; forget_review(ui); controls(ui);
    const UmiStatus status = review_content(ui, bytes, size);
    ui->busy = false; controls(ui); release(ui); return status;
}

static void read_next(UmiGtk4WorkspaceLibraryExchange *ui);
/* Stream in small bounded chunks. File size metadata is not trusted: a file
 * may grow after selection, and short reads do not necessarily mean EOF. */
static void read_finished(GObject *source, GAsyncResult *result, gpointer data)
{
    UmiGtk4WorkspaceLibraryExchange *ui = data;
    GError *error = NULL;
    GBytes *chunk = g_input_stream_read_bytes_finish(G_INPUT_STREAM(source), result, &error);
    if (ui->destroyed || chunk == NULL) {
        if (!ui->destroyed) message(ui, error != NULL ? error->message : "The layout file could not be read.");
        g_clear_error(&error); if (chunk != NULL) g_bytes_unref(chunk);
        finish_io(ui); release(ui); return;
    }
    gsize size = 0U;
    const void *bytes = g_bytes_get_data(chunk, &size);
    if (size == 0U) {
        /* Shared review completion also supports host-provided bytes. The
         * earlier file-only projection remains available for source review. */
#if 0
        UmiStatus status = ui->import_handler(ui->incoming->data, ui->incoming->len, &ui->review, ui->context);
        if (!ui->destroyed && status == UMI_STATUS_OK && ui->review != NULL) {
            show_review(ui); message(ui, "Review the imported list, then confirm and apply. Nothing has changed yet.");
        } else if (!ui->destroyed) {
            forget_review(ui); message(ui, "This layout file cannot be imported into the current workspace.");
        }
#endif
        (void)review_content(ui, ui->incoming->data, ui->incoming->len);
        g_bytes_unref(chunk); finish_io(ui); release(ui); return;
    }
    if (size >= UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY - ui->incoming->len) {
        message(ui, "This layout file exceeds the supported size.");
        g_bytes_unref(chunk); finish_io(ui); release(ui); return;
    }
    g_byte_array_append(ui->incoming, bytes, (guint)size);
    g_bytes_unref(chunk); read_next(ui); release(ui);
}
static void read_next(UmiGtk4WorkspaceLibraryExchange *ui)
{
    const gsize remaining = UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY - ui->incoming->len;
    g_input_stream_read_bytes_async(ui->input, MIN(remaining, (gsize)8192U), G_PRIORITY_DEFAULT,
        ui->cancel, read_finished, retain(ui));
}
static void opened(GObject *source, GAsyncResult *result, gpointer data)
{
    UmiGtk4WorkspaceLibraryExchange *ui = data;
    GError *error = NULL;
    GFileInputStream *input = g_file_read_finish(G_FILE(source), result, &error);
    if (ui->destroyed || input == NULL) {
        if (!ui->destroyed) message(ui, error != NULL ? error->message : "The layout file could not be opened.");
        g_clear_error(&error); g_clear_object(&input); finish_io(ui); release(ui); return;
    }
    ui->input = G_INPUT_STREAM(input); ui->incoming = g_byte_array_new();
    read_next(ui); release(ui);
}
static void written(GObject *source, GAsyncResult *result, gpointer data)
{
    UmiGtk4WorkspaceLibraryExchange *ui = data;
    GError *error = NULL;
    const gboolean ok = g_file_replace_contents_finish(G_FILE(source), result, NULL, &error);
    message(ui, ok ? "Layout library exported. The current workspace is unchanged."
        : (error != NULL ? error->message : "Export could not be completed."));
    g_clear_error(&error); finish_io(ui); release(ui);
}

/* A chooser is owned until its response is consumed or the controller closes.
 * No path is retained as apply authority; import keeps the bytes it reviewed. */
/* The native-dialog response API is deprecated. Use the supported
 * async/finish pair, preserving owned GFile cleanup, cancellation, private
 * export bytes and the existing reviewed-import pipeline.
 * The superseded implementation is retained below for engineering review. */
#if 0
static void chosen(GtkNativeDialog *dialog, int response, gpointer data)
{
    UmiGtk4WorkspaceLibraryExchange *ui = retain(data);
    GFile *file = response == GTK_RESPONSE_ACCEPT ? gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog)) : NULL;
    g_signal_handlers_disconnect_by_data(dialog, ui);
    gtk_native_dialog_destroy(dialog); g_clear_object(&ui->chooser);
    if (ui->destroyed || file == NULL) { finish_io(ui); g_clear_object(&file); release(ui); return; }
    if (ui->exporting)
        g_file_replace_contents_async(file, ui->outgoing, ui->outgoing_size, NULL, FALSE,
            G_FILE_CREATE_PRIVATE, ui->cancel, written, retain(ui));
    else g_file_read_async(file, G_PRIORITY_DEFAULT, ui->cancel, opened, retain(ui));
    g_object_unref(file); release(ui);
}
#endif
/* Consume the asynchronous result even after close; only a current live owner starts I/O. */
static void chosen(GObject *source, GAsyncResult *result, gpointer data)
{
    /* The launch owns this reference until this completion, including close
     * and cancellation. Finish always consumes the result, even after teardown. */
    UmiGtk4WorkspaceLibraryExchange *ui = data;
    GError *error = NULL;
    GFile *file = ui->exporting
        ? gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, &error)
        : gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error);
    const bool current = ui->chooser == GTK_FILE_DIALOG(source);
    if (current) g_clear_object(&ui->chooser);
    if (ui->destroyed || !current || file == NULL) {
        if (!ui->destroyed && current && error != NULL &&
            !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
            !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) &&
            !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
            message(ui, error->message);
        g_clear_error(&error);
        g_clear_object(&file);
        if (ui->destroyed || current) finish_io(ui);
        release(ui);
        return;
    }
    g_clear_error(&error);
    /* Keep the existing private export and bounded streaming importer. A
     * chooser result selects a file; it never grants permission to apply it. */
    if (ui->exporting)
        g_file_replace_contents_async(file, ui->outgoing, ui->outgoing_size, NULL, FALSE,
            G_FILE_CREATE_PRIVATE, ui->cancel, written, retain(ui));
    else
        g_file_read_async(file, G_PRIORITY_DEFAULT, ui->cancel, opened, retain(ui));
    g_object_unref(file);
    release(ui);
}
static void choose_file(GtkButton *button, gpointer data)
{
    UmiGtk4WorkspaceLibraryExchange *ui = retain(data);
    if (ui->destroyed || ui->busy) { release(ui); return; }
    ui->busy = true; ui->exporting = GTK_WIDGET(button) == ui->export_button;
/* Review invalidation can notify GTK observers that close the product.
 * Recheck retirement before resetting cancellation or calling its owner.
 * The superseded implementation is retained below for engineering review. */
#if 0
    forget_review(ui); controls(ui);
    g_cancellable_reset(ui->cancel);
#endif
    /* Notifications may retire the controller; recheck before continuing owner work. */
    forget_review(ui); controls(ui);
    if (ui->destroyed) { finish_io(ui); release(ui); return; }
    g_cancellable_reset(ui->cancel);
    if (ui->exporting) {
        size_t size = 0U;
        UmiStatus status = ui->export_handler(NULL, 0U, &size, ui->context);
        if (!ui->destroyed && status == UMI_STATUS_OK && size > 0U && size < UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY) {
            ui->outgoing = malloc(size + 1U);
            if (ui->outgoing == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                status = ui->export_handler(ui->outgoing, size + 1U, &ui->outgoing_size, ui->context);
                if (status == UMI_STATUS_OK && ui->outgoing_size > size) status = UMI_STATUS_INVALID_STATE;
            }
        } else if (status == UMI_STATUS_OK) status = UMI_STATUS_INVALID_STATE;
        if (ui->destroyed || status != UMI_STATUS_OK) {
            message(ui, "The committed layout library is not available for export. Finish layout editing and try again.");
            finish_io(ui); release(ui); return;
        }
    }
/* Choose through GtkFileDialog without changing export/import labels,
 * suggested filename, cancellation or the explicit review-and-apply boundary.
 * No deprecated chooser API or diagnostic suppression is needed.
 * The superseded implementation is retained below for engineering review. */
#if 0
    GtkRoot *root = gtk_widget_get_root(ui->root);
    ui->chooser = gtk_file_chooser_native_new(ui->exporting ? "Export layout library" : "Review layout file",
        GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL,
        ui->exporting ? GTK_FILE_CHOOSER_ACTION_SAVE : GTK_FILE_CHOOSER_ACTION_OPEN,
        ui->exporting ? "Export" : "Review", "Cancel");
    if (ui->exporting) gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(ui->chooser), "workspace-layouts.umicom-layouts");
    g_signal_connect(ui->chooser, "response", G_CALLBACK(chosen), ui);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(ui->chooser)); release(ui);
}
#endif
    /* Configure the supported asynchronous chooser without changing review or export authority. */
    if (ui->destroyed) { finish_io(ui); release(ui); return; }
    GtkRoot *root = gtk_widget_get_root(ui->root);
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, ui->exporting ? "Export layout library" : "Review layout file");
    gtk_file_dialog_set_accept_label(dialog, ui->exporting ? "Export" : "Review");
    if (ui->exporting)
        gtk_file_dialog_set_initial_name(dialog, "workspace-layouts.umicom-layouts");
    if (ui->destroyed) {
        g_object_unref(dialog);
        finish_io(ui); release(ui); return;
    }
    ui->chooser = dialog;
    /* GTK retains the dialog during the asynchronous call. Our separate
     * controller reference keeps byte buffers alive until chosen completes. */
    if (ui->exporting)
        gtk_file_dialog_save(dialog, GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL,
            ui->cancel, chosen, retain(ui));
    else
        gtk_file_dialog_open(dialog, GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL,
            ui->cancel, chosen, retain(ui));
    release(ui);
}
static void apply_review(GtkButton *button, gpointer data)
{
    UmiGtk4WorkspaceLibraryExchange *ui = retain(data); (void)button;
    if (ui->destroyed || ui->busy || ui->review == NULL ||
        !gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->confirm))) { release(ui); return; }
    ui->busy = true; controls(ui);
    const bool saved = ui->saved_review;
    const UmiStatus status = ui->destroyed ? UMI_STATUS_CANCELLED : ui->apply_handler(ui->review, ui->context);
    if (!ui->destroyed) {
        forget_review(ui);
        message(ui, status == UMI_STATUS_OK ? "Imported layouts applied. Use Save library to keep them for restart."
            : "The import was not applied. Review the file again against the current workspace.");
        if (saved) message(ui, status == UMI_STATUS_OK
            ? "Reviewed saved layouts applied. Storage is unchanged. If Save reports a conflict, use the confirmed Restore library action to reread the latest store before retrying."
            : "The saved review was not applied. Review saved layouts again against the current workspace.");
    }
    ui->busy = false; controls(ui); release(ui);
}
/* Keep the controller alive through callbacks and GTK notifications. A
 * failed replacement clears the old review so it cannot remain actionable. */
static void review_saved(GtkButton *button, gpointer data)
{
    (void)button;
    UmiGtk4WorkspaceLibraryExchange *ui = retain(data);
    if (ui->destroyed || ui->busy || ui->saved_handler == NULL) { release(ui); return; }
    ui->busy = true; forget_review(ui); controls(ui);
    UmiStatus status = ui->destroyed ? UMI_STATUS_CANCELLED
        : ui->saved_handler(&ui->review, ui->saved_context);
    if (!ui->destroyed && status == UMI_STATUS_OK && ui->review != NULL) {
        ui->saved_review = true;
        status = show_review(ui);
    } else if (status == UMI_STATUS_OK) status = UMI_STATUS_INVALID_STATE;
    if (!ui->destroyed) {
        if (status != UMI_STATUS_OK) {
            forget_review(ui);
            message(ui, "Saved layouts could not be reviewed. Finish layout editing and check storage. Nothing was applied.");
        } else message(ui, "Review the saved arrangements, then confirm and apply. Storage will not be reread or overwritten.");
    }
    ui->busy = false; controls(ui); release(ui);
}
UmiStatus umi_gtk4_ws_library_exchange_set_saved_review_handler(
    UmiGtk4WorkspaceLibraryExchange *ui,
    UmiGtk4WorkspaceLibrarySavedReviewHandler handler, void *context)
{
    if (ui == NULL || ui->destroyed) return UMI_STATUS_INVALID_ARGUMENT;
    if (ui->busy) return UMI_STATUS_BUSY;
    retain(ui); ui->busy = true; forget_review(ui);
    if (!ui->destroyed) { ui->saved_handler = handler; ui->saved_context = context; }
    ui->busy = false; controls(ui);
    const UmiStatus status = ui->destroyed ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
    release(ui); return status;
}
static GtkWidget *make_button(GtkWidget *box, const char *label, const char *id,
    GCallback callback, UmiGtk4WorkspaceLibraryExchange *ui)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    (void)umi_gtk4_automation_tag_widget(button, id);
    g_signal_connect(button, "clicked", callback, ui); gtk_box_append(GTK_BOX(box), button); return button;
}
UmiStatus umi_gtk4_ws_library_exchange_create(
    UmiGtk4WorkspaceLibraryExportHandler export_handler,
    UmiGtk4WorkspaceLibraryImportHandler import_handler,
    UmiGtk4WorkspaceLibraryImportApplyHandler apply_handler,
    void *context, UmiGtk4WorkspaceLibraryExchange **out_exchange)
{
    if (export_handler == NULL || import_handler == NULL || apply_handler == NULL || out_exchange == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiGtk4WorkspaceLibraryExchange *ui = g_try_new0(UmiGtk4WorkspaceLibraryExchange, 1);
    if (ui == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    ui->references = 1U; ui->export_handler = export_handler; ui->import_handler = import_handler;
    ui->apply_handler = apply_handler; ui->context = context; ui->cancel = g_cancellable_new();
    ui->root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(ui->root), row);
    ui->export_button = make_button(row, "Export layouts…", "workstation.layout-library.export", G_CALLBACK(choose_file), ui);
    ui->import_button = make_button(row, "Review layout file…", "workstation.layout-library.import", G_CALLBACK(choose_file), ui);
    ui->saved_button = make_button(row, "Review saved layouts", "workstation.layout-library.review-saved", G_CALLBACK(review_saved), ui);
    ui->summary = gtk_label_new(""); gtk_label_set_wrap(GTK_LABEL(ui->summary), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(ui->summary), 64); gtk_label_set_xalign(GTK_LABEL(ui->summary), 0.0F);
    gtk_label_set_selectable(GTK_LABEL(ui->summary), TRUE); gtk_box_append(GTK_BOX(ui->root), ui->summary);
    ui->details = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    (void)umi_gtk4_automation_tag_widget(ui->details, "workstation.layout-library.import-details");
    gtk_box_append(GTK_BOX(ui->root), ui->details);
    ui->confirm = gtk_check_button_new_with_label("Replace my named layouts with this reviewed library");
    gtk_box_append(GTK_BOX(ui->root), ui->confirm); g_signal_connect(ui->confirm, "toggled", G_CALLBACK(confirmed), ui);
    ui->apply_button = make_button(ui->root, "Apply reviewed layouts", "workstation.layout-library.apply-import", G_CALLBACK(apply_review), ui);
    ui->status = gtk_label_new("Export or review a layout file without changing saved library storage.");
    gtk_label_set_wrap(GTK_LABEL(ui->status), TRUE); gtk_label_set_max_width_chars(GTK_LABEL(ui->status), 64);
    gtk_label_set_xalign(GTK_LABEL(ui->status), 0.0F); gtk_box_append(GTK_BOX(ui->root), ui->status);
    (void)umi_gtk4_automation_tag_widget(ui->confirm, "workstation.layout-library.confirm-import");
    (void)umi_gtk4_automation_tag_widget(ui->summary, "workstation.layout-library.import-summary");
    (void)umi_gtk4_automation_tag_widget(ui->status, "workstation.layout-library.import-status");
    controls(ui); *out_exchange = ui; return UMI_STATUS_OK;
}
GtkWidget *umi_gtk4_ws_library_exchange_widget(UmiGtk4WorkspaceLibraryExchange *ui)
{ return ui != NULL && !ui->destroyed ? ui->root : NULL; }
void umi_gtk4_ws_library_exchange_destroy(UmiGtk4WorkspaceLibraryExchange *ui)
{
    if (ui == NULL || ui->destroyed) return;
    ui->destroyed = true; g_cancellable_cancel(ui->cancel);
    g_signal_handlers_disconnect_by_data(ui->saved_button, ui);
    ui->saved_handler = NULL; ui->saved_context = NULL;
    g_signal_handlers_disconnect_by_data(ui->export_button, ui); g_signal_handlers_disconnect_by_data(ui->import_button, ui);
    g_signal_handlers_disconnect_by_data(ui->apply_button, ui); g_signal_handlers_disconnect_by_data(ui->confirm, ui);
/* Cancellation above closes the asynchronous dialog. Release only our
 * dialog reference here; GTK retains its source and chosen owns the final
 * controller reference until the cancelled result has been consumed.
 * The superseded implementation is retained below for engineering review. */
#if 0
    if (ui->chooser != NULL) {
        g_signal_handlers_disconnect_by_data(ui->chooser, ui);
        gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(ui->chooser)); g_clear_object(&ui->chooser);
    }
#endif
    g_clear_object(&ui->chooser);
    gtk_widget_set_sensitive(ui->root, FALSE); release(ui);
}
