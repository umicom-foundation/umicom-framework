/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_evidence_gtk4.c
 *
 * PURPOSE:
 *   Verify immutable accepted report evidence across native filters, replacement and interruption.
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
#include "umicom/ui/gtk4/ctest_report.h"
static GtkWidget *Find(GtkWidget *root, const char *tag)
{
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, tag);
    CHECK(widget); return widget;
}
static void Click(GtkWidget *root, const char *tag) { g_signal_emit_by_name(Find(root, tag), "clicked"); }
static void Drain(void)
{
    gint64 end = g_get_monotonic_time() + 30 * G_TIME_SPAN_MILLISECOND;
    while (g_get_monotonic_time() < end) {
        (void)UmiTestDispatchSlice(NULL, end, 32U); g_usleep(1000U);
    }
}
static void Wait(GtkWidget *root)
{
    gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (UmiGtk4CtestReportBusy(root) && g_get_monotonic_time() < end) {
        (void)UmiTestDispatchSlice(NULL, end, 32U); g_usleep(1000U);
    }
    CHECK(!UmiGtk4CtestReportBusy(root)); Drain();
}
static void Write(const char *path, const char *bytes)
{
    UmiOutputFile *file = NULL;
    OK(UmiOutputFileCreate(path, &file)); OK(UmiOutputFileWrite(file, bytes, strlen(bytes)));
    OK(UmiOutputFileClose(file)); UmiOutputFileDestroy(file);
}
static void Refusal(GtkWidget *root, UmiStatus wanted)
{
    UmiCtestReportEvidence out, before;
    memset(&out, 0x5a, sizeof out); memcpy(&before, &out, sizeof before);
    CHECK(UmiGtk4CtestReportReadEvidence(root, &out) == wanted);
    CHECK(!memcmp(&out, &before, sizeof out));
}

static void SameEvidence(const UmiCtestReportEvidence *a, const UmiCtestReportEvidence *b)
{
    CHECK(!strcmp(a->path, b->path) && a->identity.bytes == b->identity.bytes &&
          !strcmp(a->identity.sha256, b->identity.sha256));
    CHECK(a->summary.tests == b->summary.tests && a->summary.passed == b->summary.passed &&
          a->summary.failed == b->summary.failed && a->summary.timed_out == b->summary.timed_out &&
          a->summary.skipped == b->summary.skipped && a->summary.disabled == b->summary.disabled &&
          a->summary.not_run == b->summary.not_run &&
          a->summary.measured_test_ms == b->summary.measured_test_ms);
}

typedef struct Observation { GtkWidget *root; unsigned calls; } Observation;
static void DuringPublication(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    Observation *o = data; ++o->calls;
    Refusal(o->root, UMI_STATUS_BUSY);
}
int main(int argc, char **argv)
{
    const char *const cases[] = {"normal", "empty-suite", "mixed", "filter", "draft-path",
        "file-change", "cancel-replacement", "missing-replacement", "clear", "busy",
        "observer", "hidden", "owned", "null", "wrong-widget", "replacement"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    if (!gtk_init_check()) return 77;
    const char mixed[] =
        "<testsuite tests='6' failures='2' disabled='1' skipped='2'>"
        "<testcase name='pass' status='run' time='1'/>"
        "<testcase name='fail' status='fail' time='2'><failure message='Failed'/></testcase>"
        "<testcase name='timeout' status='fail' time='3'><failure message='Timeout'/></testcase>"
        "<testcase name='skip' status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase>"
        "<testcase name='disabled' status='disabled' time='0'/>"
        "<testcase name='missing' status='notrun' time='0'><skipped message='Unable to find executable'/></testcase>"
        "</testsuite>";
    const char replacement[] = "<testsuite tests='1'><testcase name='new' status='run' time='0'/></testsuite>";
    const char *xml = !strcmp(name, "empty-suite") ? "<testsuite tests='0'/>" : mixed;
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY], missing[UMI_PATH_CAPACITY];
    FixtureDirectory(directory); FixturePath(path, directory, "original.xml");
    FixturePath(other, directory, "replacement.xml"); FixturePath(missing, directory, "missing.xml");
    Write(path, xml); Write(other, replacement);
    GtkWidget *root = UmiGtk4CtestReportCreate(); CHECK(root); g_object_ref_sink(root);
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_child(window, root); gtk_window_present(window); Drain();
    Refusal(root, UMI_STATUS_NOT_FOUND);
    if (!strcmp(name, "null")) {
        Refusal(NULL, UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4CtestReportReadEvidence(root, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    if (!strcmp(name, "wrong-widget")) Refusal(GTK_WIDGET(window), UMI_STATUS_INVALID_ARGUMENT);
    Observation observed = {root, 0U}; gulong handler = 0U;
    GObject *label = G_OBJECT(Find(root, "ctest.report.evidence"));
    if (!strcmp(name, "observer"))
        handler = g_signal_connect(label, "notify::label", G_CALLBACK(DuringPublication), &observed);
    gtk_editable_set_text(GTK_EDITABLE(Find(root, "ctest.report.path")), path);
    Click(root, "ctest.report.open");
    if (!strcmp(name, "busy")) Refusal(root, UMI_STATUS_BUSY);
    Wait(root);
    if (handler) { CHECK(observed.calls == 1U); g_signal_handler_disconnect(label, handler); }
    UmiCtestReportEvidence first, result;
    OK(UmiGtk4CtestReportReadEvidence(root, &first));
    char *expected = g_compute_checksum_for_string(G_CHECKSUM_SHA256, xml, -1);
    CHECK(!strcmp(first.path, path) && first.identity.bytes == strlen(xml));
    CHECK(!strcmp(first.identity.sha256, expected)); g_free(expected);
    if (!strcmp(name, "empty-suite")) CHECK(first.summary.tests == 0U);
    else {
        CHECK(first.summary.tests == 6U && first.summary.passed == 1U && first.summary.failed == 1U);
        CHECK(first.summary.timed_out == 1U && first.summary.skipped == 1U &&
              first.summary.disabled == 1U && first.summary.not_run == 1U);
    }
    const char *display = gtk_label_get_text(GTK_LABEL(label));
    CHECK(strstr(display, first.identity.sha256) != NULL);
    if (!strcmp(name, "filter")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "ctest.report.outcome")), 2U);
        Click(root, "ctest.report.apply");
    } else if (!strcmp(name, "draft-path")) {
        gtk_editable_set_text(GTK_EDITABLE(Find(root, "ctest.report.path")), other);
    } else if (!strcmp(name, "file-change")) {
        /* Change only this fixture file after admission. Evidence must continue
         * to identify its owned parser input, with no implicit second read. */
        CHECK(g_file_set_contents(path, replacement, -1, NULL));
    } else if (!strcmp(name, "missing-replacement") || !strcmp(name, "cancel-replacement") ||
               !strcmp(name, "replacement")) {
        gtk_editable_set_text(GTK_EDITABLE(Find(root, "ctest.report.path")),
            !strcmp(name, "missing-replacement") ? missing : other);
        Click(root, "ctest.report.open");
        if (!strcmp(name, "cancel-replacement")) Click(root, "ctest.report.cancel");
        Wait(root);
    } else if (!strcmp(name, "hidden")) gtk_widget_set_visible(root, FALSE);
    if (!strcmp(name, "clear")) {
        Click(root, "ctest.report.clear"); Refusal(root, UMI_STATUS_NOT_FOUND);
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(label)), "No accepted report evidence"));
    } else {
        OK(UmiGtk4CtestReportReadEvidence(root, &result));
        if (!strcmp(name, "replacement")) {
            expected = g_compute_checksum_for_string(G_CHECKSUM_SHA256, replacement, -1);
            CHECK(!strcmp(result.path, other) && !strcmp(result.identity.sha256, expected));
            CHECK(result.summary.tests == 1U && result.summary.passed == 1U); g_free(expected);
        } else {
            /* Compare public values, not compiler-dependent struct padding.
             * The earlier byte comparison is retained for review. */
#if 0
            CHECK(!memcmp(&first, &result, sizeof first));
#endif
            SameEvidence(&first, &result);
        }
    }
    gtk_window_destroy(window); g_object_unref(root); Drain();
    /* Public evidence is a value, not a view into widget or parser lifetimes. */
    expected = g_compute_checksum_for_string(G_CHECKSUM_SHA256, xml, -1);
    CHECK(!strcmp(first.identity.sha256, expected) && !strcmp(first.path, path)); g_free(expected);
    return 0;
}
