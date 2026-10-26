/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_comparison_gtk4.c
 *
 * PURPOSE:
 *   Check captured comparison inputs, publication boundaries and retained native widget lifetime.
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
#include "umicom/ui/gtk4/ctest_comparison.h"
#include "umicom/base/sha256.h"

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
    while (UmiGtk4CtestComparisonPending(root)) {
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
static void HideOnText(GtkTextBuffer *buffer, GtkWidget *root)
{
    (void)buffer;
    gtk_widget_set_visible(root, FALSE);
}
static void CancelOnText(GtkTextBuffer *buffer, GtkWidget *root)
{
    (void)buffer;
    Click(root, "ctest.comparison.cancel");
}
static void ReenterOnText(GtkTextBuffer *buffer, GtkWidget *root)
{
    (void)buffer;
    Click(root, "ctest.comparison.open");
}
static void CloseOnText(GtkTextBuffer *buffer, GtkWindow *window)
{
    (void)buffer;
    gtk_window_destroy(window);
}
int main(int argc, char **argv)
{
    const char *cases[] = {"normal", "paging", "captured-before", "captured-after", "cancel-load",
        "hide-show", "close-load", "hidden", "retained", "replacement-before", "replacement-after",
        "malformed", "relative", "clear", "hide-clear", "cancel-render", "hide-render",
        "close-render", "reentrant", "empty", "unchanged-failure", "cancel-replacement", "identity-reload"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *test = argv[1];
    if (!gtk_init_check()) return 77;
    char directory[UMI_PATH_CAPACITY], before[UMI_PATH_CAPACITY], after[UMI_PATH_CAPACITY],
        missing[UMI_PATH_CAPACITY], malformed[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(before, directory, "before.xml");
    FixturePath(after, directory, "after.xml");
    FixturePath(missing, directory, "missing.xml");
    FixturePath(malformed, directory, "partial.xml");
    char *many = ManyXml(130);
    const char *empty = "<testsuite tests='0'></testsuite>";
    Write(before, !strcmp(test, "unchanged-failure") ? many : empty);
    Write(after, !strcmp(test, "empty") ? empty : many);
    Write(malformed, "<testsuite>");
    char before_hash[UMI_SHA256_HEX_CAPACITY], after_hash[UMI_SHA256_HEX_CAPACITY];
    const char *before_xml = !strcmp(test, "unchanged-failure") ? many : empty;
    const char *after_xml = !strcmp(test, "empty") ? empty : many;
    OK(UmiSha256Buffer(before_xml, strlen(before_xml), before_hash));
    OK(UmiSha256Buffer(after_xml, strlen(after_xml), after_hash));
    char *accepted_identity = g_strdup_printf("Baseline bytes: %zu | SHA-256: %s\nCurrent bytes: %zu | SHA-256: %s\n",
        strlen(before_xml), before_hash, strlen(after_xml), after_hash);
    free(many);
    GtkApplication *application = gtk_application_new("org.umicom.SavedComparison", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    GtkWindow *window = GTK_WINDOW(gtk_application_window_new(application));
    g_object_ref(window);
    GtkWidget *root = UmiGtk4CtestComparisonCreate();
    g_object_ref_sink(root);
    gtk_window_set_child(window, root);
    gtk_window_present(window);
    Drain();
    GtkWidget *left = Find(root, "ctest.comparison.before");
    GtkWidget *right = Find(root, "ctest.comparison.after");
    GtkWidget *open = Find(root, "ctest.comparison.open");
    CHECK(GTK_IS_EDITABLE(left) && GTK_IS_EDITABLE(right) && gtk_widget_get_mapped(open));
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(
        GTK_TEXT_VIEW(Find(root, "ctest.comparison.output"))));
    gtk_editable_set_text(GTK_EDITABLE(left), before);
    gtk_editable_set_text(GTK_EDITABLE(right), after);
    if (!strcmp(test, "hidden")) {
        gtk_widget_set_visible(root, FALSE);
        Click(root, "ctest.comparison.open");
        CHECK(!UmiGtk4CtestComparisonPending(root));
        char *text = Text(buffer); CHECK(!text[0]); g_free(text);
        goto done;
    }
    if (!strcmp(test, "retained")) {
        g_object_ref(open);
        gtk_window_destroy(window);
        g_signal_emit_by_name(open, "clicked");
        CHECK(!UmiGtk4CtestComparisonPending(root));
        g_object_unref(open);
        char *text = Text(buffer); CHECK(!text[0]); g_free(text);
        goto done;
    }
    Click(root, "ctest.comparison.open");
    CHECK(UmiGtk4CtestComparisonPending(root));
    if (!strcmp(test, "captured-before")) gtk_editable_set_text(GTK_EDITABLE(left), missing);
    if (!strcmp(test, "captured-after")) gtk_editable_set_text(GTK_EDITABLE(right), missing);
    if (!strcmp(test, "cancel-load")) Click(root, "ctest.comparison.cancel");
    if (!strcmp(test, "hide-show")) {
        gtk_widget_set_visible(root, FALSE);
        gtk_widget_set_visible(root, TRUE);
    }
    if (!strcmp(test, "close-load")) gtk_window_destroy(window);
    if (!strcmp(test, "cancel-render"))
        g_signal_connect_object(buffer, "changed", G_CALLBACK(CancelOnText), root, 0);
    if (!strcmp(test, "hide-render"))
        g_signal_connect_object(buffer, "changed", G_CALLBACK(HideOnText), root, 0);
    if (!strcmp(test, "close-render"))
        g_signal_connect_object(buffer, "changed", G_CALLBACK(CloseOnText), window, 0);
    if (!strcmp(test, "reentrant"))
        g_signal_connect_object(buffer, "changed", G_CALLBACK(ReenterOnText), root, 0);
    Wait(root);
    char *accepted = Text(buffer);
    if (!strcmp(test, "cancel-load") || !strcmp(test, "hide-show") || !strcmp(test, "close-load")) {
        CHECK(!accepted[0]); g_free(accepted); goto done;
    }
    CHECK(strstr(accepted, accepted_identity));
    if (!strcmp(test, "empty") || !strcmp(test, "unchanged-failure")) {
        CHECK(strstr(accepted, "No outcome changes. This does not establish an all-passed result."));
        if (!strcmp(test, "unchanged-failure")) CHECK(strstr(accepted, "Unchanged outcomes: 130"));
    } else CHECK(strstr(accepted, "Showing 1..64 of 130") && strstr(accepted, "Added: 130 | Removed: 0"));
    CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.comparison.before-source"))), before));
    CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.comparison.after-source"))), after));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.comparison.status"))), "Saved pair accepted."));
    if (!strcmp(test, "paging") || strstr(test, "replacement") || !strcmp(test, "malformed")) {
        Click(root, "ctest.comparison.next");
        g_free(accepted); accepted = Text(buffer);
        CHECK(strstr(accepted, "Showing 65..128 of 130"));
        if (!strcmp(test, "paging")) {
            Click(root, "ctest.comparison.next");
            char *last = Text(buffer); CHECK(strstr(last, "Showing 129..130 of 130")); g_free(last);
            Click(root, "ctest.comparison.previous");
        } else if (!strcmp(test, "cancel-replacement")) {
            gtk_editable_set_text(GTK_EDITABLE(left), after);
            Click(root, "ctest.comparison.open"); Click(root, "ctest.comparison.cancel"); Wait(root);
        } else {
            gtk_editable_set_text(GTK_EDITABLE(!strcmp(test, "replacement-before") ? left : right),
                !strcmp(test, "malformed") ? malformed : missing);
            Click(root, "ctest.comparison.open"); Wait(root);
        }
        char *current = Text(buffer); CHECK(!strcmp(accepted, current)); g_free(current);
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.comparison.before-source"))), before));
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.comparison.after-source"))), after));
    }
    if (!strcmp(test, "identity-reload")) {
        gtk_editable_set_text(GTK_EDITABLE(left), after);
        Click(root, "ctest.comparison.open"); Wait(root);
        char *current = Text(buffer);
        CHECK(strstr(current, "Unchanged outcomes: 130"));
        CHECK(!strstr(current, before_hash) && strstr(current, after_hash));
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.comparison.before-source"))), after));
        g_free(current);
    }
    if (!strcmp(test, "relative")) {
        gtk_editable_set_text(GTK_EDITABLE(right), "after.xml");
        Click(root, "ctest.comparison.open");
        CHECK(!UmiGtk4CtestComparisonPending(root));
        char *current = Text(buffer); CHECK(!strcmp(accepted, current)); g_free(current);
    }
    if (!strcmp(test, "clear") || !strcmp(test, "hide-clear")) {
        if (!strcmp(test, "hide-clear"))
            g_signal_connect_object(buffer, "changed", G_CALLBACK(HideOnText), root, 0);
        Click(root, "ctest.comparison.clear");
        char *current = Text(buffer); CHECK(!current[0]); g_free(current);
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.comparison.before-source"))), "No baseline accepted"));
        CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "ctest.comparison.after-source"))), "No current report accepted"));
    }
    g_free(accepted);
done:
    g_free(accepted_identity);
    gtk_window_destroy(window);
    Wait(root);
    g_object_unref(buffer);
    g_object_unref(root);
    g_object_unref(window);
    g_object_unref(application);
    return 0;
}
