/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_find.c
 *
 * PURPOSE:
 *   Find literal text in an owned guide using the shared editor search engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "learning_catalogue_private.h"
#include "umicom/editor/search_engine.h"
#include <string.h>

static UmiStatus Ready(LearningCatalogue *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    return owner->state.busy ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}
UmiStatus UmiLearningCatalogueGtk4FindText(GtkWidget *widget, const char *query,
    int case_sensitive, int backwards, UmiLearningCatalogueFindMatch *out_match)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = Ready(owner);
    if (status != UMI_STATUS_OK) return status;
    if (query == NULL || (case_sensitive != 0 && case_sensitive != 1) ||
        (backwards != 0 && backwards != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = 0U;
    while (bytes < UMI_LEARNING_READING_FIND_CAPACITY && query[bytes] != '\0') ++bytes;
    if (bytes == UMI_LEARNING_READING_FIND_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (bytes == 0U || !g_utf8_validate(query, (gssize)bytes, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->reading_document == NULL) return UMI_STATUS_NOT_FOUND;
    const char *text = UmiBrowserReadingDocumentText(owner->reading_document);
    UmiBrowserReadingSummary summary;
    status = UmiBrowserReadingDocumentSummary(owner->reading_document, &summary);
    if (status != UMI_STATUS_OK) return status;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(owner->content));
    GtkTextIter first, last;
    if (!gtk_text_buffer_get_selection_bounds(buffer, &first, &last)) {
        gtk_text_buffer_get_iter_at_mark(buffer, &first, gtk_text_buffer_get_insert(buffer));
        last = first;
    }
    gint character = gtk_text_iter_get_offset(backwards ? &first : &last);
    glong characters = g_utf8_strlen(text, (gssize)summary.text_bytes);
    if (character < 0 || (glong)character > characters) return UMI_STATUS_INVALID_STATE;
    size_t start = (size_t)(g_utf8_offset_to_pointer(text, (glong)character) - text);
    UmiEditorSearchOptions options = {case_sensitive ? UMI_EDITOR_SEARCH_CASE_SENSITIVE :
        UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE, 0, 0, 0U};
    UmiEditorSearchMatch found;
    int wrapped = 0;
    status = UmiEditorSearchNavigate(text, summary.text_bytes, query, bytes,
        &options, start, backwards, 1, &found, &wrapped);
    if (status != UMI_STATUS_OK) return status;
    UmiLearningCatalogueFindMatch result = {owner->state.reader.revision,
        found.offset, found.byte_count, wrapped};
    /* Valid UTF-8 query and displayed text make each exact match a complete
     * character range. Search offsets remain bytes; GTK positions are characters.
     * Retain the root and block all mutating routes while selection signals run. */
    glong begin = g_utf8_pointer_to_offset(text, text + found.offset);
    glong end = g_utf8_pointer_to_offset(text, text + found.offset + found.byte_count);
    gtk_text_buffer_get_iter_at_offset(buffer, &first, (gint)begin);
    gtk_text_buffer_get_iter_at_offset(buffer, &last, (gint)end);
    g_object_ref(widget);
    owner->state.busy = 1;
    gtk_text_buffer_select_range(buffer, &last, &first);
    if (!owner->state.retired)
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(owner->content), &first, 0.1, FALSE, 0.0, 0.0);
    owner->state.busy = 0;
    if (out_match != NULL) *out_match = result;
    g_object_unref(widget);
    return UMI_STATUS_OK;
}
static void FindAction(LearningCatalogue *owner, int backwards)
{
    if (Ready(owner) != UMI_STATUS_OK) return;
    UmiLearningCatalogueFindMatch found;
    UmiStatus status = UmiLearningCatalogueGtk4FindText(owner->root,
        gtk_editable_get_text(GTK_EDITABLE(owner->find_query)),
        gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->find_case)) ? 1 : 0,
        backwards, &found);
    if (owner->state.retired || owner->state.busy) return;
    /* Search feedback is observable too: another route must not start work
     * inside its synchronous label notifications. The caller holds the root. */
    owner->state.busy = 1;
    LearningCatalogueMessage(owner, status, status == UMI_STATUS_OK ?
        (found.wrapped ? "Match selected after wrapping through the guide." : "Match selected in this guide.") :
        status == UMI_STATUS_NOT_FOUND ? "No matching text in the structured guide. Source and image fallbacks are not searched." :
        "Enter a nonempty UTF-8 query of at most 255 bytes.");
    owner->state.busy = 0;
}
static void Clicked(GtkButton *button, gpointer data)
{
    GtkWidget *root = GTK_WIDGET(data);
    g_object_ref(root);
    LearningCatalogue *owner = LearningCatalogueOwner(root);
    if (owner != NULL) FindAction(owner,
        GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "learning-find-backwards")));
    g_object_unref(root);
}
static void Activated(GtkEntry *entry, gpointer data)
{
    (void)entry;
    GtkWidget *root = GTK_WIDGET(data);
    g_object_ref(root);
    LearningCatalogue *owner = LearningCatalogueOwner(root);
    if (owner != NULL) FindAction(owner, 0);
    g_object_unref(root);
}
static gboolean KeyPressed(GtkEventControllerKey *keys, guint keyval,
    guint keycode, GdkModifierType modifiers, gpointer data)
{
    (void)keys; (void)keycode;
    const GdkModifierType commands = GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_ALT_MASK |
        GDK_SUPER_MASK | GDK_HYPER_MASK | GDK_META_MASK;
    if ((keyval != GDK_KEY_f && keyval != GDK_KEY_F) ||
        (modifiers & commands) != GDK_CONTROL_MASK) return FALSE;
    GtkWidget *root = GTK_WIDGET(data);
    LearningCatalogue *owner = LearningCatalogueOwner(root);
    if (Ready(owner) != UMI_STATUS_OK) return FALSE;
    g_object_ref(root); owner->state.busy = 1;
    gtk_expander_set_expanded(GTK_EXPANDER(owner->find_section), TRUE);
    if (!owner->state.retired) gtk_widget_grab_focus(owner->find_query);
    owner->state.busy = 0; g_object_unref(root);
    return TRUE;
}
static void Tag(GtkWidget *widget, const char *id)
{ g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free); }
void LearningCatalogueFindControls(LearningCatalogue *owner)
{
    owner->find_section = gtk_expander_new("Find in this guide");
    Tag(owner->find_section, "learning.catalogue.find-section");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *label = gtk_label_new("Find literal text in the displayed structured guide. Ctrl+F opens these controls.");
    gtk_label_set_wrap(GTK_LABEL(label), TRUE); gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label);
    owner->find_query = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(owner->find_query), (int)UMI_LEARNING_READING_FIND_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->find_query), "Text to find");
    Tag(owner->find_query, "learning.catalogue.find-query");
    g_signal_connect_object(owner->find_query, "activate", G_CALLBACK(Activated), owner->root, 0);
    gtk_box_append(GTK_BOX(box), owner->find_query);
    owner->find_case = gtk_check_button_new_with_label("Match case (ASCII letters)");
    Tag(owner->find_case, "learning.catalogue.find-case");
    gtk_box_append(GTK_BOX(box), owner->find_case);
    for (int backwards = 0; backwards <= 1; ++backwards) {
        GtkWidget *button = gtk_button_new_with_label(backwards ? "Find previous" : "Find next");
        Tag(button, backwards ? "learning.catalogue.find-previous" : "learning.catalogue.find-next");
        g_object_set_data(G_OBJECT(button), "learning-find-backwards", GINT_TO_POINTER(backwards));
        g_signal_connect_object(button, "clicked", G_CALLBACK(Clicked), owner->root, 0);
        gtk_box_append(GTK_BOX(box), button);
    }
    gtk_expander_set_child(GTK_EXPANDER(owner->find_section), box);
    gtk_box_append(GTK_BOX(owner->controls), owner->find_section);
    GtkEventController *keys = gtk_event_controller_key_new();
    g_object_set_data(G_OBJECT(keys), "umicom-learning-find-controller", GINT_TO_POINTER(1));
    gtk_event_controller_set_propagation_phase(keys, GTK_PHASE_BUBBLE);
    g_signal_connect_object(keys, "key-pressed", G_CALLBACK(KeyPressed), owner->root, 0);
    gtk_widget_add_controller(owner->root, keys);
}
