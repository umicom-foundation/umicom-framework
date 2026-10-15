/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_csv_native.c
 * PURPOSE: Check explicit CSV dispatch and stale-result retirement through native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "csv_fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/bank_feed.h"
#include <glib/gstdio.h>
static GtkWidget *Find(GtkWindow *window, const char *name)
{
    return umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
}
static bool Wait(GtkWindow *window)
{
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while (UmiBankFeedGtk4Busy(window) && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return !UmiBankFeedGtk4Busy(window);
}
static char *Text(GtkWidget *view)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
}
/* A setting can change from an application's notification handler before the
 * worker starts. Exercise that ownership boundary without relying on timing. */
static void ChangePreparedFormat(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    if (strstr(gtk_label_get_text(GTK_LABEL(object)), "Reading selected file") != NULL)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(data), 0U);
}
static int Reconcile(GtkWindow *window)
{
    g_signal_emit_by_name(Find(window, "bank-feed-reconcile"), "clicked");
    GtkWindow *review = NULL;
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *candidate = g_list_model_get_item(windows, i);
        if (gtk_window_get_transient_for(candidate) == window &&
            Find(candidate, "statement-compare") != NULL) {
            review = candidate;
            break;
        }
        g_object_unref(candidate);
    }
    CHECK(review != NULL);
    gtk_editable_set_text(GTK_EDITABLE(Find(review, "statement-first")), "2026-10-12");
    gtk_editable_set_text(GTK_EDITABLE(Find(review, "statement-last")), "2026-10-12");
    gtk_editable_set_text(GTK_EDITABLE(Find(review, "statement-opening")), "100.00");
    gtk_editable_set_text(GTK_EDITABLE(Find(review, "statement-closing")), "110.25");
    g_signal_emit_by_name(Find(review, "statement-compare"), "clicked");
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(review, "statement-result"))),
                 "Arithmetic matches") != NULL);
    gtk_window_destroy(review);
    g_object_unref(review);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    GtkWindow *window = UmiBankFeedGtk4Create(NULL);
    CHECK(window != NULL);
    g_object_ref(window);
    GtkWidget *path = Find(window, "bank-feed-path"), *format = Find(window, "bank-feed-format"),
              *load = Find(window, "bank-feed-load"), *save = Find(window, "bank-feed-export"),
              *view = Find(window, "bank-feed-output"), *note = Find(window, "bank-feed-note");
    CHECK(path && format && load && save && view && note);
    g_object_ref(format); g_object_ref(load); g_object_ref(save); g_object_ref(view);
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(format)) == 0U);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 1U);
    if (!strcmp(mode, "retained")) {
        gtk_window_destroy(window);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 0U);
        g_signal_emit_by_name(load, "clicked");
        CHECK(!UmiBankFeedGtk4Busy(window) && !gtk_widget_get_sensitive(save));
    } else {
        GError *error = NULL;
        char *directory = g_dir_make_tmp("umicom-statement-csv-XXXXXX", &error);
        CHECK(directory != NULL && error == NULL);
        /* A misleading extension proves dispatch follows the user's explicit
         * choice rather than a filename heuristic. */
        char *filename = g_build_filename(directory, "statement.json", NULL);
        const char *contents = !strcmp(mode, "invalid") ?
            BANK_CSV_HEADER "a,id,,,2026-02-30,GBP,1,BOOK,Immutable\n" : BANK_CSV_TEXT;
        CHECK(g_file_set_contents(filename, contents, -1, &error));
        gtk_editable_set_text(GTK_EDITABLE(path), filename);
        gulong signal = 0U;
        if (!strcmp(mode, "prepare-format"))
            signal = g_signal_connect(note, "notify::label", G_CALLBACK(ChangePreparedFormat), format);
        g_signal_emit_by_name(load, "clicked");
        if (!strcmp(mode, "prepare-format")) CHECK(!UmiBankFeedGtk4Busy(window));
        if (!strcmp(mode, "loading-change"))
            gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 0U);
        CHECK(Wait(window));
        char *text = Text(view);
        if (!strcmp(mode, "invalid")) {
            CHECK(!gtk_widget_get_sensitive(save));
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(note)), "CSV record 2, column 5") != NULL);
        } else if (!strcmp(mode, "loading-change") || !strcmp(mode, "prepare-format")) {
            CHECK(text[0] == '\0' && !gtk_widget_get_sensitive(save));
        } else {
            CHECK(strstr(text, "Booked net movement: 10.25") != NULL);
            CHECK(gtk_widget_get_sensitive(save));
            if (!strcmp(mode, "format-change")) {
                gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 0U);
                g_free(text); text = Text(view);
                CHECK(text[0] == '\0' && !gtk_widget_get_sensitive(save));
            } else if (!strcmp(mode, "reconcile")) CHECK(Reconcile(window) == 0);
            else CHECK(!strcmp(mode, "import"));
        }
        if (signal != 0U) g_signal_handler_disconnect(note, signal);
        g_free(text);
        CHECK(g_remove(filename) == 0 && g_rmdir(directory) == 0);
        g_free(filename); g_free(directory); g_clear_error(&error);
    }
    gtk_window_destroy(window);
    g_object_unref(format); g_object_unref(load); g_object_unref(save); g_object_unref(view);
    g_object_unref(window);
    return 0;
}
