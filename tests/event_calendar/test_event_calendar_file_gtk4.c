/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/event_calendar/test_event_calendar_file_gtk4.c
 *
 * PURPOSE:
 *   Verify asynchronous calendar imports retain prior evidence across cancellation and closure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "calendar_fixture.h"
#include "umicom/trading_ui/gtk4/event_calendar.h"
#include "umicom/ui/gtk4/automation.h"
#include <glib/gstdio.h>

static GtkWidget *Find(GtkWidget *panel, const char *name) {
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(panel, name);
    CHECK(widget != NULL);
    return widget;
}
static size_t Rows(GtkWidget *panel) {
    GtkWidget *list = Find(panel, "trading.calendar.rows");
    size_t count = 0U;
    for (GtkWidget *row = gtk_widget_get_first_child(list); row != NULL;
         row = gtk_widget_get_next_sibling(row))
        ++count;
    return count;
}
/* The worker owns the only remaining file reference. Waiting for its release
 * observes task cleanup, including a cancelled or retired completion, rather
 * than guessing that a fixed delay was sufficient for the worker to finish. */
static void Finish(GWeakRef *remaining) {
    gint64 deadline = g_get_monotonic_time() + 10000000;
    for (;;) {
        GObject *file = g_weak_ref_get(remaining);
        if (file == NULL)
            break;
        g_object_unref(file);
        CHECK(g_get_monotonic_time() < deadline);
        while (g_main_context_iteration(NULL, FALSE)) {
        }
        g_usleep(1000U);
    }
    g_weak_ref_clear(remaining);
}
int main(int argc, char **argv) {
    CHECK(argc == 3);
    if (!gtk_init_check())
        return 77;
    const char *test = argv[1];
    CHECK(g_mkdir_with_parents(argv[2], 0700) == 0);
    char *directory = g_build_filename(argv[2], "calendar-XXXXXX", NULL);
    CHECK(g_mkdtemp(directory) != NULL);
    char *filename = g_build_filename(directory, "events.csv", NULL);
    GtkWidget *panel = UmiGtk4EventCalendarCreate();
    CHECK(panel != NULL);
    g_object_ref_sink(panel);
    CHECK(UmiGtk4EventCalendarLoadCsv(panel, CALENDAR_CSV, strlen(CALENDAR_CSV)) == UMI_STATUS_OK);
    CHECK(Rows(panel) == 2U);
    const char *input = strcmp(test, "invalid") == 0 ? "invalid" : CALENDAR_HEADER ECONOMIC_ROW;
    if (strcmp(test, "missing") != 0) {
        if (strcmp(test, "limit") == 0) {
            gsize size = (gsize)UMI_EVENT_CALENDAR_MAX_BYTES + 1U;
            char *large = g_malloc(size);
            memset(large, 'x', size);
            CHECK(g_file_set_contents(filename, large, (gssize)size, NULL));
            g_free(large);
        } else
            CHECK(g_file_set_contents(filename, input, -1, NULL));
    }
    if (strcmp(test, "nonlocal") == 0) {
        GFile *remote = g_file_new_for_uri("https://invalid.example/calendar.csv");
        CHECK(UmiGtk4EventCalendarReadFile(panel, remote) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(!UmiGtk4EventCalendarIsReading(panel) && Rows(panel) == 2U);
        g_object_unref(remote);
    } else {
        GFile *file = g_file_new_for_path(filename);
        GWeakRef remaining;
        g_weak_ref_init(&remaining, G_OBJECT(file));
        CHECK(UmiGtk4EventCalendarReadFile(panel, file) == UMI_STATUS_OK);
        CHECK(UmiGtk4EventCalendarIsReading(panel));
        CHECK(UmiGtk4EventCalendarReadFile(panel, file) == UMI_STATUS_BUSY);
        CHECK(UmiGtk4EventCalendarLoadCsv(panel, CALENDAR_CSV, strlen(CALENDAR_CSV)) ==
              UMI_STATUS_BUSY);
        g_object_unref(file);
        if (strcmp(test, "cancel") == 0) {
            g_signal_emit_by_name(Find(panel, "trading.calendar.cancel"), "clicked");
            CHECK(!UmiGtk4EventCalendarIsReading(panel));
            CHECK(UmiGtk4EventCalendarLoadCsv(panel, CALENDAR_CSV, strlen(CALENDAR_CSV)) ==
                  UMI_STATUS_OK);
        } else if (strcmp(test, "detached") == 0) {
            UmiGtk4EventCalendarDetach(panel);
            CHECK(!UmiGtk4EventCalendarIsReading(panel) && !gtk_widget_is_sensitive(panel));
        } else if (strcmp(test, "destroyed") == 0) {
            gpointer retained = panel;
            g_object_add_weak_pointer(G_OBJECT(panel), &retained);
            g_object_unref(panel);
            panel = NULL;
            CHECK(retained == NULL);
        }
        Finish(&remaining);
        if (panel != NULL) {
            CHECK(!UmiGtk4EventCalendarIsReading(panel));
            if (strcmp(test, "valid") == 0)
                CHECK(Rows(panel) == 1U);
            else {
                CHECK(strcmp(test, "invalid") == 0 || strcmp(test, "missing") == 0 ||
                      strcmp(test, "limit") == 0 || strcmp(test, "cancel") == 0 ||
                      strcmp(test, "detached") == 0);
                CHECK(Rows(panel) == 2U);
            }
            if (strcmp(test, "invalid") == 0 || strcmp(test, "missing") == 0 ||
                strcmp(test, "limit") == 0)
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(panel, "trading.calendar.message"))),
                             "retained") != NULL);
        }
    }
    if (panel != NULL)
        g_object_unref(panel);
    if (g_file_test(filename, G_FILE_TEST_EXISTS))
        CHECK(g_remove(filename) == 0);
    CHECK(g_rmdir(directory) == 0);
    g_free(filename);
    g_free(directory);
    return 0;
}
