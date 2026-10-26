/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_coverage_gtk4.c
 *
 * PURPOSE:
 *   Exercise native coverage admission, replacement, cancellation and owner retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../build_log/fixture.h"
#undef CHECK
#include "fixture.h"
#include "../support/main_context.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/ctest_coverage.h"

static GtkWidget *Find(GtkWidget *root, const char *tag)
{
    return umi_gtk4_automation_find_tagged_widget(root, tag);
}
static void Click(GtkWidget *root, const char *tag)
{
    GtkWidget *button = Find(root, tag);
    CHECK(GTK_IS_BUTTON(button));
    g_signal_emit_by_name(button, "clicked");
}
static void Drain(void)
{
    gint64 deadline = g_get_monotonic_time() + 50 * G_TIME_SPAN_MILLISECOND;
    while (g_get_monotonic_time() < deadline) {
        (void)UmiTestDispatchSlice(NULL, deadline, 32U);
        g_usleep(1000U);
    }
}
static void Wait(GtkWidget *root)
{
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (UmiGtk4CtestCoveragePending(root)) {
        CHECK(g_get_monotonic_time() < deadline);
        (void)UmiTestDispatchSlice(NULL, deadline, 32U);
        g_usleep(1000U);
    }
}
static char *Text(GtkTextBuffer *buffer)
{
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
}
static void Write(const char *path, const char *text)
{
    UmiOutputFile *file = NULL;
    OK(UmiOutputFileCreate(path, &file));
    OK(UmiOutputFileWrite(file, text, strlen(text)));
    OK(UmiOutputFileClose(file));
    UmiOutputFileDestroy(file);
}
static void CloseOnText(GtkTextBuffer *buffer, GtkWindow *window)
{
    (void)buffer;
    gtk_window_destroy(window);
}
/* Cancellation retires only pending I/O. A synchronous observer of accepted
 * text may hide the panel, but cannot leave accepted text with stale metadata. */
static void CancelOnText(GtkTextBuffer *buffer, GtkWidget *root)
{
    (void)buffer;
    Click(root, "ctest.coverage.cancel");
}
static void HideOnText(GtkTextBuffer *buffer, GtkWidget *root)
{
    (void)buffer;
    gtk_widget_set_visible(root, FALSE);
}
static void OpenOnText(GtkTextBuffer *buffer, GtkWidget *root)
{
    (void)buffer;
    Click(root, "ctest.coverage.open");
}
int main(int argc, char **argv)
{
    const char *cases[] = {"normal", "paging", "captured-path", "captured-identity",
        "cancel-load", "hide-show", "close-load", "hidden", "retained", "replacement",
        "invalid-identity", "relative", "clear", "close-render", "reentrant", "empty", "stale",
        "cancel-replacement", "cancel-render", "hide-render", "hide-clear", "cancel-page"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *test = argv[1];
    if (!gtk_init_check()) return 77;
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], report[UMI_PATH_CAPACITY],
        missing[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(path, directory, "coverage.csv");
    FixturePath(report, directory, "report.xml");
    FixturePath(missing, directory, "missing.csv");
    Write(report, "<testsuite tests='1'><testcase name='notes.pass' status='run' time='1'/></testsuite>");
    GString *plan = g_string_new("kind,host,configuration,source_identity,test,capability,report\n");
    if (strcmp(test, "empty")) {
        g_string_append(plan, "host,windows,Debug,source-A,,,report.xml\n");
        g_string_append(plan, "require,windows,Debug,,notes.pass,GTK display,\n");
        for (size_t i = 1; i < 130U; ++i)
            g_string_append_printf(plan, "require,windows,Debug,,missing.%03zu,GTK display,\n", i);
    }
    Write(path, plan->str);
    g_string_free(plan, TRUE);
    GtkApplication *application = gtk_application_new("org.umicom.CoverageReview", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    GtkWindow *window = GTK_WINDOW(gtk_application_window_new(application));
    g_object_ref(window);
    GtkWidget *root = UmiGtk4CtestCoverageCreate();
    g_object_ref_sink(root);
    gtk_window_set_child(window, root);
    gtk_window_present(window);
    Drain();
    GtkWidget *entry = Find(root, "ctest.coverage.path");
    GtkWidget *identity = Find(root, "ctest.coverage.identity");
    GtkWidget *open = Find(root, "ctest.coverage.open");
    CHECK(GTK_IS_EDITABLE(entry) && GTK_IS_EDITABLE(identity) && gtk_widget_get_mapped(open));
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(Find(root, "ctest.coverage.output"))));
    gtk_editable_set_text(GTK_EDITABLE(entry), path);
    gtk_editable_set_text(GTK_EDITABLE(identity), !strcmp(test, "stale") ? "source-B" : "source-A");
    if (!strcmp(test, "hidden")) {
        gtk_widget_set_visible(root, FALSE);
        Click(root, "ctest.coverage.open");
        CHECK(!UmiGtk4CtestCoveragePending(root));
        goto done;
    }
    if (!strcmp(test, "retained")) {
        g_object_ref(open);
        gtk_window_destroy(window);
        g_signal_emit_by_name(open, "clicked");
        CHECK(!UmiGtk4CtestCoveragePending(root));
        g_object_unref(open);
        goto done;
    }
    Click(root, "ctest.coverage.open");
    CHECK(UmiGtk4CtestCoveragePending(root));
    if (!strcmp(test, "captured-path")) gtk_editable_set_text(GTK_EDITABLE(entry), missing);
    if (!strcmp(test, "captured-identity")) gtk_editable_set_text(GTK_EDITABLE(identity), "later-source");
    if (!strcmp(test, "cancel-load")) Click(root, "ctest.coverage.cancel");
    if (!strcmp(test, "hide-show")) {
        gtk_widget_set_visible(root, FALSE);
        gtk_widget_set_visible(root, TRUE);
    }
    if (!strcmp(test, "close-load")) gtk_window_destroy(window);
    if (!strcmp(test, "close-render"))
        g_signal_connect_object(buffer, "changed", G_CALLBACK(CloseOnText), window, 0);
    if (!strcmp(test, "reentrant"))
        g_signal_connect_object(buffer, "changed", G_CALLBACK(OpenOnText), root, 0);
    if (!strcmp(test, "cancel-render"))
        g_signal_connect_object(buffer, "changed", G_CALLBACK(CancelOnText), root, 0);
    if (!strcmp(test, "hide-render"))
        g_signal_connect_object(buffer, "changed", G_CALLBACK(HideOnText), root, 0);
    Wait(root);
    char *accepted = Text(buffer);
    if (!strcmp(test, "cancel-load") || !strcmp(test, "hide-show") || !strcmp(test, "close-load")) {
        CHECK(accepted[0] == '\0');
        g_free(accepted);
        goto done;
    }
    if (!strcmp(test, "empty")) {
        CHECK(strstr(accepted, "No requirements: empty coverage cannot pass."));
    } else {
        CHECK(strstr(accepted, "Showing 1..64 of 130 required records."));
        if (!strcmp(test, "stale")) {
            CHECK(strstr(accepted, "Passed: 0 | Outstanding: 130"));
            CHECK(strstr(accepted, "[source mismatch]"));
        } else {
            CHECK(strstr(accepted, "Passed: 1 | Outstanding: 129"));
            CHECK(strstr(accepted, "[passed] windows / Debug : notes.pass"));
        }
    }
    if (!strcmp(test, "cancel-render") || !strcmp(test, "hide-render")) {
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.coverage.source"))), path));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.coverage.status"))),
            "Saved coverage loaded."));
        if (!strcmp(test, "hide-render")) CHECK(!gtk_widget_get_visible(root));
    }
    if (!strcmp(test, "cancel-page")) {
        g_signal_connect_object(buffer, "changed", G_CALLBACK(CancelOnText), root, 0);
        Click(root, "ctest.coverage.next");
        char *page = Text(buffer);
        CHECK(strstr(page, "Showing 65..128 of 130"));
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.coverage.source"))), path));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.coverage.status"))),
            "Saved coverage loaded."));
        g_free(page);
    }
    if (!strcmp(test, "hide-clear")) {
        g_signal_connect_object(buffer, "changed", G_CALLBACK(HideOnText), root, 0);
        Click(root, "ctest.coverage.clear");
        char *empty = Text(buffer);
        CHECK(!empty[0] && !gtk_widget_get_visible(root));
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.coverage.source"))),
            "No coverage loaded"));
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.coverage.status"))),
            "Saved review closed."));
        g_free(empty);
    }
    if (!strcmp(test, "captured-path"))
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.coverage.source"))), path));
    if (!strcmp(test, "captured-identity"))
        CHECK(strstr(accepted, "Requested source: source-A\n") && !strstr(accepted, "later-source"));
    if (!strcmp(test, "paging") || !strcmp(test, "replacement") ||
        !strcmp(test, "cancel-replacement")) {
        Click(root, "ctest.coverage.next");
        g_free(accepted); accepted = Text(buffer);
        CHECK(strstr(accepted, "Showing 65..128 of 130"));
        if (!strcmp(test, "paging")) {
            Click(root, "ctest.coverage.next");
            char *last = Text(buffer);
            CHECK(strstr(last, "Showing 129..130 of 130"));
            g_free(last);
            Click(root, "ctest.coverage.previous");
        }
    }
    if (!strcmp(test, "replacement")) {
        gtk_editable_set_text(GTK_EDITABLE(entry), missing);
        Click(root, "ctest.coverage.open"); Wait(root);
    }
    if (!strcmp(test, "cancel-replacement")) {
        gtk_editable_set_text(GTK_EDITABLE(identity), "source-B");
        Click(root, "ctest.coverage.open"); Click(root, "ctest.coverage.cancel"); Wait(root);
    }
    if (!strcmp(test, "invalid-identity") || !strcmp(test, "relative")) {
        if (!strcmp(test, "invalid-identity")) gtk_editable_set_text(GTK_EDITABLE(identity), "");
        else gtk_editable_set_text(GTK_EDITABLE(entry), "coverage.csv");
        Click(root, "ctest.coverage.open");
        CHECK(!UmiGtk4CtestCoveragePending(root));
    }
    if (!strcmp(test, "replacement") || !strcmp(test, "cancel-replacement") ||
        !strcmp(test, "invalid-identity") || !strcmp(test, "relative") || !strcmp(test, "paging")) {
        char *after = Text(buffer);
        CHECK(!strcmp(accepted, after));
        g_free(after);
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.coverage.source"))), path));
    }
    if (!strcmp(test, "clear")) {
        Click(root, "ctest.coverage.clear");
        char *empty = Text(buffer); CHECK(!empty[0]); g_free(empty);
    }
    g_free(accepted);
done:
    gtk_window_destroy(window);
    Wait(root);
    g_object_unref(buffer);
    g_object_unref(root);
    g_object_unref(window);
    g_object_unref(application);
    return 0;
}
