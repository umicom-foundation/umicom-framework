/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/text_export/test_retirement.c
 *
 * PURPOSE:
 *   Verify retired controls refuse capture after reparenting and notification-time release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/text_export.h"
#include "umicom/ui/gtk4/automation.h"
#include <glib/gstdio.h>
#include <stdio.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

typedef struct RetirementObserver {
    GtkWidget *root;
    unsigned calls;
    unsigned admissions;
} RetirementObserver;

static UmiStatus Admit(void *data)
{
    RetirementObserver *observer = data;
    ++observer->admissions;
    return UMI_STATUS_OK;
}

static void ReleaseOnDisable(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    RetirementObserver *observer = data;
    if (gtk_widget_get_sensitive(GTK_WIDGET(object))) return;
    ++observer->calls;
    /* Retirement must be idempotent even during its own notification. A direct
     * signal emission also exercises the disconnected capture callback. */
    UmiTextExportGtk4Retire(observer->root);
    g_signal_emit_by_name(object, "clicked");
    g_clear_object(&observer->root);
}

int main(void)
{
    if (!gtk_init_check()) return 77;
    char *directory = g_dir_make_tmp("umicom-export-retirement-XXXXXX", NULL);
    CHECK(directory != NULL);
    char *filename = g_build_filename(directory, "retired.txt", NULL);
    CHECK(filename != NULL);
    for (unsigned mode = 0U; mode < 3U; ++mode) {
        RetirementObserver observer = {0};
        GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);
        gtk_text_buffer_set_text(buffer, "A captured report", -1);
        observer.root = UmiTextExportGtk4CreateGuardedBuffer(buffer, "report.txt",
            Admit, &observer, NULL);
        CHECK(observer.root != NULL);
        g_object_ref_sink(observer.root);
        CHECK(GTK_IS_EXPANDER(observer.root));
        gtk_expander_set_expanded(GTK_EXPANDER(observer.root), TRUE);
        GtkWidget *save = umi_gtk4_automation_find_tagged_widget(observer.root, "text.export.save");
        GtkWidget *entry = umi_gtk4_automation_find_tagged_widget(observer.root, "text.export.path");
        CHECK(GTK_IS_BUTTON(save) && GTK_IS_EDITABLE(entry));
        g_object_ref(save); g_object_ref(entry);
        gtk_editable_set_text(GTK_EDITABLE(entry), filename);
        GtkWidget *destination = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        g_object_ref_sink(destination);
        if (mode == 1U) {
            /* Reparent before retirement: the old container cannot supply the
             * effective disabled state after the controls move elsewhere. */
            GtkWidget *parent = gtk_widget_get_parent(save);
            CHECK(GTK_IS_BOX(parent));
            gtk_box_remove(GTK_BOX(parent), save);
            parent = gtk_widget_get_parent(entry);
            CHECK(GTK_IS_BOX(parent));
            gtk_box_remove(GTK_BOX(parent), entry);
            gtk_box_append(GTK_BOX(destination), save);
            gtk_box_append(GTK_BOX(destination), entry);
        }
        if (mode == 2U)
            g_signal_connect(save, "notify::sensitive", G_CALLBACK(ReleaseOnDisable), &observer);
        UmiTextExportGtk4Retire(observer.root);
        CHECK(!gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(entry));
        CHECK(!gtk_widget_is_sensitive(save) && !gtk_widget_is_sensitive(entry));
        g_signal_emit_by_name(save, "clicked");
        CHECK(observer.admissions == 0U);
        CHECK(!g_file_test(filename, G_FILE_TEST_EXISTS));
        if (mode == 2U) CHECK(observer.calls == 1U && observer.root == NULL);
        else {
            UmiTextExportGtk4Retire(observer.root);
            CHECK(!UmiTextExportGtk4Busy(observer.root));
        }
        g_signal_handlers_disconnect_by_data(save, &observer);
        g_clear_object(&observer.root);
        g_object_unref(destination);
        g_object_unref(save); g_object_unref(entry); g_object_unref(buffer);
    }
    CHECK(g_rmdir(directory) == 0);
    g_free(filename); g_free(directory);
    return 0;
}
