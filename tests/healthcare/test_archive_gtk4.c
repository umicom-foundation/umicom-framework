/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_archive_gtk4.c
 * PURPOSE: Exercise explicit save/open, retained edits and archive notification lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/healthcare/access_gtk4.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); result = 1; goto cleanup; } } while (0)
static GtkWidget *find(GtkWidget *root, const char *name)
{
    if (!strcmp(gtk_widget_get_name(root), name)) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *match = find(child, name);
        if (match) return match;
    }
    return NULL;
}
static bool click(GtkWidget *root, const char *name)
{
    GtkWidget *button = find(root, name);
    if (!button) return false;
    g_signal_emit_by_name(button, "clicked");
    return true;
}
static guint count(GtkWidget *root)
{
    GtkWidget *selector = find(root, "healthcare-requests");
    return selector ? g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(selector))) : 0U;
}
static bool wait_idle(GtkWidget *root)
{
    gint64 deadline = g_get_monotonic_time() + 10 * G_USEC_PER_SEC;
    while (UmiHealthcareAccessWorkspaceBusy(root) && g_get_monotonic_time() < deadline) {
        (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return !UmiHealthcareAccessWorkspaceBusy(root);
}
typedef struct CloseObserver { GtkWidget *root; bool fired; } CloseObserver;
static void close_owner(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    CloseObserver *observer = data;
    if (observer->fired) return;
    observer->fired = true;
    g_object_run_dispose(G_OBJECT(observer->root));
}
int main(int argc, char **argv)
{
    const char *name = argc == 3 ? argv[1] : "";
    GtkWidget *root = NULL, *observed = NULL, *retained = NULL;
    char *directory = NULL, *archive = NULL, *alternate = NULL, *before = NULL, *after = NULL;
    gsize beforeLength = 0U, afterLength = 0U;
    gulong signal = 0U;
    CloseObserver observer = {NULL, false};
    int result = 0;
    if (!gtk_init_check()) return 77;
    CHECK(argc == 3 && g_path_is_absolute(argv[2]));
    CHECK(g_mkdir_with_parents(argv[2], 0700) == 0);
    directory = g_build_filename(argv[2], "run-XXXXXX", NULL);
    CHECK(g_mkdtemp(directory) != NULL);
    archive = g_build_filename(directory, "synthetic.json", NULL);
    alternate = g_build_filename(directory, "alternate.json", NULL);
    CHECK(UmiHealthcareAccessWorkspaceNew(&root) == UMI_STATUS_OK);
    g_object_ref_sink(root);
    CHECK(find(root, "healthcare-archive-path") != NULL);
    gtk_editable_set_text(GTK_EDITABLE(find(root, "healthcare-archive-path")), archive);
    CHECK(click(root, "healthcare-register")); CHECK(click(root, "healthcare-request"));
    CHECK(click(root, "healthcare-no-response")); CHECK(click(root, "healthcare-unmet"));
    CHECK(click(root, "healthcare-followup"));
    if (!strcmp(name, "acceptance-close")) {
        observer.root = root;
        observed = g_object_ref(find(root, "healthcare-archive-status"));
        retained = g_object_ref(find(root, "healthcare-archive-save"));
        signal = g_signal_connect(observed, "notify::label", G_CALLBACK(close_owner), &observer);
        CHECK(click(root, "healthcare-archive-save"));
        CHECK(observer.fired);
        CHECK(!g_file_test(archive, G_FILE_TEST_EXISTS));
        g_signal_emit_by_name(retained, "clicked");
        CHECK(!g_file_test(archive, G_FILE_TEST_EXISTS));
    } else {
        CHECK(click(root, "healthcare-archive-save"));
        if (!strcmp(name, "busy")) {
            CHECK(UmiHealthcareAccessWorkspaceBusy(root));
            CHECK(click(root, "healthcare-request")); CHECK(count(root) == 1U);
            gtk_editable_set_text(GTK_EDITABLE(find(root, "healthcare-archive-path")), alternate);
            CHECK(click(root, "healthcare-archive-save"));
        }
        CHECK(wait_idle(root));
        CHECK(g_file_get_contents(archive, &before, &beforeLength, NULL));
        CHECK(strstr(before, "STF-000001") != NULL);
        if (!strcmp(name, "roundtrip")) {
            g_object_unref(root); root = NULL;
            CHECK(UmiHealthcareAccessWorkspaceNew(&root) == UMI_STATUS_OK);
            g_object_ref_sink(root);
            gtk_editable_set_text(GTK_EDITABLE(find(root, "healthcare-archive-path")), archive);
            CHECK(click(root, "healthcare-archive-open")); CHECK(wait_idle(root));
            CHECK(count(root) == 1U);
            GtkDropDown *selector = GTK_DROP_DOWN(find(root, "healthcare-requests"));
            CHECK(gtk_drop_down_get_selected(selector) == GTK_INVALID_LIST_POSITION);
            gtk_drop_down_set_selected(selector, 0U);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(root, "healthcare-details"))), "STF-000001") != NULL);
            CHECK(click(root, "healthcare-request")); CHECK(count(root) == 2U);
        } else if (!strcmp(name, "overwrite")) {
            CHECK(click(root, "healthcare-request")); CHECK(count(root) == 2U);
            CHECK(click(root, "healthcare-archive-save")); CHECK(wait_idle(root));
            CHECK(g_file_get_contents(archive, &after, &afterLength, NULL));
            CHECK(beforeLength == afterLength && memcmp(before, after, beforeLength) == 0);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(root, "healthcare-archive-status"))), "Save failed") != NULL);
        } else if (!strcmp(name, "busy")) {
            CHECK(count(root) == 1U);
            CHECK(!g_file_test(alternate, G_FILE_TEST_EXISTS));
            CHECK(strstr(before, "REQ-000002") == NULL);
        } else if (!strcmp(name, "approval") || !strcmp(name, "malformed") || !strcmp(name, "completion-close")) {
            CHECK(click(root, "healthcare-request")); CHECK(count(root) == 2U);
            CHECK(click(root, "healthcare-archive-open"));
            CHECK(!UmiHealthcareAccessWorkspaceBusy(root)); CHECK(count(root) == 2U);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(root, "healthcare-archive-status"))), "confirm") != NULL);
            gtk_check_button_set_active(GTK_CHECK_BUTTON(find(root, "healthcare-archive-replace")), TRUE);
            if (!strcmp(name, "malformed")) {
                CHECK(g_file_set_contents(alternate, "{\"format\":\"bad\"}", -1, NULL));
                gtk_editable_set_text(GTK_EDITABLE(find(root, "healthcare-archive-path")), alternate);
            }
            if (!strcmp(name, "completion-close")) {
                observer.root = root;
                observed = g_object_ref(find(root, "healthcare-worklist-summary"));
                retained = g_object_ref(find(root, "healthcare-archive-open"));
                signal = g_signal_connect(observed, "notify::label", G_CALLBACK(close_owner), &observer);
            }
            CHECK(click(root, "healthcare-archive-open")); CHECK(wait_idle(root));
            if (!strcmp(name, "completion-close")) {
                CHECK(observer.fired);
                g_signal_emit_by_name(retained, "clicked");
            } else {
                CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(find(root, "healthcare-archive-replace"))));
                CHECK(count(root) == (!strcmp(name, "malformed") ? 2U : 1U));
                if (!strcmp(name, "malformed"))
                    CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(root, "healthcare-archive-status"))), "unchanged") != NULL);
            }
        } else CHECK(0 && "unknown case");
    }
cleanup:
    if (observed && signal) g_signal_handler_disconnect(observed, signal);
    if (root) { (void)wait_idle(root); g_object_unref(root); }
    g_clear_object(&observed); g_clear_object(&retained);
    g_free(before); g_free(after);
    /* Only remove files created in this test's unique, explicitly supplied directory. */
    if (archive) (void)g_remove(archive);
    if (alternate) (void)g_remove(alternate);
    if (directory) (void)g_rmdir(directory);
    g_free(archive); g_free(alternate); g_free(directory);
    return result;
}
