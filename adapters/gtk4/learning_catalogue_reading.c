/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_reading.c
 *
 * PURPOSE:
 *   Present owned reading structure, explicit local links, section navigation and per-panel text size.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "learning_catalogue_private.h"
#include "umicom/ui/gtk4/drop_down.h"
#include <string.h>

static UmiStatus Ready(LearningCatalogue *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    return owner->state.busy ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}
static GtkTextTag *TextTag(GtkTextBuffer *buffer, const char *name)
{
    GtkTextTag *tag = gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(buffer), name);
    return tag != NULL ? tag : gtk_text_buffer_create_tag(buffer, name, NULL);
}
static void Range(GtkTextBuffer *buffer, const char *text, size_t start,
    size_t length, GtkTextIter *first, GtkTextIter *last)
{
    /* The worker validates complete UTF-8 before building a model. Core ranges
     * are byte offsets; GTK offsets count Unicode characters, never bytes. */
    glong begin = g_utf8_pointer_to_offset(text, text + start);
    glong end = g_utf8_pointer_to_offset(text, text + start + length);
    gtk_text_buffer_get_iter_at_offset(buffer, first, (gint)begin);
    gtk_text_buffer_get_iter_at_offset(buffer, last, (gint)end);
}
static void ApplyReadingTags(LearningCatalogue *owner)
{
    if (owner->state.retired) return;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(owner->content));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    GtkTextTag *body = TextTag(buffer, "umicom-reading-body");
    if (owner->state.retired) return;
    double points = 12.0 * (double)owner->reading_zoom_percent / 100.0;
    g_object_set(body, "size-points", points, NULL);
    if (owner->state.retired) return;
    gtk_text_buffer_apply_tag(buffer, body, &first, &last);
    if (owner->reading_document == NULL || owner->state.retired) return;
    UmiBrowserReadingSummary summary;
    if (UmiBrowserReadingDocumentSummary(owner->reading_document, &summary) != UMI_STATUS_OK) return;
    const char *text = UmiBrowserReadingDocumentText(owner->reading_document);
    for (size_t i = 0U; i < summary.spans && !owner->state.retired; ++i) {
        UmiBrowserReadingSpan span;
        if (UmiBrowserReadingDocumentSpanAt(owner->reading_document, i, &span) != UMI_STATUS_OK) break;
        GtkTextTag *tag = NULL;
        if (span.kind == UMI_BROWSER_READING_HEADING) {
            char name[40]; g_snprintf(name, sizeof(name), "umicom-reading-heading-%u", span.heading_level);
            tag = TextTag(buffer, name);
            if (owner->state.retired) break;
            double scale = span.heading_level == 1U ? 1.6 : span.heading_level == 2U ? 1.35 : 1.15;
            g_object_set(tag, "weight", PANGO_WEIGHT_BOLD, "size-points", points*scale, NULL);
        } else if (span.kind == UMI_BROWSER_READING_CODE) {
            tag = TextTag(buffer, "umicom-reading-code");
            if (owner->state.retired) break;
            g_object_set(tag, "family", "monospace", NULL);
        } else if (span.kind == UMI_BROWSER_READING_STRONG) {
            tag = TextTag(buffer, "umicom-reading-strong");
            if (owner->state.retired) break;
            g_object_set(tag, "weight", PANGO_WEIGHT_BOLD, NULL);
        } else {
            tag = TextTag(buffer, "umicom-reading-emphasis");
            if (owner->state.retired) break;
            g_object_set(tag, "style", PANGO_STYLE_ITALIC, NULL);
        }
        if (owner->state.retired) break;
        Range(buffer, text, span.byte_start, span.byte_length, &first, &last);
        gtk_text_buffer_apply_tag(buffer, tag, &first, &last);
    }
    if (owner->state.retired) return;
    GtkTextTag *link_tag = TextTag(buffer, "umicom-reading-link");
    if (owner->state.retired) return;
    g_object_set(link_tag, "underline", PANGO_UNDERLINE_SINGLE, NULL);
    for (size_t i = 0U; i < summary.links && !owner->state.retired; ++i) {
        UmiBrowserReadingLink link;
        if (UmiBrowserReadingDocumentLinkAt(owner->reading_document, i, &link) != UMI_STATUS_OK) break;
        Range(buffer, text, link.byte_start, link.byte_length, &first, &last);
        gtk_text_buffer_apply_tag(buffer, link_tag, &first, &last);
    }
}
static UmiStatus PositionAnchor(LearningCatalogue *owner, const char *identifier)
{
    if (owner->reading_document == NULL) return UMI_STATUS_NOT_FOUND;
    size_t offset = 0U;
    UmiStatus status = UmiBrowserReadingDocumentAnchor(owner->reading_document, identifier, &offset);
    if (status != UMI_STATUS_OK) return status;
    const char *text = UmiBrowserReadingDocumentText(owner->reading_document);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(owner->content));
    GtkTextIter first, last;
    Range(buffer, text, offset, 0U, &first, &last);
    gtk_text_buffer_place_cursor(buffer, &first);
    if (!owner->state.retired)
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(owner->content), &first, 0.1, FALSE, 0.0, 0.0);
    return UMI_STATUS_OK;
}
/* Preserve structured-only publication for review. The replacement also owns
 * verified source text, giving Find the same immutable bytes as the display. */
#if 0
void LearningCatalogueReadingPublish(LearningCatalogue *owner, LearningCatalogueJob *job)
{
    /* Publish model ownership before any buffer/model notification. Callback
     * queries therefore see metadata for the same reader snapshot. Dispatch
     * keeps busy true until all publication callbacks have finished. */
    UmiBrowserReadingDocumentDestroy(owner->reading_document);
    owner->reading_document = job->reading_document; job->reading_document = NULL;
    owner->state.has_reading_structure = owner->reading_document != NULL;
    LearningCatalogueImagePublish(owner, job);
    if (owner->state.retired) return;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(owner->content));
    gtk_text_buffer_set_text(buffer, job->display != NULL ? job->display : "", -1);
    if (owner->state.retired) return;
    ApplyReadingTags(owner);
    if (owner->state.retired) return;
    GtkStringList *rows = gtk_string_list_new(NULL);
    UmiBrowserReadingSummary summary = {0};
    if (owner->reading_document != NULL)
        (void)UmiBrowserReadingDocumentSummary(owner->reading_document, &summary);
    const char *text = UmiBrowserReadingDocumentText(owner->reading_document);
    for (size_t i = 0U; i < summary.links; ++i) {
        UmiBrowserReadingLink link;
        (void)UmiBrowserReadingDocumentLinkAt(owner->reading_document, i, &link);
        char *caption = g_strndup(text + link.byte_start, link.byte_length);
        char *row = g_strdup_printf("%s — %s", caption[0] != '\0' ? caption : "(empty link)", link.target);
        gtk_string_list_append(rows, row); g_free(row); g_free(caption);
    }
    gtk_drop_down_set_model(GTK_DROP_DOWN(owner->reading_links), G_LIST_MODEL(rows));
    g_object_unref(rows);
    if (owner->state.retired) return;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->reading_links),
        summary.links != 0U ? 0U : GTK_INVALID_LIST_POSITION);
    if (owner->state.retired) return;
    GtkStringList *images = gtk_string_list_new(NULL);
    for (size_t i = 0U; i < summary.image_descriptions; ++i) {
        UmiBrowserReadingImage image;
        (void)UmiBrowserReadingDocumentImageAt(owner->reading_document, i, &image);
        char *caption = g_strndup(text + image.byte_start, image.byte_length);
        char *row = g_strdup_printf("%s — %s", caption,
            image.target_status == UMI_STATUS_OK ? image.target : "source unavailable");
        gtk_string_list_append(images, row); g_free(row); g_free(caption);
    }
    gtk_drop_down_set_model(GTK_DROP_DOWN(owner->reading_images), G_LIST_MODEL(images));
    g_object_unref(images);
    if (owner->state.retired) return;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->reading_images),
        summary.image_descriptions != 0U ? 0U : GTK_INVALID_LIST_POSITION);
    if (!owner->state.retired && owner->state.reader.selection.fragment[0] != '\0')
        (void)PositionAnchor(owner, owner->state.reader.selection.fragment);
}
#endif

void LearningCatalogueReadingPublish(LearningCatalogue *owner, LearningCatalogueJob *job)
{
    /* Publish model ownership before any buffer/model notification. Callback
     * queries therefore see metadata for the same reader snapshot. Dispatch
     * keeps busy true until all publication callbacks have finished. */
    UmiBrowserReadingDocumentDestroy(owner->reading_document);
    owner->reading_document = job->reading_document; job->reading_document = NULL;
    owner->state.has_reading_structure = owner->reading_document != NULL;
    /* Transfer the already verified source before publication callbacks. The
     * next successful selection releases it; a refused read keeps it intact. */
    g_free(owner->reading_source);
    owner->reading_source = NULL;
    if (job->display_is_source) {
        owner->reading_source = job->display; job->display = NULL;
    }
    LearningCatalogueImagePublish(owner, job);
    if (owner->state.retired) return;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(owner->content));
    const char *display = owner->reading_source != NULL ? owner->reading_source : job->display;
    gtk_text_buffer_set_text(buffer, display != NULL ? display : "", -1);
    if (owner->state.retired) return;
    /* Replacing a buffer leaves its insert mark after the inserted text. A new
     * reading snapshot begins at the start; an explicit anchor below may move
     * it afterward. Find therefore starts from a visible, deterministic cursor
     * on Open, Back and Forward instead of wrapping from the end by accident. */
    GtkTextIter start;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_place_cursor(buffer, &start);
    if (owner->state.retired) return;
    ApplyReadingTags(owner);
    if (owner->state.retired) return;
    GtkStringList *rows = gtk_string_list_new(NULL);
    UmiBrowserReadingSummary summary = {0};
    if (owner->reading_document != NULL)
        (void)UmiBrowserReadingDocumentSummary(owner->reading_document, &summary);
    const char *text = UmiBrowserReadingDocumentText(owner->reading_document);
    for (size_t i = 0U; i < summary.links; ++i) {
        UmiBrowserReadingLink link;
        (void)UmiBrowserReadingDocumentLinkAt(owner->reading_document, i, &link);
        char *caption = g_strndup(text + link.byte_start, link.byte_length);
        char *row = g_strdup_printf("%s — %s", caption[0] != '\0' ? caption : "(empty link)", link.target);
        gtk_string_list_append(rows, row); g_free(row); g_free(caption);
    }
    gtk_drop_down_set_model(GTK_DROP_DOWN(owner->reading_links), G_LIST_MODEL(rows));
    g_object_unref(rows);
    if (owner->state.retired) return;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->reading_links),
        summary.links != 0U ? 0U : GTK_INVALID_LIST_POSITION);
    if (owner->state.retired) return;
    GtkStringList *images = gtk_string_list_new(NULL);
    for (size_t i = 0U; i < summary.image_descriptions; ++i) {
        UmiBrowserReadingImage image;
        (void)UmiBrowserReadingDocumentImageAt(owner->reading_document, i, &image);
        char *caption = g_strndup(text + image.byte_start, image.byte_length);
        char *row = g_strdup_printf("%s — %s", caption,
            image.target_status == UMI_STATUS_OK ? image.target : "source unavailable");
        gtk_string_list_append(images, row); g_free(row); g_free(caption);
    }
    gtk_drop_down_set_model(GTK_DROP_DOWN(owner->reading_images), G_LIST_MODEL(images));
    g_object_unref(images);
    if (owner->state.retired) return;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->reading_images),
        summary.image_descriptions != 0U ? 0U : GTK_INVALID_LIST_POSITION);
    /* Keep the original silent anchor request for review. Navigation already
     * opened verified content, so a missing section is a visible navigation
     * notice, not a failed read or permission to discard the new snapshot. */
#if 0
    if (!owner->state.retired && owner->state.reader.selection.fragment[0] != '\0')
        (void)PositionAnchor(owner, owner->state.reader.selection.fragment);
#endif
    if (!owner->state.retired && owner->state.reader.selection.fragment[0] != '\0') {
        UmiStatus anchor_status = PositionAnchor(owner, owner->state.reader.selection.fragment);
        if (anchor_status != UMI_STATUS_OK)
            g_strlcpy(job->notice,
                "Verified resource opened, but the requested anchor was not found in the displayed text.",
                sizeof(job->notice));
    }
}
UmiStatus UmiLearningCatalogueGtk4CopyReadingSummary(GtkWidget *widget, UmiBrowserReadingSummary *out_summary)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->reading_document == NULL) return UMI_STATUS_NOT_FOUND;
    return UmiBrowserReadingDocumentSummary(owner->reading_document, out_summary);
}
UmiStatus UmiLearningCatalogueGtk4ReadingLinkAt(GtkWidget *widget, size_t index, UmiBrowserReadingLink *out_link)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || out_link == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->reading_document == NULL) return UMI_STATUS_NOT_FOUND;
    return UmiBrowserReadingDocumentLinkAt(owner->reading_document, index, out_link);
}
UmiStatus UmiLearningCatalogueGtk4OpenReadingLink(GtkWidget *widget, size_t index)
{
    UmiStatus status = Ready(LearningCatalogueOwner(widget));
    if (status != UMI_STATUS_OK) return status;
    UmiBrowserReadingLink link;
    status = UmiLearningCatalogueGtk4ReadingLinkAt(widget, index, &link);
    return status == UMI_STATUS_OK ? UmiLearningCatalogueGtk4Follow(widget, link.target) : status;
}
UmiStatus UmiLearningCatalogueGtk4ReadingImageAt(GtkWidget *widget, size_t index,
    UmiBrowserReadingImage *out_image)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || out_image == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->reading_document == NULL) return UMI_STATUS_NOT_FOUND;
    return UmiBrowserReadingDocumentImageAt(owner->reading_document, index, out_image);
}
UmiStatus UmiLearningCatalogueGtk4OpenReadingImage(GtkWidget *widget, size_t index)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = Ready(owner);
    if (status != UMI_STATUS_OK) return status;
    UmiBrowserReadingImage image;
    status = UmiLearningCatalogueGtk4ReadingImageAt(widget, index, &image);
    if (status != UMI_STATUS_OK) return status;
    if (image.target_status != UMI_STATUS_OK) return image.target_status;
    /* Resolve against the displayed reader's owned pack, never a newer shelf
     * revision. Only IMAGE resources enter this action; an img tag cannot turn
     * a declared source file or guide into a different navigation command. */
    UmiLearningPack *pack = NULL;
    status = UmiLearningReaderCopyPack(owner->reader, &pack);
    UmiLearningPackLink selected;
    if (status == UMI_STATUS_OK)
        status = UmiLearningPackResolveLink(pack, owner->state.reader.selection.resource.id,
            image.target, &selected);
    if (status == UMI_STATUS_OK && (selected.resource.kind != UMI_LEARNING_RESOURCE_IMAGE ||
        selected.fragment[0] != '\0' || strchr(image.target, '#') != NULL)) status = UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningPackDestroy(pack);
    return status == UMI_STATUS_OK ? UmiLearningCatalogueGtk4Follow(widget, image.target) : status;
}
UmiStatus UmiLearningCatalogueGtk4GoToAnchor(GtkWidget *widget, const char *identifier)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = Ready(owner);
    if (status != UMI_STATUS_OK) return status;
    g_object_ref(widget); owner->state.busy = 1;
    status = PositionAnchor(owner, identifier);
    owner->state.busy = 0; g_object_unref(widget);
    return status;
}
UmiStatus UmiLearningCatalogueGtk4SetReadingZoom(GtkWidget *widget, unsigned int percent)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = Ready(owner);
    if (status != UMI_STATUS_OK) return status;
    if (percent < 50U || percent > 200U) return UMI_STATUS_INVALID_ARGUMENT;
    g_object_ref(widget); owner->state.busy = 1;
    owner->reading_zoom_percent = percent; owner->state.reading_zoom_percent = percent;
    ApplyReadingTags(owner);
    if (!owner->state.retired) gtk_spin_button_set_value(GTK_SPIN_BUTTON(owner->reading_zoom), (double)percent);
    owner->state.busy = 0; g_object_unref(widget);
    return UMI_STATUS_OK;
}
static void ReadingAction(GtkButton *button, gpointer data)
{
    GtkWidget *widget = data; g_object_ref(widget);
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (Ready(owner) == UMI_STATUS_OK) {
        const char *action = g_object_get_data(G_OBJECT(button), "learning-reading-action");
        UmiStatus status;
        if (strcmp(action, "link") == 0) {
            guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->reading_links));
            status = index == GTK_INVALID_LIST_POSITION ? UMI_STATUS_NOT_FOUND :
                UmiLearningCatalogueGtk4OpenReadingLink(widget, (size_t)index);
        } else if (strcmp(action, "image") == 0) {
            guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->reading_images));
            status = index == GTK_INVALID_LIST_POSITION ? UMI_STATUS_NOT_FOUND :
                UmiLearningCatalogueGtk4OpenReadingImage(widget, (size_t)index);
        } else status = UmiLearningCatalogueGtk4GoToAnchor(widget,
            gtk_editable_get_text(GTK_EDITABLE(owner->reading_anchor)));
        /* Preserve the earlier notice for review. Refused illustration, link
         * and anchor actions now share a catalogue-wide notification boundary. */
#if 0
        if (status != UMI_STATUS_OK && !owner->state.retired)
            LearningCatalogueMessage(owner, status, "This reading link or anchor is not available under the local guide policy.");
#endif
        if (status != UMI_STATUS_OK && !owner->state.retired && !owner->state.busy) {
            owner->state.busy = 1;
            LearningCatalogueMessage(owner, status,
                "This reading link, illustration or anchor is not available under the local guide policy.");
            owner->state.busy = 0;
        }
    }
    g_object_unref(widget);
}
static void ZoomChanged(GtkSpinButton *spin, gpointer data)
{
    (void)UmiLearningCatalogueGtk4SetReadingZoom(GTK_WIDGET(data), (unsigned int)gtk_spin_button_get_value_as_int(spin));
}
static void Tag(GtkWidget *widget, const char *id)
{ g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free); }
static void Label(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE); gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label);
}
static void Button(LearningCatalogue *owner, GtkWidget *box, const char *label, const char *action, const char *id)
{
    GtkWidget *button = gtk_button_new_with_label(label); Tag(button, id);
    g_object_set_data(G_OBJECT(button), "learning-reading-action", (gpointer)action);
    g_signal_connect_object(button, "clicked", G_CALLBACK(ReadingAction), owner->root, 0);
    gtk_box_append(GTK_BOX(box), button);
}
void LearningCatalogueReadingControls(LearningCatalogue *owner)
{
    owner->reading_zoom_percent = 100U; owner->state.reading_zoom_percent = 100U;
    GtkWidget *section = gtk_expander_new("Reading controls");
    Tag(section, "learning.catalogue.reading-section");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    Label(box, "Links from this guide. Only declared local resources can be opened.");
    /* Preserve direct construction for review. The shared helper makes the
     * model ownership transfer explicit and enforces the native panel contract. */
#if 0
    owner->reading_links = gtk_drop_down_new(G_LIST_MODEL(gtk_string_list_new(NULL)), NULL);
#endif
    owner->reading_links = umi_ui_gtk4_drop_down_new_take_string_list(gtk_string_list_new(NULL));
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(owner->reading_links), TRUE);
    Tag(owner->reading_links, "learning.catalogue.reading-links"); gtk_box_append(GTK_BOX(box), owner->reading_links);
    Button(owner, box, "Open selected reading link", "link", "learning.catalogue.reading-open-link");
    Label(box, "Illustrations described in this guide. Opening one is an explicit local request.");
    /* Preserve direct construction for review. The shared helper makes the
     * model ownership transfer explicit and enforces the native panel contract. */
#if 0
    owner->reading_images = gtk_drop_down_new(G_LIST_MODEL(gtk_string_list_new(NULL)), NULL);
#endif
    owner->reading_images = umi_ui_gtk4_drop_down_new_take_string_list(gtk_string_list_new(NULL));
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(owner->reading_images), TRUE);
    Tag(owner->reading_images, "learning.catalogue.reading-images");
    gtk_box_append(GTK_BOX(box), owner->reading_images);
    Button(owner, box, "Open selected illustration", "image", "learning.catalogue.reading-open-image");
    Label(box, "Go to an exact section identifier");
    owner->reading_anchor = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(owner->reading_anchor), (int)UMI_BROWSER_READING_ANCHOR_CAPACITY - 1);
    Tag(owner->reading_anchor, "learning.catalogue.reading-anchor"); gtk_box_append(GTK_BOX(box), owner->reading_anchor);
    Button(owner, box, "Go to section", "anchor", "learning.catalogue.reading-go-anchor");
    Label(box, "Text size, percent");
    owner->reading_zoom = gtk_spin_button_new_with_range(50.0, 200.0, 10.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(owner->reading_zoom), 100.0);
    Tag(owner->reading_zoom, "learning.catalogue.reading-zoom");
    g_signal_connect_object(owner->reading_zoom, "value-changed", G_CALLBACK(ZoomChanged), owner->root, 0);
    gtk_box_append(GTK_BOX(box), owner->reading_zoom);
    gtk_expander_set_child(GTK_EXPANDER(section), box); gtk_box_append(GTK_BOX(owner->controls), section);
    LearningCatalogueFindControls(owner);
}
