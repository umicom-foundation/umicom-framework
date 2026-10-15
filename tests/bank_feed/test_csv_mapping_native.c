/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_csv_mapping_native.c
 * PURPOSE: Exercise user mappings and stale-result retirement through native report controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/bank_feed.h"
#include <glib/gstdio.h>
static GtkWidget *Find(GtkWindow *window, const char *name)
{
    return umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
}
static void Set(GtkWindow *window, const char *name, const char *value)
{
    gtk_editable_set_text(GTK_EDITABLE(Find(window, name)), value);
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
/* These edits occur synchronously inside GTK notifications. They exercise
 * reentrant request preparation without depending on worker scheduling. */
static void Prepared(GObject *object, GParamSpec *property, gpointer entry)
{
    (void)property;
    if (strstr(gtk_label_get_text(GTK_LABEL(object)), "Reading selected file") != NULL)
        gtk_editable_set_text(GTK_EDITABLE(entry), "different-account");
}
static void Invalidated(GtkTextBuffer *buffer, gpointer entry)
{
    (void)buffer;
    gtk_editable_set_text(GTK_EDITABLE(entry), "different-account");
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    GtkWindow *window = UmiBankFeedGtk4Create(NULL);
    CHECK(window != NULL);
    g_object_ref(window);
    GtkWidget *format = Find(window, "bank-feed-format"), *load = Find(window, "bank-feed-load"),
              *save = Find(window, "bank-feed-export"), *view = Find(window, "bank-feed-output"),
              *account = Find(window, "bank-map-account-value"), *note = Find(window, "bank-feed-note");
    CHECK(format && load && save && view && account && note);
    g_object_ref(account); g_object_ref(load); g_object_ref(save); g_object_ref(view);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 2U);
    const char *clear[] = {"bank-map-account", "bank-map-currency", "bank-map-reference",
                          "bank-map-status", "bank-map-mutability"};
    for (size_t i = 0U; i < G_N_ELEMENTS(clear); ++i) Set(window, clear[i], "");
    Set(window, "bank-map-account-value", "practice-account");
    Set(window, "bank-map-currency-value", "GBP");
    Set(window, "bank-map-transaction", "ID");
    Set(window, "bank-map-date", "Date");
    Set(window, "bank-map-amount", "Amount");
    Set(window, "bank-map-information", "Description");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(window, "bank-map-date-order")), 1U);
    if (strcmp(mode, "pending") != 0)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(window, "bank-map-booked")), 1U);
    if (!strcmp(mode, "retained")) {
        gtk_window_destroy(window);
        gtk_editable_set_text(GTK_EDITABLE(account), "closed");
        g_signal_emit_by_name(load, "clicked");
        CHECK(!UmiBankFeedGtk4Busy(window) && !gtk_widget_get_sensitive(save));
    } else {
        GError *error = NULL;
        char *directory = g_dir_make_tmp("umicom-mapped-csv-XXXXXX", &error);
        CHECK(directory != NULL && error == NULL);
        char *filename = g_build_filename(directory, "statement.csv", NULL);
        const char *contents = "ID,Date,Amount,Description\n"
                               "one,12/10/2026,12.50,credit\ntwo,12/10/2026,-2.25,debit\n";
        if (!strcmp(mode, "separate")) {
            contents = "ID,Date,Debit,Credit,Description\n"
                       "one,12/10/2026,,12.50,credit\ntwo,12/10/2026,2.25,0,debit\n";
            Set(window, "bank-map-amount", "");
            Set(window, "bank-map-debit", "Debit");
            Set(window, "bank-map-credit", "Credit");
            gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(window, "bank-map-amount-mode")), 1U);
        }
        if (!strcmp(mode, "invalid")) Set(window, "bank-map-date", "Wrong");
        if (!strcmp(mode, "oversized")) {
            char longName[300]; memset(longName, 'x', sizeof longName - 1U);
            longName[sizeof longName - 1U] = '\0';
            Set(window, "bank-map-date", longName);
        }
        CHECK(g_file_set_contents(filename, contents, -1, &error));
        Set(window, "bank-feed-path", filename);
        gulong signal = 0U;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
        if (!strcmp(mode, "prepare-change"))
            signal = g_signal_connect(note, "notify::label", G_CALLBACK(Prepared), account);
        if (!strcmp(mode, "invalidation-change")) {
            gtk_text_buffer_set_text(buffer, "Old report", -1);
            signal = g_signal_connect(buffer, "changed", G_CALLBACK(Invalidated), account);
        }
        g_signal_emit_by_name(load, "clicked");
        if (!strcmp(mode, "prepare-change") || !strcmp(mode, "invalidation-change") ||
            !strcmp(mode, "oversized")) CHECK(!UmiBankFeedGtk4Busy(window));
        if (!strcmp(mode, "change-loading"))
            gtk_editable_set_text(GTK_EDITABLE(account), "different-account");
        if (!strcmp(mode, "close-loading")) gtk_window_destroy(window);
        CHECK(Wait(window));
        char *text = Text(view);
        if (!strcmp(mode, "close-loading")) {
            /* The worker can finish, but no result may be published into a
             * retained control after its native window has been removed. */
            CHECK(text[0] == '\0' && !gtk_widget_get_sensitive(save));
        } else if (!strcmp(mode, "invalid") || !strcmp(mode, "oversized") ||
                   !strcmp(mode, "prepare-change") || !strcmp(mode, "invalidation-change") ||
                   !strcmp(mode, "change-loading")) {
            CHECK(text[0] == '\0' && !gtk_widget_get_sensitive(save));
            if (!strcmp(mode, "invalid"))
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(note)), "CSV record 1") != NULL);
            if (!strcmp(mode, "oversized"))
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(note)), "too long") != NULL);
        } else {
            CHECK(strstr(text, !strcmp(mode, "pending") ? "Pending net movement: 10.25"
                                                        : "Booked net movement: 10.25") != NULL);
            CHECK(gtk_widget_get_sensitive(save));
            if (!strcmp(mode, "change-after-import")) {
                gtk_editable_set_text(GTK_EDITABLE(account), "different-account");
                g_free(text); text = Text(view);
                CHECK(text[0] == '\0' && !gtk_widget_get_sensitive(save));
            } else CHECK(!strcmp(mode, "import") || !strcmp(mode, "pending") || !strcmp(mode, "separate"));
        }
        if (signal != 0U)
            g_signal_handler_disconnect(!strcmp(mode, "invalidation-change") ? G_OBJECT(buffer) : G_OBJECT(note), signal);
        g_free(text);
        CHECK(g_remove(filename) == 0 && g_rmdir(directory) == 0);
        g_free(filename); g_free(directory); g_clear_error(&error);
    }
    gtk_window_destroy(window);
    g_object_unref(account); g_object_unref(load); g_object_unref(save); g_object_unref(view);
    g_object_unref(window);
    return 0;
}
