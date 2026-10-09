/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education_workspace/test_project_workflow_gtk4.c
 * PURPOSE: Exercise explicit lesson export and IDE adoption through the real GTK panel.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/platform/filesystem.h"
#include "umicom/ui/gtk4/education_workspace.h"

typedef struct AdoptionProbe
{
    UmiEducationGtkPanel *panel;
    GtkWidget *open;
    const char *mode;
    char expected[UMI_PATH_CAPACITY];
    unsigned calls, released;
} AdoptionProbe;

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
static void ReleaseHost(gpointer context)
{
    AdoptionProbe *probe = context;
    ++probe->released;
}
static void Adopt(const UmiEducationProjectWorkflow *project, void *context)
{
    AdoptionProbe *probe = context;
    ++probe->calls;
    CHECK(probe->calls == 1U && probe->released == 0U);
    CHECK(strcmp(project->project.root, probe->expected) == 0);
    CHECK(strcmp(project->build.source_directory, probe->expected) == 0);
    CHECK(strcmp(project->project.entry_point, "main.c") == 0);
    CHECK(project->project.trusted == 0);
    if (strcmp(probe->mode, "recursive") == 0)
        g_signal_emit_by_name(probe->open, "clicked");
    if (strcmp(probe->mode, "close-callback") == 0)
    {
        UmiEducationGtkDestroy(probe->panel);
        probe->panel = NULL;
        /* Host data and copied project metadata survive the closing callback.
         * They are released only when the outer GTK action has returned. */
        CHECK(probe->released == 0U);
        CHECK(strcmp(project->project.root, probe->expected) == 0);
    }
}
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 2);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    AdoptionProbe probe = {0};
    probe.mode = argv[1];
    probe.panel = UmiEducationGtkCreate();
    CHECK(probe.panel != NULL);
    GtkWidget *root = UmiEducationGtkWidget(probe.panel);
    GtkWidget *export = Find(root, "education.export-project");
    GtkWidget *path = Find(root, "education.project-path");
    probe.open = Find(root, "education.open-project");
    CHECK(GTK_IS_BUTTON(export) && GTK_IS_ENTRY(path) && GTK_IS_BUTTON(probe.open));
    g_object_ref(probe.open);
    CHECK(!gtk_widget_get_sensitive(probe.open) && !gtk_widget_get_visible(probe.open));
    bool unbound = strcmp(probe.mode, "unbound") == 0;
    if (!unbound)
    {
        CHECK(UmiEducationGtkSetProjectOpener(probe.panel, Adopt, &probe, ReleaseHost) ==
              UMI_STATUS_OK);
        CHECK(gtk_widget_get_visible(probe.open));
        /* A second host cannot replace the first binding or steal its lifetime. */
        CHECK(UmiEducationGtkSetProjectOpener(probe.panel, Adopt, &probe, ReleaseHost) ==
              UMI_STATUS_ALREADY_EXISTS);
        CHECK(probe.released == 0U);
    }
    g_signal_emit_by_name(probe.open, "clicked");
    CHECK(probe.calls == 0U);
    char parent[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY];
    FixtureDirectory(parent);
    FixturePath(probe.expected, parent, "project caf\xc3\xa9");
    gtk_editable_set_text(GTK_EDITABLE(path), probe.expected);
    g_signal_emit_by_name(export, "clicked");
    CHECK(umi_fs_is_directory(probe.expected));
    CHECK(gtk_widget_get_sensitive(probe.open) == !unbound);
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(Find(root, "education.exported-project"))),
                 probe.expected) == 0);
    if (strcmp(probe.mode, "failed-export") == 0)
    {
        /* Exporting again must not overwrite the first project, and must not
         * leave Open enabled for a partially failed replacement operation. */
        g_signal_emit_by_name(export, "clicked");
        CHECK(!gtk_widget_get_sensitive(probe.open));
        g_signal_emit_by_name(probe.open, "clicked");
        CHECK(probe.calls == 0U && umi_fs_is_directory(probe.expected));
    }
    else if (strcmp(probe.mode, "captured-path") == 0)
    {
        FixturePath(other, parent, "different destination");
        gtk_editable_set_text(GTK_EDITABLE(path), other);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "education.lesson")), 1U);
        g_signal_emit_by_name(probe.open, "clicked");
        CHECK(probe.calls == 1U && !umi_fs_exists(other));
    }
    else if (strcmp(probe.mode, "retained") == 0)
    {
        UmiEducationGtkDestroy(probe.panel);
        probe.panel = NULL;
        CHECK(probe.released == 1U);
        g_signal_emit_by_name(probe.open, "clicked");
        CHECK(probe.calls == 0U);
    }
    else
    {
        CHECK(unbound || strcmp(probe.mode, "success") == 0 ||
              strcmp(probe.mode, "recursive") == 0 || strcmp(probe.mode, "close-callback") == 0);
        g_signal_emit_by_name(probe.open, "clicked");
        CHECK(probe.calls == (unbound ? 0U : 1U));
    }
    if (probe.panel != NULL)
        UmiEducationGtkDestroy(probe.panel);
    CHECK(probe.released == (unbound ? 0U : 1U));
    g_object_unref(probe.open);
    return 0;
}
#include "../native_process/utf8_entry.inc"
