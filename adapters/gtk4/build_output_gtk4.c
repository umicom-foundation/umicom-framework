/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/build_output_gtk4.c
 * PURPOSE: Keep compiler output responsive while preserving a deliberately paused display.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


/* Text repair, selection, pause and control lifetime now belong to the shared
 * output presenter. This build-specific implementation is retained for review;
 * the adapter below preserves its public API and automation control names. */
#if 0
#include "umicom/build/live_output_gtk4.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

typedef struct BuildOutputView {
    UmiBuildOutputSnapshot latest;
    GtkWidget *text;
    GtkWidget *status;
    GtkWidget *follow;
    uint64_t displayed_operation;
    uint64_t displayed_revision;
    gboolean displayed;
} BuildOutputView;

/* Semantic tags let native tests and accessibility tooling find controls
 * without relying on their visual order or translated button labels. */
static void tag(GtkWidget *widget, const char *name)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(name), g_free);
}

/* Show transport state separately from text; bytes arriving do not imply that
 * a compiler succeeded or that a paused display contains the latest evidence. */
static void describe(BuildOutputView *view)
{
    const UmiBuildOutputSnapshot *output = &view->latest;
    gboolean pending = !view->displayed || view->displayed_operation != output->operation_id ||
        view->displayed_revision != output->revision || output->counters_saturated;
    if (output->operation_id == 0U) {
        gtk_label_set_text(GTK_LABEL(view->status), "Ready. Build output appears here after submission.");
        return;
    }
    char *label = g_strdup_printf("Latest capture: %s — %s | %zu bytes retained of %" PRIu64 "%s%s%s%s",
        umi_build_phase_text(output->phase),
        output->phase_complete ? umi_status_text(output->status) : "running; result pending",
        output->length, output->total_bytes,
        output->truncated ? " | earlier bytes omitted" : "",
        output->streamed ? "" : " | final output only",
        gtk_check_button_get_active(GTK_CHECK_BUTTON(view->follow)) ? "" :
            (pending ? " | paused; newer output available" : " | paused"),
        output->counters_saturated ? " | byte/revision counter limit reached" : "");
    gtk_label_set_text(GTK_LABEL(view->status), label);
    g_free(label);
}

/* Raw process bytes need not form a C string. Make NUL visible before UTF-8
 * repair, so text after it is not accidentally hidden from the developer. */
static void display_latest(BuildOutputView *view)
{
    GString *bytes = g_string_sized_new(view->latest.length + 1U);
    for (size_t index = 0U; index < view->latest.length; ++index) {
        if (view->latest.bytes[index] == '\0') g_string_append(bytes, "\xe2\x90\x80");
        else g_string_append_c(bytes, view->latest.bytes[index]);
    }
    char *valid = g_utf8_make_valid(bytes->str, (gssize)bytes->len);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view->text));
    gtk_text_buffer_set_text(buffer, valid, -1);
    view->displayed_operation = view->latest.operation_id;
    view->displayed_revision = view->latest.revision;
    view->displayed = TRUE;
    if (gtk_check_button_get_active(GTK_CHECK_BUTTON(view->follow))) {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view->text), &end, 0.0, FALSE, 0.0, 0.0);
    }
    g_free(valid);
    g_string_free(bytes, TRUE);
    describe(view);
}

/* Signal connections borrow the panel weakly. A retained button cannot keep
 * the panel alive or reach a freed application after its parent is destroyed. */
static void refresh_clicked(GtkButton *button, GtkWidget *panel)
{
    if (!gtk_widget_is_ancestor(GTK_WIDGET(button), panel)) return;
    BuildOutputView *view = g_object_get_data(G_OBJECT(panel), "umicom-build-output-view");
    if (view != NULL) display_latest(view);
}

static void follow_toggled(GtkCheckButton *button, GtkWidget *panel)
{
    if (!gtk_widget_is_ancestor(GTK_WIDGET(button), panel)) return;
    BuildOutputView *view = g_object_get_data(G_OBJECT(panel), "umicom-build-output-view");
    if (view == NULL) return;
    if (gtk_check_button_get_active(button)) display_latest(view);
    else describe(view);
}

static void copy_clicked(GtkButton *button, GtkWidget *panel)
{
    if (!gtk_widget_is_ancestor(GTK_WIDGET(button), panel)) return;
    BuildOutputView *view = g_object_get_data(G_OBJECT(panel), "umicom-build-output-view");
    if (view == NULL) return;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view->text));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    gdk_clipboard_set_text(gtk_widget_get_clipboard(panel), text);
    g_free(text);
}

/* State owns these references so GTK child disposal cannot leave dangling
 * display pointers while a retained panel still has a valid reference. */
static void release_view(gpointer data)
{
    BuildOutputView *view = data;
    g_clear_object(&view->text);
    g_clear_object(&view->status);
    g_clear_object(&view->follow);
    g_free(view);
}

/* Build a standalone presenter; no worker, timer or application is created. */
GtkWidget *UmiBuildOutputGtk4Create(void)
{
    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    BuildOutputView *view = g_new0(BuildOutputView, 1);
    g_object_set_data_full(G_OBJECT(panel), "umicom-build-output-view", view, release_view);
    tag(panel, "build.output.panel");
    GtkWidget *controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    view->follow = gtk_check_button_new_with_label("Follow output");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(view->follow), TRUE);
    GtkWidget *refresh = gtk_button_new_with_label("Refresh output");
    GtkWidget *copy = gtk_button_new_with_label("Copy displayed output");
    tag(view->follow, "build.output.follow"); tag(refresh, "build.output.refresh"); tag(copy, "build.output.copy");
    gtk_box_append(GTK_BOX(controls), view->follow);
    gtk_box_append(GTK_BOX(controls), refresh);
    gtk_box_append(GTK_BOX(controls), copy);
    gtk_box_append(GTK_BOX(panel), controls);
    view->status = gtk_label_new(NULL);
    gtk_label_set_wrap(GTK_LABEL(view->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(view->status), 0.0F);
    tag(view->status, "build.output.status");
    gtk_box_append(GTK_BOX(panel), view->status);
    view->text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view->text), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view->text), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->text), GTK_WRAP_NONE);
    gtk_widget_set_tooltip_text(view->text,
        "Current phase output. Pause Follow output to select text. NUL and invalid UTF-8 bytes are made visible.");
    tag(view->text, "build.output.text");
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 180);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view->text);
    gtk_box_append(GTK_BOX(panel), scroll);
    g_signal_connect_object(refresh, "clicked", G_CALLBACK(refresh_clicked), panel, 0);
    g_signal_connect_object(copy, "clicked", G_CALLBACK(copy_clicked), panel, 0);
    g_signal_connect_object(view->follow, "toggled", G_CALLBACK(follow_toggled), panel, 0);
    g_object_ref(view->text);
    g_object_ref(view->status);
    g_object_ref(view->follow);
    display_latest(view);
    return panel;
}

/* Validate the length before any copy or UTF-8 conversion. Reject older
 * evidence so delayed frontend work cannot replace a newer build's display. */
UmiStatus UmiBuildOutputGtk4Update(GtkWidget *panel, const UmiBuildOutputSnapshot *snapshot)
{
    if (panel == NULL || !GTK_IS_BOX(panel) || snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    BuildOutputView *view = g_object_get_data(G_OBJECT(panel), "umicom-build-output-view");
    if (view == NULL || snapshot->length >= sizeof(snapshot->bytes) ||
        snapshot->bytes[snapshot->length] != '\0' || snapshot->total_bytes < snapshot->length ||
        snapshot->phase_index >= UMI_BUILD_PROJECT_SESSION_MAX_PHASES ||
        snapshot->phase < UMI_BUILD_PHASE_CONFIGURE || snapshot->phase > UMI_BUILD_PHASE_DEPLOY ||
        (snapshot->operation_id == 0U && (snapshot->revision != 0U || snapshot->length != 0U)) ||
        (snapshot->operation_id != 0U && snapshot->revision == 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (snapshot->operation_id < view->latest.operation_id ||
        (snapshot->operation_id == view->latest.operation_id && snapshot->revision < view->latest.revision))
        return UMI_STATUS_INVALID_STATE;
    gboolean changed = snapshot->operation_id != view->latest.operation_id ||
        snapshot->revision != view->latest.revision || snapshot->counters_saturated;
    view->latest = *snapshot;
    if (changed && gtk_check_button_get_active(GTK_CHECK_BUTTON(view->follow))) display_latest(view);
    else describe(view);
    return UMI_STATUS_OK;
}

#endif

#include "umicom/build/live_output_gtk4.h"
#include "umicom/ui/gtk4/output_view.h"
#include <string.h>

GtkWidget *UmiBuildOutputGtk4Create(void)
{
    GtkWidget *panel = UmiOutputViewGtk4Create("build.output",
        "Ready. Build output appears here after submission.");
    g_object_set_data(G_OBJECT(panel), "umicom-build-output-adapter", GINT_TO_POINTER(1));
    return panel;
}

/* Adapt domain evidence without duplicating output ownership or text rendering. */
UmiStatus UmiBuildOutputGtk4Update(GtkWidget *panel, const UmiBuildOutputSnapshot *snapshot)
{
    if (panel == NULL || !GTK_IS_BOX(panel) || snapshot == NULL ||
        g_object_get_data(G_OBJECT(panel), "umicom-build-output-adapter") == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (snapshot->length >= sizeof(snapshot->bytes) ||
        snapshot->bytes[snapshot->length] != '\0' || snapshot->total_bytes < snapshot->length ||
        snapshot->phase_index >= UMI_BUILD_PROJECT_SESSION_MAX_PHASES ||
        snapshot->phase < UMI_BUILD_PHASE_CONFIGURE || snapshot->phase > UMI_BUILD_PHASE_DEPLOY ||
        (snapshot->operation_id == 0U && (snapshot->revision != 0U || snapshot->length != 0U)) ||
        (snapshot->operation_id != 0U && snapshot->revision == 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    _Static_assert(UMI_BUILD_LIVE_OUTPUT_CAPACITY <= UMI_OUTPUT_VIEW_CAPACITY, "Output view must retain build tail");
    UmiOutputViewSnapshot *view = g_new0(UmiOutputViewSnapshot, 1);
    view->operation_id = snapshot->operation_id;
    view->revision = snapshot->revision;
    view->length = snapshot->length;
    view->total_bytes = snapshot->total_bytes;
    view->truncated = snapshot->truncated;
    view->counters_saturated = snapshot->counters_saturated;
    memcpy(view->bytes, snapshot->bytes, snapshot->length + 1U);
    if (snapshot->operation_id != 0U) {
        (void)g_snprintf(view->context, sizeof(view->context), "Latest capture: %s",
            umi_build_phase_text(snapshot->phase));
        (void)g_snprintf(view->status, sizeof(view->status), "%s%s",
            snapshot->phase_complete ? umi_status_text(snapshot->status) : "running; result pending",
            snapshot->streamed ? "" : " | final output only");
    }
    UmiStatus status = UmiOutputViewGtk4Update(panel, view);
    g_free(view);
    return status;
}
