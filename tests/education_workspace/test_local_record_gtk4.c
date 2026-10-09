/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education_workspace/test_local_record_gtk4.c
 * PURPOSE: Check explicit learning storage selection through the native panel and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/education_workspace/local_record.h"
#include "umicom/platform/filesystem.h"
#include "umicom/ui/gtk4/education_workspace.h"
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
static void Click(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id);
    CHECK(GTK_IS_BUTTON(button));
    g_signal_emit_by_name(button, "clicked");
}
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 2);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    UmiEducationGtkPanel *panel = UmiEducationGtkCreate();
    CHECK(panel != NULL);
    GtkWidget *root = UmiEducationGtkWidget(panel);
    GtkWidget *path = Find(root, "education.storage-path");
    GtkWidget *location = Find(root, "education.storage-location");
    GtkWidget *note = Find(root, "education.note");
    CHECK(GTK_IS_ENTRY(path) && GTK_IS_LABEL(location) && GTK_IS_TEXT_VIEW(note));
    CHECK(gtk_editable_get_text(GTK_EDITABLE(path))[0] == '\0');
    Click(root, "education.open");
    CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(note)));
    const char *mode = argv[1];
    if (strcmp(mode, "retained-browser") == 0)
    {
        GtkWidget *button = g_object_ref(Find(root, "education.storage-existing"));
        CHECK(GTK_IS_BUTTON(button));
        UmiEducationGtkDestroy(panel);
        g_signal_emit_by_name(button, "clicked"); /* No controller remains to open a browser. */
        g_object_unref(button);
        return 0;
    }
    if (strcmp(mode, "unparented-browser") == 0)
    {
        Click(root, "education.storage-new");
        CHECK(gtk_editable_get_text(GTK_EDITABLE(path))[0] == '\0');
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "education.status"))),
                     "in a window") != NULL);
        UmiEducationGtkDestroy(panel);
        return 0;
    }
    char parent[UMI_PATH_CAPACITY], database[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY];
    FixtureDirectory(parent);
    FixturePath(database, parent, "progress caf\xc3\xa9.sqlite");
    /* Probe optional SQLite without changing panel ownership. */
    UmiDataServer *server = NULL;
    UmiEducationWorkspace *workspace = NULL;
    UmiStatus status =
        UmiEducationLocalRecordOpenAt(database, "learner", "Study", &server, &workspace);
    if (status == UMI_STATUS_NOT_IMPLEMENTED)
    {
        UmiEducationGtkDestroy(panel);
        return 77;
    }
    CHECK(status == UMI_STATUS_OK);
    UmiEducationClose(workspace);
    umi_data_server_destroy(server);
    gtk_editable_set_text(GTK_EDITABLE(path), database);
    Click(root, "education.open");
    CHECK(gtk_text_view_get_editable(GTK_TEXT_VIEW(note)));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(location)), database) != NULL);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(note));
    gtk_text_buffer_set_text(buffer, "A note at my chosen location", -1);
    if (strcmp(mode, "dirty") == 0)
    {
        FixturePath(other, parent, "next.sqlite");
        gtk_editable_set_text(GTK_EDITABLE(path), other);
        Click(root, "education.open");
        CHECK(!umi_fs_exists(other));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(location)), database) != NULL);
    }
    else
        CHECK(strcmp(mode, "save") == 0 || strcmp(mode, "failed-open") == 0);
    Click(root, "education.save-note");
    if (strcmp(mode, "failed-open") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(path), "relative.sqlite");
        Click(root, "education.open");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(location)), database) != NULL);
        CHECK(gtk_text_view_get_editable(GTK_TEXT_VIEW(note)));
    }
    UmiEducationGtkDestroy(panel);
    server = NULL;
    workspace = NULL;
    CHECK(UmiEducationLocalRecordOpenAt(database, "learner", "Study", &server, &workspace) ==
          UMI_STATUS_OK);
    UmiEducationProgress progress;
    CHECK(UmiEducationProgressRead(workspace, UmiEducationLessonAt(0U)->id, &progress) ==
          UMI_STATUS_OK);
    CHECK(strcmp(progress.note, "A note at my chosen location") == 0);
    UmiEducationClose(workspace);
    umi_data_server_destroy(server);
    return 0;
}
#include "../native_process/utf8_entry.inc"
