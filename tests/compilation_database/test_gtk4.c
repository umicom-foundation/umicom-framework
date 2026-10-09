/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compilation_database/test_gtk4.c
 * PURPOSE: Exercise explicit compiler-database reads, argument preparation and stale native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/arguments.h"
#include "umicom/language_runtime/json_writer.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/compilation_database.h"
#include <glib/gstdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition)                                                                           \
    do                                                                                             \
    {                                                                                              \
        if (!(condition))                                                                          \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition);                        \
            failed = 1;                                                                            \
            goto cleanup;                                                                          \
        }                                                                                          \
    } while (0)
static void Drain(void)
{
    for (unsigned i = 0U; i < 20U; ++i)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000U);
    }
}
static bool Wait(GtkWidget *panel)
{
    for (unsigned i = 0U; i < 2000U; ++i)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        if (g_object_get_data(G_OBJECT(panel), "umicom-compilation-pending") == NULL)
            return true;
        g_usleep(1000U);
    }
    return false;
}
static void CloseOnArguments(GtkEditable *entry, gpointer data)
{
    (void)entry;
    gtk_window_destroy(GTK_WINDOW(data));
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {"read",    "use",          "changed",       "cancel",
                           "hidden",  "retained",     "pending-close", "use-close",
                           "missing", "other-window", "source"};
    bool known = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    g_setenv("GTK_A11Y", "test", TRUE);
    g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    GError *error = NULL;
    gchar *directory = g_dir_make_tmp("umicom-compiler-database-XXXXXX", &error);
    gchar *path = NULL, *source = NULL;
    GtkWindow *window = NULL, *other = NULL;
    GtkWidget *panel = NULL, *arguments = NULL, *read = NULL, *use = NULL;
    CHECK(directory != NULL);
    path = g_build_filename(directory, "compile_commands.json", NULL);
    source = g_build_filename(directory, "main.c", NULL);
    char json[10000];
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, json, sizeof json);
    CHECK(umi_language_runtime_json_writer_raw(&writer, "[{\"directory\":") == UMI_STATUS_OK);
    CHECK(umi_language_runtime_json_writer_string(&writer, directory) == UMI_STATUS_OK);
    CHECK(umi_language_runtime_json_writer_raw(
              &writer, ",\"file\":\"main.c\",\"arguments\":[\"cc\",\"-std=c23\",\"main.c\"]}]") ==
          UMI_STATUS_OK);
    if (strcmp(mode, "missing") != 0)
        CHECK(g_file_set_contents(path, json, -1, &error));
    window = GTK_WINDOW(gtk_window_new());
    g_object_ref(window);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_window_set_child(window, box);
    arguments = gtk_entry_new();
    g_object_ref_sink(arguments);
    gtk_editable_set_text(GTK_EDITABLE(arguments), "--background-index");
    gtk_box_append(GTK_BOX(box), arguments);
    CHECK(UmiGtk4CompilationDatabasePanelCreate(directory,
                                                strcmp(mode, "source") == 0 ? source : NULL,
                                                GTK_EDITABLE(arguments), &panel) == UMI_STATUS_OK);
    g_object_ref_sink(panel);
    gtk_box_append(GTK_BOX(box), panel);
    gtk_window_present(window);
    Drain();
    read = umi_gtk4_automation_find_tagged_widget(panel, "compiler.database.read");
    use = umi_gtk4_automation_find_tagged_widget(panel, "compiler.database.use");
    GtkWidget *folder =
        umi_gtk4_automation_find_tagged_widget(panel, "compiler.database.directory");
    GtkWidget *cancel = umi_gtk4_automation_find_tagged_widget(panel, "compiler.database.cancel");
    GtkWidget *message = umi_gtk4_automation_find_tagged_widget(panel, "compiler.database.message");
    GtkWidget *details = umi_gtk4_automation_find_tagged_widget(panel, "compiler.database.details");
    CHECK(read != NULL && use != NULL && folder != NULL && cancel != NULL && message != NULL &&
          details != NULL);
    CHECK(g_object_get_data(G_OBJECT(panel), "umicom-compilation-pending") == NULL);
    CHECK(!gtk_widget_get_sensitive(use));
    if (strcmp(mode, "retained") == 0)
    {
        gtk_window_destroy(window);
        g_signal_emit_by_name(read, "clicked");
        g_signal_emit_by_name(use, "clicked");
        CHECK(g_object_get_data(G_OBJECT(panel), "umicom-compilation-pending") == NULL);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(arguments)), "--background-index") == 0);
        goto cleanup;
    }
    g_signal_emit_by_name(read, "clicked");
    CHECK(g_object_get_data(G_OBJECT(panel), "umicom-compilation-pending") != NULL);
    if (strcmp(mode, "pending-close") == 0)
        gtk_window_destroy(window);
    if (strcmp(mode, "cancel") == 0)
        g_signal_emit_by_name(cancel, "clicked");
    if (strcmp(mode, "hidden") == 0)
        gtk_widget_set_visible(panel, FALSE);
    if (strcmp(mode, "changed") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(folder), "changed");
        gtk_editable_set_text(GTK_EDITABLE(folder), directory);
    }
    CHECK(Wait(panel));
    if (strcmp(mode, "pending-close") == 0 || strcmp(mode, "cancel") == 0 ||
        strcmp(mode, "hidden") == 0 || strcmp(mode, "changed") == 0 || strcmp(mode, "missing") == 0)
    {
        g_signal_emit_by_name(use, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(arguments)), "--background-index") == 0);
        if (strcmp(mode, "hidden") == 0)
        {
            gtk_widget_set_visible(panel, TRUE);
            Drain();
            CHECK(gtk_widget_get_sensitive(read));
        }
        goto cleanup;
    }
    CHECK(gtk_widget_get_sensitive(use));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(message)), "Read 1 command rows") != NULL);
    if (strcmp(mode, "source") == 0)
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(message)), "first matching source") != NULL);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(details));
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    gchar *preview = gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
    bool includes = strstr(preview, "-std=c23") != NULL && strstr(preview, "main.c") != NULL;
    g_free(preview);
    CHECK(includes);
    if (strcmp(mode, "other-window") == 0)
    {
        other = GTK_WINDOW(gtk_window_new());
        g_object_ref(other);
        gtk_box_remove(GTK_BOX(box), arguments);
        gtk_window_set_child(other, arguments);
        gtk_window_present(other);
        g_signal_emit_by_name(use, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(arguments)), "--background-index") == 0);
    }
    else if (strcmp(mode, "use") == 0 || strcmp(mode, "use-close") == 0)
    {
        if (strcmp(mode, "use-close") == 0)
            g_signal_connect(arguments, "changed", G_CALLBACK(CloseOnArguments), window);
        g_signal_emit_by_name(use, "clicked");
        UmiArguments *parsed = g_new0(UmiArguments, 1U);
        UmiStatus parsed_status =
            UmiArgumentsParse(gtk_editable_get_text(GTK_EDITABLE(arguments)), parsed);
        bool selected = parsed_status == UMI_STATUS_OK && parsed->count == 2U &&
                        strncmp(parsed->values[1], "--compile-commands-dir=", 23U) == 0;
        g_free(parsed);
        CHECK(selected);
    }
cleanup:
    if (window != NULL)
        gtk_window_destroy(window);
    if (other != NULL)
        gtk_window_destroy(other);
    if (panel != NULL)
        (void)Wait(panel);
    g_clear_object(&other);
    g_clear_object(&window);
    g_clear_object(&panel);
    g_clear_object(&arguments);
    if (path != NULL && strcmp(mode, "missing") != 0)
        (void)g_unlink(path);
    if (directory != NULL)
        (void)g_rmdir(directory);
    g_free(source);
    g_free(path);
    g_free(directory);
    g_clear_error(&error);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
