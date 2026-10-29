/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_coverage_filter_gtk4.c
 *
 * PURPOSE:
 *   Exercise native coverage filtering, paging and immutable accepted evidence.
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
static GtkWidget *Find(GtkWidget *root, const char *tag) {
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, tag); CHECK(widget); return widget;
}
static void Click(GtkWidget *root, const char *tag) {
    GtkWidget *button = Find(root, tag); CHECK(GTK_IS_BUTTON(button)); g_signal_emit_by_name(button, "clicked");
}
static void Entry(GtkWidget *root, const char *tag, const char *value) {
    gtk_editable_set_text(GTK_EDITABLE(Find(root, tag)), value);
}
static void Write(const char *path, const char *text) {
    UmiOutputFile *file = NULL; OK(UmiOutputFileCreate(path, &file));
    OK(UmiOutputFileWrite(file, text, strlen(text))); OK(UmiOutputFileClose(file)); UmiOutputFileDestroy(file);
}
static void Drain(void) {
    gint64 deadline = g_get_monotonic_time() + 80 * G_TIME_SPAN_MILLISECOND;
    while (g_get_monotonic_time() < deadline) { (void)UmiTestDispatchSlice(NULL, deadline, 32U); g_usleep(1000U); }
}
static void Wait(GtkWidget *root) {
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (UmiGtk4CtestCoveragePending(root)) {
        CHECK(g_get_monotonic_time() < deadline); (void)UmiTestDispatchSlice(NULL, deadline, 32U); g_usleep(1000U);
    }
}
static char *Text(GtkTextBuffer *buffer) {
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
}
typedef struct Probe { GtkWidget *root; GtkWindow *window; const char *mode; unsigned calls; } Probe;
static void Changed(GtkTextBuffer *buffer, gpointer data) {
    (void)buffer; Probe *probe = data;
    /* Text replacement can notify for deletion and insertion. Count the guarded
     * observer action once; retain the old signal-count assumption for review. */
#if 0
    if (probe->calls++ != 0U) return;
#endif
    if (probe->calls != 0U) return;
    ++probe->calls;
    if (!strcmp(probe->mode, "close-render")) gtk_window_destroy(probe->window);
    else if (!strcmp(probe->mode, "clear-render")) Click(probe->root, "ctest.coverage.clear");
    else Entry(probe->root, "ctest.coverage.filter.text", "notes.pass");
}
int main(int argc, char **argv) {
    const char *cases[] = {"passed", "outstanding", "host", "configuration", "text", "capability", "empty",
        "paging", "edit-render", "close-render", "clear-render", "reload", "failed-reload", "retained", "oversize", "messages"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    char directory[UMI_PATH_CAPACITY], manifest[UMI_PATH_CAPACITY], report[UMI_PATH_CAPACITY], missing[UMI_PATH_CAPACITY];
    FixtureDirectory(directory); FixturePath(manifest, directory, "coverage.csv");
    FixturePath(report, directory, "report.xml"); FixturePath(missing, directory, "missing.csv");
    /* Valid CTest observations require an explicit duration. Preserve the
     * incomplete draft fixture, which would fail admission before filtering. */
#if 0
    Write(report, "<testsuite tests='3' failures='1' skipped='1'>"
        "<testcase name='notes.pass' status='run'/><testcase name='notes.fail' status='fail'><failure message='Failed'/></testcase>"
        "<testcase name='notes.skip' status='notrun'><skipped message='SKIP_RETURN_CODE=77'/></testcase></testsuite>");
#endif
    Write(report, "<testsuite tests='3' failures='1' skipped='1'>"
        "<testcase name='notes.pass' status='run' time='1'/><testcase name='notes.fail' status='fail' time='1'><failure message='Failed'/></testcase>"
        "<testcase name='notes.skip' status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase></testsuite>");
    GString *plan = g_string_new("kind,host,configuration,source_identity,test,capability,report\n"
        "host,Windows,Debug,source-A,,,report.xml\n"
        "require,Windows,Debug,,notes.pass,GTK display,\n"
        "require,Windows,Debug,,notes.fail,GPU compute,\n"
        "require,Windows,Debug,,notes.skip,GTK display,\n"
        "require,Linux,Release,,notes.linux,POSIX,\n");
    for (unsigned i = 0U; i < 70U; ++i)
        g_string_append_printf(plan, "require,Windows,Debug,,missing.%03u,GTK display,\n", i);
    Write(manifest, plan->str); g_string_free(plan, TRUE);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref(window);
    GtkWidget *root = UmiGtk4CtestCoverageCreate(); g_object_ref_sink(root);
    gtk_window_set_default_size(window, 900, 900); gtk_window_set_child(window, root); gtk_window_present(window); Drain();
    Entry(root, "ctest.coverage.path", manifest); Entry(root, "ctest.coverage.identity", "source-A");
    Click(root, "ctest.coverage.open"); Wait(root);
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "ctest.coverage.output"))));
    char *before = Text(buffer); CHECK(strstr(before, "Whole review: 74 requirements | Passed: 1 | Outstanding: 73"));
    guint outcome = !strcmp(mode, "passed") ? 2U : !strcmp(mode, "outstanding") || !strcmp(mode, "paging") ? 1U : 0U;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "ctest.coverage.filter.outcome")), outcome);
    if (!strcmp(mode, "host")) Entry(root, "ctest.coverage.filter.host", "LINUX");
    if (!strcmp(mode, "configuration")) Entry(root, "ctest.coverage.filter.configuration", "release");
    if (!strcmp(mode, "capability")) Entry(root, "ctest.coverage.filter.text", "GPU");
    if (!strcmp(mode, "empty")) Entry(root, "ctest.coverage.filter.text", ".*");
    if (!strcmp(mode, "text") || !strcmp(mode, "edit-render") || !strcmp(mode, "close-render") ||
        !strcmp(mode, "clear-render") || !strcmp(mode, "reload") || !strcmp(mode, "failed-reload"))
        Entry(root, "ctest.coverage.filter.text", "notes.fail");
    if (!strcmp(mode, "oversize")) {
        char longText[257]; memset(longText, 'x', 256U); longText[256] = '\0';
        Entry(root, "ctest.coverage.filter.text", longText);
    }
    if (!strcmp(mode, "messages")) {
        CHECK(strstr(before, "Saved result message: SKIP_RETURN_CODE=77"));
        Entry(root, "ctest.coverage.filter.text", "notes.skip");
    }
    GtkWidget *apply = Find(root, "ctest.coverage.filter.apply"); g_object_ref(apply);
    Probe probe = {root, window, mode, 0U};
    gulong signal = 0U;
    if (!strcmp(mode, "edit-render") || !strcmp(mode, "close-render") || !strcmp(mode, "clear-render"))
        signal = g_signal_connect(buffer, "changed", G_CALLBACK(Changed), &probe);
    if (!strcmp(mode, "retained")) gtk_window_destroy(window);
    g_signal_emit_by_name(apply, "clicked");
    if (signal != 0U) g_signal_handler_disconnect(buffer, signal);
    char *after = Text(buffer);
    if (!strcmp(mode, "retained") || !strcmp(mode, "oversize")) CHECK(!strcmp(before, after));
    else {
        CHECK(strstr(after, "Whole review: 74 requirements | Passed: 1 | Outstanding: 73"));
        if (!strcmp(mode, "outstanding") || !strcmp(mode, "paging")) {
            CHECK(strstr(after, "Filter: 73 matching records") && !strstr(after, "[passed] Windows"));
            if (!strcmp(mode, "paging")) {
                Click(root, "ctest.coverage.next"); char *page = Text(buffer);
                CHECK(strstr(page, "Showing 65..73 of 73")); g_free(page);
                Click(root, "ctest.coverage.previous");
            }
        } else if (!strcmp(mode, "empty")) CHECK(strstr(after, "No matching rows. This is not a passing qualification."));
        else CHECK(strstr(after, "Filter: 1 matching records"));
        if (!strcmp(mode, "passed")) CHECK(strstr(after, "[passed] Windows / Debug : notes.pass"));
        if (!strcmp(mode, "host") || !strcmp(mode, "configuration")) CHECK(strstr(after, "[missing host] Linux"));
        if (!strcmp(mode, "edit-render")) {
            CHECK(probe.calls == 1U && strstr(after, "Applied test/capability text: notes.fail\n"));
            CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(root, "ctest.coverage.filter.text"))), "notes.pass"));
        }
        if (!strcmp(mode, "clear-render") || !strcmp(mode, "close-render")) CHECK(probe.calls == 1U);
        if (!strcmp(mode, "reload") || !strcmp(mode, "failed-reload")) {
            if (!strcmp(mode, "failed-reload")) Entry(root, "ctest.coverage.path", missing);
            Click(root, "ctest.coverage.open"); Wait(root);
            char *reloaded = Text(buffer); CHECK(!strcmp(after, reloaded)); g_free(reloaded);
        }
    }
    if (!strcmp(mode, "messages")) {
        CHECK(strstr(after, "[skipped] Windows / Debug : notes.skip"));
        CHECK(strstr(after, "Saved result message: SKIP_RETURN_CODE=77"));
        CHECK(!strstr(after, "Saved result message: Failed"));
    }
    g_free(after); g_free(before);
    gtk_window_destroy(window); g_object_unref(apply); g_object_unref(buffer); g_object_unref(root); g_object_unref(window);
    Drain(); return 0;
}
