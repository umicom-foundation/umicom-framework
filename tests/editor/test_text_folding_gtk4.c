/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_text_folding_gtk4.c
 * PURPOSE: Verify manual folding preserves complete source and respects GTK callback and buffer ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/text_folding.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
/* The fixture owns unparented views and their buffers. No user project,
 * provider, filesystem or application settings are accessed by these cases. */
static void Select(GtkTextBuffer *buffer, int first, int end, int reverse)
{
    GtkTextIter a, b;
    gtk_text_buffer_get_iter_at_offset(buffer, &a, first);
    gtk_text_buffer_get_iter_at_offset(buffer, &b, end);
    gtk_text_buffer_select_range(buffer, reverse ? &b : &a, reverse ? &a : &b);
}
static gchar *Read(GtkTextBuffer *buffer, int hidden)
{
    GtkTextIter a, b;
    gtk_text_buffer_get_bounds(buffer, &a, &b);
    return gtk_text_buffer_get_text(buffer, &a, &b, hidden != 0);
}
static void Changed(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer;
    ++*(unsigned *)data;
}
typedef struct Observer
{
    GtkTextView *view;
    const char *mode;
    int fired;
    UmiStatus nested;
} Observer;
static void Observe(GtkTextBuffer *buffer, Observer *observer)
{
    if (observer->fired)
        return;
    observer->fired = 1;
    if (strstr(observer->mode, "edit") != NULL)
        gtk_text_buffer_set_text(buffer, "host replacement", -1);
    else if (strstr(observer->mode, "navigation") != NULL)
    {
        GtkTextIter at;
        gtk_text_buffer_get_end_iter(buffer, &at);
        gtk_text_buffer_place_cursor(buffer, &at);
    }
    else if (strcmp(observer->mode, "apply-replace-buffer") == 0)
    {
        GtkTextBuffer *other = gtk_text_buffer_new(NULL);
        gtk_text_view_set_buffer(observer->view, other);
        g_object_unref(other);
    }
    else if (strcmp(observer->mode, "apply-nested") == 0)
        observer->nested = UmiGtk4TextFoldSelection(observer->view);
}
static void Apply(GtkTextBuffer *buffer, GtkTextTag *tag, GtkTextIter *a, GtkTextIter *b, gpointer data)
{
    (void)tag;
    (void)a;
    (void)b;
    Observe(buffer, data);
}
static void Added(GtkTextTagTable *table, GtkTextTag *tag, gpointer data)
{
    Observer *observer = data;
    if (strcmp(observer->mode, "tag-add-remove") == 0)
    {
        observer->fired = 1;
        gtk_text_tag_table_remove(table, tag);
    }
    else
        Observe(gtk_text_view_get_buffer(observer->view), observer);
}
static void Removed(GtkTextTagTable *table, GtkTextTag *tag, gpointer data)
{
    (void)table;
    (void)tag;
    Observer *observer = data;
    Observe(gtk_text_view_get_buffer(observer->view), observer);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {"basic",
                           "modified",
                           "header",
                           "partial-end",
                           "reverse",
                           "unicode",
                           "readonly",
                           "empty",
                           "single-line",
                           "no-selection",
                           "clear-empty",
                           "clear",
                           "repeated-clear",
                           "edit-reveals",
                           "delete-reveals",
                           "replace-reveals",
                           "caret-reveals",
                           "selection-reveals",
                           "unrelated-mark",
                           "visible-caret",
                           "multiple",
                           "capacity",
                           "independent",
                           "shared-buffer",
                           "shared-table",
                           "other-tags",
                           "buffer-switch",
                           "refold",
                           "apply-edit",
                           "apply-navigation",
                           "apply-replace-buffer",
                           "apply-nested",
                           "tag-add-edit",
                           "tag-add-navigation",
                           "tag-add-remove",
                           "tag-remove-edit",
                           "invalid",
                           "native-history"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    GtkTextView *view = GTK_TEXT_VIEW(g_object_ref_sink(gtk_text_view_new())), *other = NULL;
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(view));
    gchar *complete = NULL, *visible = NULL;
    GString *many = NULL;
    gulong hook = 0U;
    GObject *hook_owner = NULL;
    unsigned changes = 0U;
    const char *source =
        strcmp(mode, "unicode") == 0 ? "caf\xc3\xa9\n\xe9\x9b\xaa\nend\n" : "header\nbody\ntail\nend\n";
    gtk_text_buffer_set_enable_undo(buffer, FALSE);
    gtk_text_buffer_set_text(buffer, source, -1);
    gtk_text_buffer_set_modified(buffer, strcmp(mode, "modified") == 0);
    g_signal_connect(buffer, "changed", G_CALLBACK(Changed), &changes);
    if (strcmp(mode, "invalid") == 0)
    {
        CHECK(UmiGtk4TextFoldSelection(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4TextFoldClear(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4TextFoldCount(NULL) == 0U && !UmiGtk4TextFoldLineHidden(NULL, 1U));
        goto cleanup;
    }
    if (strcmp(mode, "clear-empty") == 0)
    {
        CHECK(UmiGtk4TextFoldClear(view) == UMI_STATUS_OK && UmiGtk4TextFoldCount(view) == 0U);
        goto cleanup;
    }
    if (strcmp(mode, "empty") == 0)
    {
        gtk_text_buffer_set_text(buffer, "", 0);
        CHECK(UmiGtk4TextFoldSelection(view) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    if (strcmp(mode, "no-selection") == 0)
    {
        CHECK(UmiGtk4TextFoldSelection(view) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    if (strcmp(mode, "single-line") == 0)
    {
        Select(buffer, 0, 3, 0);
        CHECK(UmiGtk4TextFoldSelection(view) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    if (strcmp(mode, "multiple") == 0 || strcmp(mode, "capacity") == 0)
    {
        many = g_string_new(NULL);
        for (unsigned i = 0U; i < 132U; ++i)
            g_string_append(many, "line\n");
        gtk_text_buffer_set_text(buffer, many->str, -1);
        unsigned limit = strcmp(mode, "multiple") == 0 ? 2U : 64U;
        for (unsigned i = 0U; i < limit; ++i)
        {
            Select(buffer, (int)(i * 10U), (int)(i * 10U + 10U), 0);
            CHECK(UmiGtk4TextFoldSelection(view) == UMI_STATUS_OK);
        }
        CHECK(UmiGtk4TextFoldCount(view) == limit);
        if (limit == 64U)
        {
            Select(buffer, 640, 650, 0);
            CHECK(UmiGtk4TextFoldSelection(view) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(UmiGtk4TextFoldCount(view) == 64U);
        }
        CHECK(UmiGtk4TextFoldLineHidden(view, 2U) && UmiGtk4TextFoldLineHidden(view, 4U));
        goto cleanup;
    }
    if (strcmp(mode, "readonly") == 0)
        gtk_text_view_set_editable(view, FALSE);
    if (strcmp(mode, "native-history") == 0)
    {
        gtk_text_buffer_set_enable_undo(buffer, TRUE);
        CHECK(!gtk_text_buffer_get_can_undo(buffer));
    }
    int end = strcmp(mode, "unicode") == 0 ? 7 : 12;
    if (strcmp(mode, "partial-end") == 0)
        end = 9;
    int start = strcmp(mode, "header") == 0 ? 2 : 0;
    Select(buffer, start, end, strcmp(mode, "reverse") == 0);
    Observer observer = {view, mode, 0, UMI_STATUS_OK};
    if (strncmp(mode, "apply-", 6U) == 0)
    {
        hook_owner = G_OBJECT(buffer);
        hook = g_signal_connect(buffer, "apply-tag", G_CALLBACK(Apply), &observer);
    }
    if (strncmp(mode, "tag-add-", 8U) == 0)
    {
        hook_owner = G_OBJECT(gtk_text_buffer_get_tag_table(buffer));
        hook = g_signal_connect(hook_owner, "tag-added", G_CALLBACK(Added), &observer);
    }
    UmiStatus status = UmiGtk4TextFoldSelection(view);
    if (hook != 0U)
    {
        g_signal_handler_disconnect(hook_owner, hook);
        hook = 0U;
    }
    if (strncmp(mode, "tag-add-", 8U) == 0 ||
        (strncmp(mode, "apply-", 6U) == 0 && strcmp(mode, "apply-nested") != 0))
    {
        CHECK(observer.fired && status == UMI_STATUS_INVALID_STATE && UmiGtk4TextFoldCount(view) == 0U);
        visible = Read(buffer, 0);
        complete = Read(buffer, 1);
        CHECK(strcmp(visible, complete) == 0);
        if (strstr(mode, "edit") != NULL)
            CHECK(strcmp(complete, "host replacement") == 0);
        goto cleanup;
    }
    CHECK(status == UMI_STATUS_OK && UmiGtk4TextFoldCount(view) == 1U);
    CHECK(!UmiGtk4TextFoldLineHidden(view, 0U) && !UmiGtk4TextFoldLineHidden(view, 1U));
    CHECK(UmiGtk4TextFoldLineHidden(view, 2U) && !UmiGtk4TextFoldLineHidden(view, 3U));
    complete = Read(buffer, 1);
    CHECK(strcmp(complete, source) == 0);
    g_clear_pointer(&complete, g_free);
    visible = Read(buffer, 0);
    CHECK(strcmp(visible, strcmp(mode, "unicode") == 0 ? "caf\xc3\xa9\nend\n" : "header\ntail\nend\n") == 0);
    g_clear_pointer(&visible, g_free);
    CHECK(changes == 0U && gtk_text_buffer_get_modified(buffer) == (strcmp(mode, "modified") == 0));
    GtkTextIter at;
    gtk_text_buffer_get_iter_at_mark(buffer, &at, gtk_text_buffer_get_insert(buffer));
    CHECK(gtk_text_iter_get_offset(&at) == start);
    CHECK(!gtk_text_buffer_get_has_selection(buffer));
    if (strcmp(mode, "apply-nested") == 0)
        CHECK(observer.fired && observer.nested == UMI_STATUS_BUSY);
    if (strcmp(mode, "native-history") == 0)
        CHECK(!gtk_text_buffer_get_can_undo(buffer));
    if (strcmp(mode, "clear") == 0 || strcmp(mode, "repeated-clear") == 0 || strcmp(mode, "refold") == 0)
    {
        CHECK(UmiGtk4TextFoldClear(view) == UMI_STATUS_OK && UmiGtk4TextFoldCount(view) == 0U);
        if (strcmp(mode, "repeated-clear") == 0)
            CHECK(UmiGtk4TextFoldClear(view) == UMI_STATUS_OK);
        visible = Read(buffer, 0);
        CHECK(strcmp(visible, source) == 0);
        if (strcmp(mode, "refold") == 0)
        {
            Select(buffer, 0, 12, 0);
            CHECK(UmiGtk4TextFoldSelection(view) == UMI_STATUS_OK && UmiGtk4TextFoldCount(view) == 1U);
        }
    }
    if (strcmp(mode, "edit-reveals") == 0)
    {
        gtk_text_buffer_get_end_iter(buffer, &at);
        gtk_text_buffer_insert(buffer, &at, "more", -1);
        CHECK(UmiGtk4TextFoldCount(view) == 0U);
    }
    if (strcmp(mode, "delete-reveals") == 0)
    {
        GtkTextIter finish;
        gtk_text_buffer_get_iter_at_offset(buffer, &at, 0);
        gtk_text_buffer_get_iter_at_offset(buffer, &finish, 1);
        gtk_text_buffer_delete(buffer, &at, &finish);
        CHECK(UmiGtk4TextFoldCount(view) == 0U);
    }
    if (strcmp(mode, "replace-reveals") == 0)
    {
        gtk_text_buffer_set_text(buffer, "new", -1);
        CHECK(UmiGtk4TextFoldCount(view) == 0U);
    }
    if (strcmp(mode, "caret-reveals") == 0)
    {
        gtk_text_buffer_get_iter_at_offset(buffer, &at, 8);
        gtk_text_buffer_place_cursor(buffer, &at);
        CHECK(UmiGtk4TextFoldCount(view) == 0U);
    }
    if (strcmp(mode, "selection-reveals") == 0)
    {
        Select(buffer, 0, 16, 0);
        CHECK(UmiGtk4TextFoldCount(view) == 0U);
    }
    if (strcmp(mode, "unrelated-mark") == 0)
    {
        gtk_text_buffer_get_iter_at_offset(buffer, &at, 8);
        gtk_text_buffer_create_mark(buffer, "annotation", &at, TRUE);
        CHECK(UmiGtk4TextFoldCount(view) == 1U);
    }
    if (strcmp(mode, "visible-caret") == 0)
    {
        gtk_text_buffer_get_end_iter(buffer, &at);
        gtk_text_buffer_place_cursor(buffer, &at);
        CHECK(UmiGtk4TextFoldCount(view) == 1U);
    }
    if (strcmp(mode, "independent") == 0 || strcmp(mode, "shared-buffer") == 0 ||
        strcmp(mode, "shared-table") == 0)
    {
        GtkTextBuffer *second = strcmp(mode, "shared-buffer") == 0
                                    ? g_object_ref(buffer)
                                    : gtk_text_buffer_new(strcmp(mode, "shared-table") == 0
                                                              ? gtk_text_buffer_get_tag_table(buffer)
                                                              : NULL);
        other = GTK_TEXT_VIEW(g_object_ref_sink(gtk_text_view_new_with_buffer(second)));
        g_object_unref(second);
        if (strcmp(mode, "shared-buffer") == 0)
        {
            CHECK(UmiGtk4TextFoldCount(other) == 1U);
            CHECK(UmiGtk4TextFoldClear(other) == UMI_STATUS_OK && UmiGtk4TextFoldCount(view) == 0U);
        }
        else
        {
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(other), source, -1);
            CHECK(UmiGtk4TextFoldCount(other) == 0U && UmiGtk4TextFoldCount(view) == 1U);
            CHECK(UmiGtk4TextFoldClear(other) == UMI_STATUS_OK && UmiGtk4TextFoldCount(view) == 1U);
        }
    }
    if (strcmp(mode, "other-tags") == 0)
    {
        GtkTextTag *tag = gtk_text_buffer_create_tag(buffer, "annotation", "weight", 700, NULL);
        GtkTextIter finish;
        gtk_text_buffer_get_bounds(buffer, &at, &finish);
        gtk_text_buffer_apply_tag(buffer, tag, &at, &finish);
        CHECK(UmiGtk4TextFoldClear(view) == UMI_STATUS_OK);
        gtk_text_buffer_get_start_iter(buffer, &at);
        CHECK(gtk_text_iter_has_tag(&at, tag));
    }
    if (strcmp(mode, "buffer-switch") == 0)
    {
        GtkTextBuffer *replacement = gtk_text_buffer_new(NULL);
        gtk_text_view_set_buffer(view, replacement);
        g_object_unref(replacement);
        CHECK(UmiGtk4TextFoldCount(view) == 0U);
        gtk_text_view_set_buffer(view, buffer);
        CHECK(UmiGtk4TextFoldCount(view) == 1U);
    }
    if (strcmp(mode, "tag-remove-edit") == 0)
    {
        hook_owner = G_OBJECT(gtk_text_buffer_get_tag_table(buffer));
        hook = g_signal_connect(hook_owner, "tag-removed", G_CALLBACK(Removed), &observer);
        CHECK(UmiGtk4TextFoldClear(view) == UMI_STATUS_OK);
        g_signal_handler_disconnect(hook_owner, hook);
        hook = 0U;
        CHECK(observer.fired && UmiGtk4TextFoldCount(view) == 0U);
        visible = Read(buffer, 0);
        CHECK(strcmp(visible, "host replacement") == 0);
    }
cleanup:
    if (hook != 0U)
        g_signal_handler_disconnect(hook_owner, hook);
    g_free(visible);
    g_free(complete);
    if (many != NULL)
        g_string_free(many, TRUE);
    g_clear_object(&other);
    g_clear_object(&view);
    g_clear_object(&buffer);
    return failed;
}
