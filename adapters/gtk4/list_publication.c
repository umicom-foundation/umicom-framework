/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/list_publication.c
 * PURPOSE: Preserve row and container lifetime across native list observers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/list_publication.h"
#include <stdlib.h>

static const char ListPublicationKey[] = "umicom-list-publication-active";
static int ListPublicationMatches(GtkListBox *list, GtkWidget *const *expected, size_t count)
{
    for (size_t i = 0U; i < count; ++i)
        if (gtk_list_box_get_row_at_index(list, (int)i) != GTK_LIST_BOX_ROW(expected[i])) return 0;
    return gtk_list_box_get_row_at_index(list, (int)count) == NULL;
}
static void ListPublicationRelease(GtkWidget **widgets, size_t count)
{
    if (widgets == NULL) return;
    for (size_t i = 0U; i < count; ++i) g_clear_object(&widgets[i]);
    free(widgets);
}
UmiStatus UmiGtk4ListBoxPublish(GtkListBox *list, GtkWidget *const *rows, size_t count,
    UmiGtk4ListPublicationCurrent current, void *context)
{
    if (!GTK_IS_LIST_BOX(list) || current == NULL || (count != 0U && rows == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_GTK4_LIST_PUBLICATION_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (g_object_get_data(G_OBJECT(list), ListPublicationKey) != NULL) return UMI_STATUS_BUSY;
    for (size_t i = 0U; i < count; ++i) {
        if (!GTK_IS_WIDGET(rows[i]) || g_object_is_floating(rows[i]) ||
            gtk_widget_get_parent(rows[i]) != NULL || rows[i] == GTK_WIDGET(list) ||
            GTK_IS_ROOT(rows[i]) || gtk_widget_is_ancestor(GTK_WIDGET(list), rows[i]))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (rows[i] == rows[j]) return UMI_STATUS_INVALID_ARGUMENT;
    }
    GtkWidget **prepared = count != 0U ? calloc(count, sizeof *prepared) : NULL;
    GtkWidget **published = count != 0U ? calloc(count, sizeof *published) : NULL;
    GtkWidget **previous = calloc(UMI_GTK4_LIST_PUBLICATION_LIMIT, sizeof *previous);
    if (previous == NULL || (count != 0U && (prepared == NULL || published == NULL))) {
        free(prepared); free(published); free(previous);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    g_object_ref(list);
    /* Copy references before calling host code. Its observer may release the
     * original array, detach the list, or change which project owns the page. */
    for (size_t i = 0U; i < count; ++i) prepared[i] = g_object_ref(rows[i]);
    size_t previousCount = 0U, publishedCount = 0U;
    UmiStatus status = UMI_STATUS_OK;
    char reservation;
    g_object_set_data(G_OBJECT(list), ListPublicationKey, &reservation);
    for (;;) {
        GtkListBoxRow *row = gtk_list_box_get_row_at_index(list, (int)previousCount);
        if (row == NULL) break;
        if (previousCount == UMI_GTK4_LIST_PUBLICATION_LIMIT) {
            status = UMI_STATUS_CAPACITY_EXCEEDED; goto done;
        }
        previous[previousCount++] = g_object_ref(GTK_WIDGET(row));
    }
    status = current(list, context);
    if (status != UMI_STATUS_OK) goto done;
    if (!ListPublicationMatches(list, previous, previousCount)) { status = UMI_STATUS_INVALID_STATE; goto done; }
    for (size_t i = 0U; i < previousCount; ++i) {
        /* Never remove a row inserted by an observer in response to an earlier
         * removal. Stop and let the host capture a fresh page instead. */
        if (gtk_list_box_get_row_at_index(list, 0) != GTK_LIST_BOX_ROW(previous[i])) {
            status = UMI_STATUS_INVALID_STATE; goto done;
        }
        gtk_list_box_remove(list, previous[i]);
        status = current(list, context);
        if (status != UMI_STATUS_OK) goto done;
    }
    if (gtk_list_box_get_row_at_index(list, 0) != NULL) { status = UMI_STATUS_INVALID_STATE; goto done; }
    for (size_t i = 0U; i < count; ++i) {
        if (gtk_widget_get_parent(prepared[i]) != NULL) { status = UMI_STATUS_INVALID_STATE; goto done; }
        gtk_list_box_append(list, prepared[i]);
        status = current(list, context);
        if (status != UMI_STATUS_OK) goto done;
        GtkWidget *row = GTK_IS_LIST_BOX_ROW(prepared[i]) ? prepared[i] : gtk_widget_get_parent(prepared[i]);
        if (!GTK_IS_LIST_BOX_ROW(row) || gtk_widget_get_parent(row) != GTK_WIDGET(list) ||
            gtk_list_box_get_row_at_index(list, (int)i) != GTK_LIST_BOX_ROW(row)) {
            status = UMI_STATUS_INVALID_STATE; goto done;
        }
        published[publishedCount++] = g_object_ref(row);
        if (!ListPublicationMatches(list, published, publishedCount)) { status = UMI_STATUS_INVALID_STATE; goto done; }
    }
done:
    g_object_set_data(G_OBJECT(list), ListPublicationKey, NULL);
    ListPublicationRelease(previous, previousCount);
    ListPublicationRelease(published, publishedCount);
    ListPublicationRelease(prepared, count);
    g_object_unref(list);
    return status;
}
