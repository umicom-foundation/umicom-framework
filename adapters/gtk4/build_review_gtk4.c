/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/build_review_gtk4.c
 *
 * PURPOSE:
 *   Display an immutable history snapshot without holding the producer or executing commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/review_gtk4.h"
#include "desktop_system_brand.inc"
#include <inttypes.h>
#include <string.h>

#define REVIEW_DATA "umicom-build-review-state"
typedef struct ReviewWindow {
    UmiBuildReview *review;
    GtkDropDown *operations, *severity;
    GtkEntry *query;
    GtkTextBuffer *buffer;
    int closing;
} ReviewWindow;
typedef struct ReviewSignal { GWeakRef window; } ReviewSignal;

static void Release(gpointer data)
{
    ReviewWindow *state = data;
    UmiBuildReviewDestroy(state->review);
    g_free(state);
}
static void SignalRelease(gpointer data, GClosure *closure)
{
    ReviewSignal *signal = data;
    (void)closure;
    g_weak_ref_clear(&signal->window);
    g_free(signal);
}
static void Update(ReviewWindow *state)
{
    UmiBuildReviewFilter filter = {0};
    const char *query;
    char *text = NULL;
    size_t length = 0U;
    UmiStatus status;
    if (state->closing || state->review == NULL) return;
    if (UmiBuildReviewCount(state->review) == 0U) {
        gtk_text_buffer_set_text(state->buffer, "No retained build operations. Run an operation in Studio, then open a new review. This window never starts work.", -1);
        return;
    }
    query = gtk_editable_get_text(GTK_EDITABLE(state->query));
    if (strlen(query) >= sizeof filter.contains) {
        gtk_text_buffer_set_text(state->buffer, "The filter is longer than 255 UTF-8 bytes. Shorten it; no partial filter has been applied.", -1);
        return;
    }
    strcpy(filter.contains, query);
    guint level = gtk_drop_down_get_selected(state->severity);
    filter.minimumSeverity = level <= 3U ? (UmiBuildDiagnosticSeverity)level : UMI_BUILD_DIAGNOSTIC_NOTE;
    status = UmiBuildReviewRender(state->review, (size_t)gtk_drop_down_get_selected(state->operations), &filter, &text, &length);
    if (status == UMI_STATUS_OK && length <= G_MAXINT)
        gtk_text_buffer_set_text(state->buffer, text, (int)length);
    else gtk_text_buffer_set_text(state->buffer, "The selected record could not be rendered. Close this window and take a fresh review.", -1);
    UmiBuildReviewTextFree(text);
}
static void ChangedCommon(GtkWidget *sender, ReviewSignal *signal)
{
    GtkWindow *window = g_weak_ref_get(&signal->window);
    if (window == NULL) return;
    /* An externally retained child can outlive its window. Both weak owner
     * promotion and current rooting must succeed before touching state. */
    if (gtk_widget_get_root(sender) == GTK_ROOT(window)) {
        ReviewWindow *state = g_object_get_data(G_OBJECT(window), REVIEW_DATA);
        if (state != NULL && !state->closing) Update(state);
    }
    g_object_unref(window);
}
static void Selected(GObject *sender, GParamSpec *spec, gpointer data)
{
    (void)spec; ChangedCommon(GTK_WIDGET(sender), data);
}
static void QueryChanged(GtkEditable *sender, gpointer data)
{
    ChangedCommon(GTK_WIDGET(sender), data);
}
static void Connect(GtkWidget *widget, const char *signalName, GCallback callback, GtkWindow *window)
{
    ReviewSignal *signal = g_new0(ReviewSignal, 1);
    g_weak_ref_init(&signal->window, G_OBJECT(window));
    g_signal_connect_data(widget, signalName, callback, signal, SignalRelease, 0);
}
static gboolean Closing(GtkWindow *window, gpointer data)
{
    (void)data;
    ReviewWindow *state = g_object_get_data(G_OBJECT(window), REVIEW_DATA);
    if (state != NULL) state->closing = 1;
    return FALSE;
}
static GtkWidget *Label(const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    return label;
}
static GtkWidget *Brand(const unsigned char *data, size_t length, int width, int height)
{
    GBytes *bytes = g_bytes_new_static(data, length);
    GdkTexture *texture = gdk_texture_new_from_bytes(bytes, NULL);
    GtkWidget *picture;
    g_bytes_unref(bytes);
    if (texture == NULL) return Label("Umicom");
    picture = gtk_picture_new_for_paintable(GDK_PAINTABLE(texture));
    g_object_unref(texture);
    gtk_widget_set_size_request(picture, width, height);
    gtk_widget_set_halign(picture, GTK_ALIGN_START);
    return picture;
}
GtkWindow *UmiGtk4BuildReviewPresent(GtkWindow *parent, const UmiBuildHistory *history)
{
    ReviewWindow *state = g_try_new0(ReviewWindow, 1);
    GtkWindow *window;
    GtkWidget *body, *row, *scroll, *view;
    GtkStringList *operations;
    UmiStatus status;
    if (state == NULL) return NULL;
    status = UmiBuildReviewCapture(history, 8U, &state->review);
    window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_title(window, "Umicom — Build history review");
    gtk_window_set_default_size(window, 960, 700);
    if (parent != NULL) {
        gtk_window_set_transient_for(window, parent);
        gtk_window_set_destroy_with_parent(window, TRUE);
        GtkApplication *application = gtk_window_get_application(parent);
        if (application != NULL) gtk_window_set_application(window, application);
    }
    g_object_set_data_full(G_OBJECT(window), REVIEW_DATA, state, Release);
    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(body, 16); gtk_widget_set_margin_bottom(body, 16);
    gtk_widget_set_margin_start(body, 16); gtk_widget_set_margin_end(body, 16);
    gtk_window_set_child(window, body);
    gtk_box_append(GTK_BOX(body), Brand(UMICOM_SYSTEM_LOGO, sizeof UMICOM_SYSTEM_LOGO, 202, 55));
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(row), Brand(UMICOM_SYSTEM_ICON, sizeof UMICOM_SYSTEM_ICON, 24, 24));
    gtk_box_append(GTK_BOX(row), Label("A frozen copy of up to eight retained operations. Close and reopen to see newer results."));
    gtk_box_append(GTK_BOX(body), row);
    operations = gtk_string_list_new(NULL);
    for (size_t i = 0U; i < UmiBuildReviewCount(state->review); ++i) {
        UmiBuildReviewSummary summary;
        char title[160];
        if (UmiBuildReviewSummarise(state->review, i, NULL, &summary) != UMI_STATUS_OK) continue;
        g_snprintf(title, sizeof title, "Operation %" PRIu64 " — %s", summary.operationId,
            UmiBuildReviewOutcomeText(summary.outcome));
        gtk_string_list_append(operations, title);
    }
    /* GtkDropDown takes ownership of this model; do not unref it again. */
    state->operations = GTK_DROP_DOWN(gtk_drop_down_new(G_LIST_MODEL(operations), NULL));
    gtk_widget_set_name(GTK_WIDGET(state->operations), "umicom-build-review-operation");
    gtk_box_append(GTK_BOX(body), Label("Recorded operation"));
    gtk_box_append(GTK_BOX(body), GTK_WIDGET(state->operations));
    static const char *const levels[] = {"All severities", "Warning and above", "Error and above", "Fatal only", NULL};
    state->severity = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(levels));
    gtk_widget_set_name(GTK_WIDGET(state->severity), "umicom-build-review-severity");
    gtk_box_append(GTK_BOX(body), Label("Minimum severity"));
    gtk_box_append(GTK_BOX(body), GTK_WIDGET(state->severity));
    state->query = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(state->query, "Literal text in path, code or message (case-sensitive)");
    gtk_widget_set_name(GTK_WIDGET(state->query), "umicom-build-review-filter");
    gtk_box_append(GTK_BOX(body), Label("Contains — filters Problems only, not the recorded output"));
    gtk_box_append(GTK_BOX(body), GTK_WIDGET(state->query));
    view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), TRUE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_name(view, "umicom-build-review-report");
    state->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(body), scroll);
    gtk_box_append(GTK_BOX(body), Label("Read-only. No build, test, file, database, source-navigation or process action is performed by this review."));
    if (status != UMI_STATUS_OK) {
        char message[200];
        g_snprintf(message, sizeof message, "Cannot capture history: %s. The producer was not changed.", umi_status_text(status));
        gtk_text_buffer_set_text(state->buffer, message, -1);
    } else {
        size_t count = UmiBuildReviewCount(state->review);
        if (count != 0U) gtk_drop_down_set_selected(state->operations, (guint)(count - 1U));
        Update(state);
    }
    Connect(GTK_WIDGET(state->operations), "notify::selected", G_CALLBACK(Selected), window);
    Connect(GTK_WIDGET(state->severity), "notify::selected", G_CALLBACK(Selected), window);
    Connect(GTK_WIDGET(state->query), "changed", G_CALLBACK(QueryChanged), window);
    g_signal_connect(window, "close-request", G_CALLBACK(Closing), NULL);
    gtk_window_present(window);
    return window;
}
