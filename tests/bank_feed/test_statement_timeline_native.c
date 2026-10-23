/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_statement_timeline_native.c
 * PURPOSE: Check daily review publication, edit invalidation and retained native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/bank_statement.h"
static GtkWidget *Find(GtkWindow *window, const char *name) {
    return umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
}
static char *Text(GtkTextBuffer *buffer) {
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
}
typedef struct DailyProbe {
    GtkWindow *window;
    GtkWidget *closing;
    bool close;
    unsigned calls;
} DailyProbe;
/* Act once while results are being published, rather than after comparison
 * returns. A retained widget must not expose the interrupted generation. */
static void Interrupt(DailyProbe *probe) {
    if (probe->calls++) return;
    if (probe->close) gtk_window_destroy(probe->window);
    else gtk_editable_set_text(GTK_EDITABLE(probe->closing), "999");
}
static void BufferChanged(GtkTextBuffer *buffer, gpointer data) {
    char *text = Text(buffer);
    const bool published = strstr(text, "Daily booked balances") != NULL;
    g_free(text);
    if (published) Interrupt(data);
}
static void ExportEnabled(GObject *object, GParamSpec *spec, gpointer data) {
    (void)spec;
    if (gtk_widget_get_sensitive(GTK_WIDGET(object))) Interrupt(data);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    char *document = BankFixture(BANK_TEST_ROW);
    if (!strcmp(mode, "aggregate-only")) {
        free(document);
        document = BankFixture(
            "{\"AccountId\":\"practice-account\",\"TransactionId\":\"credit\","
            "\"BookingDateTime\":\"2026-10-12T00:00:00Z\",\"Amount\":{\"Amount\":\"1\",\"Currency\":\"GBP\"},"
            "\"CreditDebitIndicator\":\"Credit\",\"Status\":\"BOOK\"},"
            "{\"AccountId\":\"practice-account\",\"TransactionId\":\"debit\","
            "\"BookingDateTime\":\"2026-10-13T00:00:00Z\",\"Amount\":{\"Amount\":\"1\",\"Currency\":\"GBP\"},"
            "\"CreditDebitIndicator\":\"Debit\",\"Status\":\"BOOK\"}");
    }
    CHECK(document != NULL);
    UmiBankFeed *feed = NULL;
    CHECK(UmiBankFeedCreate(document, strlen(document), &feed) == UMI_STATUS_OK);
    free(document);
    GtkWindow *parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    GtkWindow *window = UmiBankStatementGtk4Create(feed, "practice-account", "GBP", parent);
    CHECK(window != NULL);
    g_object_ref(window);
    UmiBankFeedDestroy(feed);
    GtkWidget *first = Find(window, "statement-first"), *last = Find(window, "statement-last"),
        *opening = Find(window, "statement-opening"), *closing = Find(window, "statement-closing"),
        *compare = Find(window, "statement-compare"), *save = Find(window, "statement-export"),
        *dailySave = Find(window, "statement-daily-export"), *view = Find(window, "statement-daily-view");
    CHECK(first && last && opening && closing && compare && save && dailySave && GTK_IS_TEXT_VIEW(view));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    g_object_ref(buffer); g_object_ref(compare); g_object_ref(save); g_object_ref(dailySave);
    CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(view)));
    CHECK(!gtk_widget_get_sensitive(dailySave));
    gtk_editable_set_text(GTK_EDITABLE(first), "2026-10-12");
    gtk_editable_set_text(GTK_EDITABLE(last), "2026-10-12");
    gtk_editable_set_text(GTK_EDITABLE(opening), "100.00");
    gtk_editable_set_text(GTK_EDITABLE(closing), "112.50");
    if (!strcmp(mode, "aggregate-only")) {
        gtk_editable_set_text(GTK_EDITABLE(last), "2026-10-13");
        gtk_editable_set_text(GTK_EDITABLE(opening), "9223372036854775807");
        gtk_editable_set_text(GTK_EDITABLE(closing), "9223372036854775807");
    }
    bool bufferSignal = !strcmp(mode, "buffer-edit") || !strcmp(mode, "buffer-close");
    bool exportSignal = !strcmp(mode, "export-edit") || !strcmp(mode, "export-close");
    DailyProbe probe = {window, closing, !strcmp(mode, "buffer-close") || !strcmp(mode, "export-close"), 0U};
    gulong handler = 0U;
    if (bufferSignal) handler = g_signal_connect(buffer, "changed", G_CALLBACK(BufferChanged), &probe);
    if (exportSignal) handler = g_signal_connect(dailySave, "notify::sensitive", G_CALLBACK(ExportEnabled), &probe);
    if (!strcmp(mode, "retained") || !strcmp(mode, "parent")) {
        if (!strcmp(mode, "parent")) gtk_window_destroy(parent);
        else gtk_window_destroy(window);
        g_signal_emit_by_name(compare, "clicked");
        CHECK(!gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(dailySave));
    } else {
        if (!strcmp(mode, "invalid")) gtk_editable_set_text(GTK_EDITABLE(first), "2026-02-30");
        g_signal_emit_by_name(compare, "clicked");
        char *text = Text(buffer);
        if (bufferSignal || exportSignal) {
            CHECK(probe.calls > 0U);
            CHECK(!gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(dailySave));
            if (!probe.close) CHECK(text[0] == '\0');
        } else if (!strcmp(mode, "aggregate-only")) {
            CHECK(gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(dailySave));
            CHECK(strstr(text, "Daily trace unavailable") && strstr(text, "No partial balances"));
        } else if (!strcmp(mode, "invalid")) {
            CHECK(!gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(dailySave));
            CHECK(strstr(text, "Daily trace unavailable"));
        } else {
            CHECK(gtk_widget_get_sensitive(save) && gtk_widget_get_sensitive(dailySave));
            CHECK(strstr(text, "2026-10-12") && strstr(text, "closing 112.50"));
            CHECK(strstr(text, "Minimum including supplied opening: 100.00"));
            if (!strcmp(mode, "edit") || !strcmp(mode, "recompare")) {
                gtk_editable_set_text(GTK_EDITABLE(closing), "112.51");
                CHECK(!gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(dailySave));
                char *cleared = Text(buffer); CHECK(cleared[0] == '\0'); g_free(cleared);
                if (!strcmp(mode, "recompare")) {
                    g_signal_emit_by_name(compare, "clicked");
                    CHECK(gtk_widget_get_sensitive(save) && gtk_widget_get_sensitive(dailySave));
                    char *fresh = Text(buffer); CHECK(strstr(fresh, "closing 112.50")); g_free(fresh);
                }
            } else CHECK(!strcmp(mode, "snapshot"));
        }
        g_free(text);
    }
    if (handler != 0U) g_signal_handler_disconnect(bufferSignal ? G_OBJECT(buffer) : G_OBJECT(dailySave), handler);
    gtk_window_destroy(parent); gtk_window_destroy(window);
    g_signal_emit_by_name(compare, "clicked");
    CHECK(!gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(dailySave));
    g_object_unref(buffer); g_object_unref(compare); g_object_unref(save); g_object_unref(dailySave);
    g_object_unref(window); g_object_unref(parent);
    return 0;
}
