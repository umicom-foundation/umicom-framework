/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/reading_document_gtk4.c
 *
 * PURPOSE:
 *   Capture Unicode reading ranges and safely apply shared native typography.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/reading_document.h"
#include <string.h>

#define READING_BUSY_KEY "umicom-reading-style-busy"
typedef struct ReadingNativeRange {
    gint first, last;
    unsigned int style;
} ReadingNativeRange;
typedef struct ReadingNativeSnapshot {
    char text[UMI_BROWSER_READING_TEXT_MAX + 1U];
    size_t bytes, count;
    ReadingNativeRange ranges[UMI_BROWSER_READING_SPANS_MAX + UMI_BROWSER_READING_LINKS_MAX];
    UmiUiAppearanceProfile appearance;
    double body_points, code_points;
} ReadingNativeSnapshot;
typedef struct ReadingNativeOperation {
    GtkTextBuffer *buffer;
    GtkTextTagTable *table;
    UmiReadingDocumentGtk4Continue keep_going;
    void *context;
    int text_changed, tag_removed;
} ReadingNativeOperation;

/* Model offsets are bytes; GTK offsets are Unicode characters. Validate the
 * full string first, then each prefix boundary before converting to gint.
 * All conversion occurs before tag notifications can release a model owner. */
static UmiStatus CaptureRange(ReadingNativeSnapshot *snapshot, size_t begin,
    size_t length, unsigned int style)
{
    if (begin > snapshot->bytes || length > snapshot->bytes - begin ||
        !g_utf8_validate(snapshot->text, (gssize)begin, NULL) ||
        !g_utf8_validate(snapshot->text + begin, (gssize)length, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    ReadingNativeRange *range = &snapshot->ranges[snapshot->count++];
    range->first = (gint)g_utf8_pointer_to_offset(snapshot->text, snapshot->text + begin);
    range->last = (gint)g_utf8_pointer_to_offset(snapshot->text, snapshot->text + begin + length);
    range->style = style;
    return UMI_STATUS_OK;
}
static UmiStatus Capture(const UmiBrowserReadingDocument *document,
    const UmiUiAppearanceProfile *appearance, unsigned int zoom,
    ReadingNativeSnapshot *snapshot)
{
    if (document == NULL || appearance == NULL || zoom < 50U || zoom > 200U)
        return UMI_STATUS_INVALID_ARGUMENT;
    char reason[192];
    UmiStatus status = umi_ui_appearance_profile_validate(appearance, reason, sizeof(reason));
    if (status != UMI_STATUS_OK) return status;
    UmiBrowserReadingSummary summary;
    status = UmiBrowserReadingDocumentSummary(document, &summary);
    if (status != UMI_STATUS_OK) return status;
    if (summary.text_bytes > UMI_BROWSER_READING_TEXT_MAX ||
        summary.spans > UMI_BROWSER_READING_SPANS_MAX ||
        summary.links > UMI_BROWSER_READING_LINKS_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *text = UmiBrowserReadingDocumentText(document);
    if (text == NULL || text[summary.text_bytes] != '\0' ||
        !g_utf8_validate(text, (gssize)summary.text_bytes, NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    snapshot->bytes = summary.text_bytes;
    memcpy(snapshot->text, text, summary.text_bytes + 1U);
    snapshot->appearance = *appearance;
    double factor = appearance->font_scale * (double)zoom / 100.0;
    snapshot->body_points = appearance->interface_font_size * factor;
    snapshot->code_points = appearance->editor_font_size * factor;
    for (size_t i = 0U; i < summary.spans; ++i) {
        UmiBrowserReadingSpan span;
        status = UmiBrowserReadingDocumentSpanAt(document, i, &span);
        if (status != UMI_STATUS_OK) return status;
        unsigned int style;
        switch (span.kind) {
        case UMI_BROWSER_READING_HEADING:
            if (span.heading_level < 1U || span.heading_level > 6U)
                return UMI_STATUS_INVALID_ARGUMENT;
            style = span.heading_level; break;
        case UMI_BROWSER_READING_CODE: style = 7U; break;
        case UMI_BROWSER_READING_STRONG: style = 8U; break;
        case UMI_BROWSER_READING_EMPHASIS:
        case UMI_BROWSER_READING_IMAGE_DESCRIPTION: style = 9U; break;
        default: return UMI_STATUS_INVALID_ARGUMENT;
        }
        if (span.kind != UMI_BROWSER_READING_HEADING && span.heading_level != 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        status = CaptureRange(snapshot, span.byte_start, span.byte_length, style);
        if (status != UMI_STATUS_OK) return status;
    }
    for (size_t i = 0U; i < summary.links; ++i) {
        UmiBrowserReadingLink link;
        status = UmiBrowserReadingDocumentLinkAt(document, i, &link);
        if (status != UMI_STATUS_OK) return status;
        status = CaptureRange(snapshot, link.byte_start, link.byte_length, 10U);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
static void TextChanged(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer;
    ((ReadingNativeOperation *)data)->text_changed = 1;
}
static void TagRemoved(GtkTextTagTable *table, GtkTextTag *tag, gpointer data)
{
    (void)table; (void)tag;
    ((ReadingNativeOperation *)data)->tag_removed = 1;
}
static UmiStatus Continuing(ReadingNativeOperation *operation)
{
    if (operation->text_changed || operation->tag_removed) return UMI_STATUS_INVALID_STATE;
    if (operation->keep_going != NULL && !operation->keep_going(operation->context))
        return UMI_STATUS_CANCELLED;
    return (operation->text_changed || operation->tag_removed) ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}
static const char *TagName(unsigned int style)
{
    static const char *const names[] = {
        "umicom-reading-body", "umicom-reading-heading-1", "umicom-reading-heading-2",
        "umicom-reading-heading-3", "umicom-reading-heading-4", "umicom-reading-heading-5",
        "umicom-reading-heading-6", "umicom-reading-code", "umicom-reading-strong",
        "umicom-reading-emphasis", "umicom-reading-link"
    };
    return names[style];
}
static UmiStatus ConfigureTag(ReadingNativeOperation *operation,
    const ReadingNativeSnapshot *snapshot, unsigned int style, GtkTextTag **out_tag)
{
    GtkTextTag *tag = gtk_text_tag_table_lookup(operation->table, TagName(style));
    if (tag != NULL) g_object_ref(tag);
    else {
        /* Own the tag before table notification. A callback may remove it from
         * the table immediately; the local reference keeps this pointer valid. */
        tag = gtk_text_tag_new(TagName(style));
        if (!gtk_text_tag_table_add(operation->table, tag)) {
            g_object_unref(tag); return UMI_STATUS_INVALID_STATE;
        }
    }
    UmiStatus status = Continuing(operation);
    if (status == UMI_STATUS_OK &&
        gtk_text_tag_table_lookup(operation->table, TagName(style)) != tag)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        if (style == 0U) {
            g_object_set(tag, "family", snapshot->appearance.interface_font,
                "size-points", snapshot->body_points, NULL);
        } else if (style <= 6U) {
            double scale = style == 1U ? 1.6 : style == 2U ? 1.35 : 1.15;
            g_object_set(tag, "weight", PANGO_WEIGHT_BOLD,
                "size-points", snapshot->body_points * scale, NULL);
        } else if (style == 7U) {
            g_object_set(tag, "family", snapshot->appearance.editor_font,
                "size-points", snapshot->code_points, NULL);
        } else if (style == 8U) g_object_set(tag, "weight", PANGO_WEIGHT_BOLD, NULL);
        else if (style == 9U) g_object_set(tag, "style", PANGO_STYLE_ITALIC, NULL);
        else g_object_set(tag, "underline", PANGO_UNDERLINE_SINGLE, NULL);
        status = Continuing(operation);
    }
    if (status == UMI_STATUS_OK &&
        gtk_text_tag_table_lookup(operation->table, TagName(style)) != tag)
        status = UMI_STATUS_INVALID_STATE;
    /* Body attributes cannot override later code/heading roles, even when a
     * caller has supplied a tag table in a different creation order. */
    if (status == UMI_STATUS_OK && style == 0U) {
        gtk_text_tag_set_priority(tag, 0);
        status = Continuing(operation);
    }
    if (status == UMI_STATUS_OK) *out_tag = tag;
    else g_object_unref(tag);
    return status;
}
static UmiStatus ApplySnapshot(GtkTextBuffer *buffer, const ReadingNativeSnapshot *snapshot,
    UmiReadingDocumentGtk4Continue keep_going, void *context)
{
    if (g_object_get_data(G_OBJECT(buffer), READING_BUSY_KEY) != NULL) return UMI_STATUS_BUSY;
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *current = gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
    int matches = strcmp(current, snapshot->text) == 0;
    g_free(current);
    if (!matches) return UMI_STATUS_INVALID_STATE;
    ReadingNativeOperation operation = {buffer, gtk_text_buffer_get_tag_table(buffer),
        keep_going, context, 0, 0};
    g_object_ref(buffer);
    g_object_set_data(G_OBJECT(buffer), READING_BUSY_KEY, &operation);
    gulong handler = g_signal_connect(buffer, "changed", G_CALLBACK(TextChanged), &operation);
    gulong removed = g_signal_connect(operation.table, "tag-removed", G_CALLBACK(TagRemoved), &operation);
    UmiStatus status = Continuing(&operation);
    /* Clear only our ranges before restyling; unrelated annotations survive.
     * Recompute iterators after every callback boundary and abort on text edits. */
    for (unsigned int i = 0U; i <= 10U && status == UMI_STATUS_OK; ++i) {
        GtkTextTag *tag = gtk_text_tag_table_lookup(operation.table, TagName(i));
        if (tag != NULL) {
            g_object_ref(tag);
            gtk_text_buffer_get_bounds(buffer, &first, &last);
            gtk_text_buffer_remove_tag(buffer, tag, &first, &last);
            status = Continuing(&operation);
            g_object_unref(tag);
        }
    }
    GtkTextTag *tags[11] = {0};
    if (status == UMI_STATUS_OK) status = ConfigureTag(&operation, snapshot, 0U, &tags[0]);
    if (status == UMI_STATUS_OK) {
        gtk_text_buffer_get_bounds(buffer, &first, &last);
        gtk_text_buffer_apply_tag(buffer, tags[0], &first, &last);
        status = Continuing(&operation);
    }
    for (size_t i = 0U; i < snapshot->count && status == UMI_STATUS_OK; ++i) {
        const ReadingNativeRange *range = &snapshot->ranges[i];
        if (tags[range->style] == NULL)
            status = ConfigureTag(&operation, snapshot, range->style, &tags[range->style]);
        if (status == UMI_STATUS_OK &&
            gtk_text_tag_table_lookup(operation.table, TagName(range->style)) != tags[range->style])
            status = UMI_STATUS_INVALID_STATE;
        if (status == UMI_STATUS_OK) {
            gtk_text_buffer_get_iter_at_offset(buffer, &first, range->first);
            gtk_text_buffer_get_iter_at_offset(buffer, &last, range->last);
            gtk_text_buffer_apply_tag(buffer, tags[range->style], &first, &last);
            status = Continuing(&operation);
        }
    }
    for (unsigned int i = 0U; i <= 10U; ++i) g_clear_object(&tags[i]);
    g_signal_handler_disconnect(operation.table, removed);
    g_signal_handler_disconnect(buffer, handler);
    g_object_set_data(G_OBJECT(buffer), READING_BUSY_KEY, NULL);
    g_object_unref(buffer);
    return status;
}
UmiStatus UmiReadingDocumentGtk4Style(const UmiBrowserReadingDocument *document,
    GtkTextBuffer *buffer, const UmiUiAppearanceProfile *appearance,
    unsigned int zoom_percent, UmiReadingDocumentGtk4Continue keep_going, void *context)
{
    if (buffer == NULL || !GTK_IS_TEXT_BUFFER(buffer)) return UMI_STATUS_INVALID_ARGUMENT;
    if (g_object_get_data(G_OBJECT(buffer), READING_BUSY_KEY) != NULL) return UMI_STATUS_BUSY;
    ReadingNativeSnapshot *snapshot = g_try_new0(ReadingNativeSnapshot, 1);
    if (snapshot == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = Capture(document, appearance, zoom_percent, snapshot);
    if (status == UMI_STATUS_OK) status = ApplySnapshot(buffer, snapshot, keep_going, context);
    g_free(snapshot);
    return status;
}
UmiStatus UmiReadingDocumentGtk4CreateBuffer(const UmiBrowserReadingDocument *document,
    const UmiUiAppearanceProfile *appearance, unsigned int zoom_percent, GtkTextBuffer **out_buffer)
{
    if (out_buffer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ReadingNativeSnapshot *snapshot = g_try_new0(ReadingNativeSnapshot, 1);
    if (snapshot == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = Capture(document, appearance, zoom_percent, snapshot);
    if (status == UMI_STATUS_OK) {
        GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);
        gtk_text_buffer_set_text(buffer, snapshot->text, (gint)snapshot->bytes);
        status = ApplySnapshot(buffer, snapshot, NULL, NULL);
        if (status == UMI_STATUS_OK) *out_buffer = buffer;
        else g_object_unref(buffer);
    }
    g_free(snapshot);
    return status;
}
