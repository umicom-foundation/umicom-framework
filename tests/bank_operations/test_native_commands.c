/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_operations/test_native_commands.c
 *
 * PURPOSE:
 *   Exercise real banking controls, review invalidation and window ownership with memory storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/bank_operations/operations.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

/* Only the storage boundary is substituted. The real launcher, widgets,
 * command parsing, review, submit and query services remain under test. No
 * directory or personal database is created by this fixture. Persistence
 * restart coverage belongs to the separate SQLite service tests. */
/* Automatic directory creation was removed from the production path. Preserve
 * the old isolation stub for review; only the database boundary is substituted. */
#if 0
static int IsolatedDirectory(const char *directory, int mode)
{
    (void)directory; (void)mode; return 0;
}
#endif
static unsigned OpenCalls;
static bool FailOpen;
static char *OpenedPath;
static UmiStatus IsolatedOpen(const char *filename, UmiBankOperations **out)
{
    if (filename == NULL || !g_path_is_absolute(filename)) return UMI_STATUS_INVALID_ARGUMENT;
    ++OpenCalls;
    g_free(OpenedPath); OpenedPath = g_strdup(filename);
    if (FailOpen) { *out = NULL; return UMI_STATUS_IO_ERROR; }
    return UmiBankOperationsOpenMemory(out);
}
#if 0 /* Retained automatic-directory boundary; no longer used. */
#define g_mkdir_with_parents IsolatedDirectory
#endif
#define UmiBankOperationsOpenSqlite IsolatedOpen
#include "../../adapters/gtk4/bank_operations_gtk4.c"
#undef UmiBankOperationsOpenSqlite
#undef g_mkdir_with_parents

#define CHECK(value) do { if (!(value)) { fprintf(stderr, "Bank UI line %d: %s\n", __LINE__, #value); return 1; } } while (0)
static GtkWidget *FindButton(GtkWidget *root, const char *label)
{
    if (GTK_IS_BUTTON(root) && g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), label) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = FindButton(child, label);
        if (found != NULL) return found;
    }
    return NULL;
}
static bool Activate(GtkWidget *root, const char *label)
{
    GtkWidget *button = FindButton(root, label);
    if (button == NULL) return false;
    g_signal_emit_by_name(button, "clicked"); return true;
}
static void Set(GtkEntry *entry, const char *text)
{
    gtk_editable_set_text(GTK_EDITABLE(entry), text);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check()) return 77;
    GtkWidget *parent = gtk_window_new();
    g_object_ref_sink(parent);
    GtkWidget *bar = UmiGtk4BankOperationsLauncherCreate();
    gtk_window_set_child(GTK_WINDOW(parent), bar);
    GtkWidget *button = FindButton(bar, "Banking operations"); CHECK(button != NULL);
    g_object_ref(button);
    /* Merely creating, parenting or clicking an empty launcher cannot open storage. */
    g_signal_emit_by_name(button, "clicked");
    CHECK(OpenCalls == 0U);
    char *chosen = g_build_filename(g_get_tmp_dir(), "bank workspace", "practice-£.sqlite", NULL);
    CHECK(UmiGtk4BankOperationsLauncherSetPath(bar, chosen) == UMI_STATUS_OK);
    CHECK(OpenCalls == 0U);
    CHECK(UmiGtk4BankOperationsLauncherSetPath(bar, "relative.sqlite") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4BankOperationsLauncherSetPath(bar, "https://example.invalid/bank") == UMI_STATUS_INVALID_ARGUMENT);
    const char invalidUtf8[] = {(char)0xff, '\0'};
    CHECK(UmiGtk4BankOperationsLauncherSetPath(bar, invalidUtf8) == UMI_STATUS_INVALID_ARGUMENT);
    if (strcmp(argv[1], "open-retry") == 0) {
        FailOpen = true;
        g_signal_emit_by_name(button, "clicked");
        BankLauncher *pending = g_object_get_data(G_OBJECT(button), "umicom-bank-launcher");
        CHECK(OpenCalls == 1U && pending->window == NULL);
        FailOpen = false;
    }
    unsigned beforeOpen = OpenCalls;
    g_signal_emit_by_name(button, "clicked");
    CHECK(OpenCalls == beforeOpen + 1U && strcmp(OpenedPath, chosen) == 0);
    g_free(chosen);
    BankLauncher *launcher = g_object_get_data(G_OBJECT(button), "umicom-bank-launcher");
    CHECK(launcher != NULL && launcher->window != NULL);
    GtkWindow *child = g_object_ref(launcher->window);
    BankUi *ui = g_object_get_data(G_OBJECT(child), "umicom-bank-operations");
    CHECK(ui != NULL && ui->operations != NULL);
    UmiBankCounts counts;
    CHECK(UmiBankOperationsCounts(ui->operations, &counts) == UMI_STATUS_OK);
    CHECK(counts.customers == 0U && counts.revision == 0U && !counts.durable);
    if (strcmp(argv[1], "storage-selection") == 0 || strcmp(argv[1], "open-retry") == 0) {
        unsigned previousCalls = OpenCalls;
        char *original = g_strdup(ui->path);
        CHECK(UmiGtk4BankOperationsLauncherSetPath(bar, "") == UMI_STATUS_OK);
        g_signal_emit_by_name(button, "clicked");
        CHECK(launcher->window == child && OpenCalls == previousCalls);
        CHECK(strcmp(ui->path, original) == 0);
        gtk_window_destroy(child);
        CHECK(launcher->window == NULL && ui->operations == NULL && ui->closed);
        g_signal_emit_by_name(button, "clicked");
        CHECK(launcher->window == NULL && OpenCalls == previousCalls);
        g_free(original);
    } else if (strcmp(argv[1], "close-reopen") == 0) {
        g_signal_emit_by_name(button, "clicked");
        CHECK(launcher->window == child); /* Re-entry reuses the owned window. */
        GtkButton *retainedSubmit = g_object_ref(ui->submit);
        gtk_window_destroy(child);
        CHECK(ui->closed && ui->operations == NULL && launcher->window == NULL);
        g_signal_emit_by_name(retainedSubmit, "clicked");
        g_object_unref(retainedSubmit);
        g_signal_emit_by_name(button, "clicked");
        CHECK(launcher->window != NULL && launcher->window != child);
        gtk_window_destroy(launcher->window);
    } else if (strcmp(argv[1], "action-catalogue") == 0) {
        /* Every advertised banking action must update the same reviewed form.
         * Merely changing action or report pages cannot commit financial state. */
        for (guint i = 0U; i < (guint)UMI_BANK_ACTION_LAST; ++i) {
            gtk_drop_down_set_selected(ui->action, i);
            CHECK(gtk_drop_down_get_selected(ui->action) == i);
            CHECK(ui->review == NULL && !gtk_widget_get_sensitive(GTK_WIDGET(ui->submit)));
            CHECK(gtk_label_get_text(ui->hint)[0] != '\0');
            CHECK(UmiBankOperationsCounts(ui->operations, &counts) == UMI_STATUS_OK);
            CHECK(counts.customers == 0U && counts.events == 0U && counts.revision == 0U);
        }
        int pages = gtk_notebook_get_n_pages(ui->pages);
        CHECK(pages > 0);
        for (int i = 0; i < pages; ++i) {
            gtk_notebook_set_current_page(ui->pages, i);
            CHECK(gtk_notebook_get_current_page(ui->pages) == i);
        }
        CHECK(UmiBankOperationsCounts(ui->operations, &counts) == UMI_STATUS_OK && counts.revision == 0U);
        gtk_window_destroy(child);
    } else {
        CHECK(Activate(GTK_WIDGET(child), "New request"));
        Set(ui->request, "native-customer-request");
        Set(ui->id, "native-customer");
        Set(ui->name, "Practice customer");
        Set(ui->date, "2026-01-02");
        CHECK(!gtk_widget_get_sensitive(GTK_WIDGET(ui->submit)));
        g_signal_emit_by_name(ui->submit, "clicked");
        CHECK(UmiBankOperationsCounts(ui->operations, &counts) == UMI_STATUS_OK && counts.revision == 0U);
        CHECK(Activate(GTK_WIDGET(child), "Review command"));
        CHECK(ui->review != NULL && gtk_widget_get_sensitive(GTK_WIDGET(ui->submit)));
        CHECK(UmiBankOperationsCounts(ui->operations, &counts) == UMI_STATUS_OK && counts.revision == 0U);
        if (strcmp(argv[1], "edited-review") == 0) {
            Set(ui->name, "Revised practice customer");
            CHECK(ui->review == NULL && !gtk_widget_get_sensitive(GTK_WIDGET(ui->submit)));
            g_signal_emit_by_name(ui->submit, "clicked");
            CHECK(UmiBankOperationsCounts(ui->operations, &counts) == UMI_STATUS_OK && counts.revision == 0U);
            CHECK(Activate(GTK_WIDGET(child), "Review command"));
        } else CHECK(strcmp(argv[1], "review-submit") == 0);
        g_signal_emit_by_name(ui->submit, "clicked");
        CHECK(UmiBankOperationsCounts(ui->operations, &counts) == UMI_STATUS_OK);
        CHECK(counts.customers == 1U && counts.events == 1U && counts.revision == 1U);
        CHECK(ui->review == NULL && !gtk_widget_get_sensitive(GTK_WIDGET(ui->submit)));
        g_signal_emit_by_name(ui->submit, "clicked");
        CHECK(UmiBankOperationsCounts(ui->operations, &counts) == UMI_STATUS_OK && counts.revision == 1U);
        gtk_window_destroy(child);
    }
    g_object_unref(child);
    gtk_window_destroy(GTK_WINDOW(parent)); g_object_unref(parent);
    /* A retained launcher cannot create a replacement after parent removal. */
    g_signal_emit_by_name(button, "clicked");
    CHECK(launcher->window == NULL);
    g_object_unref(button);
    g_free(OpenedPath);
    return 0;
}
