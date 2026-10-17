/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education_workspace/test_project_export_async.c
 * PURPOSE: Exercise captured course export, destination preparation and closure.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "../build_log/fixture.h"
#include "../../adapters/gtk4/education_workspace_private.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"

typedef struct ExportProbe {
    UmiEducationGtkPanel *panel;
    char destination[UMI_PATH_CAPACITY];
    unsigned adopted, released;
} ExportProbe;

static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    /* Collapsed destinations still own their body without a mounted child. */
    if (GTK_IS_EXPANDER(root)) {
        GtkWidget *body = gtk_expander_get_child(GTK_EXPANDER(root));
        if (body != NULL && gtk_widget_get_parent(body) == NULL) {
            GtkWidget *found = Find(body, id);
            if (found != NULL) return found;
        }
    }
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}
static void Released(gpointer data) { ++((ExportProbe *)data)->released; }
static void Adopt(const UmiEducationProjectWorkflow *workflow, void *data)
{
    ExportProbe *probe = data;
    CHECK(strcmp(workflow->project.root, probe->destination) == 0);
    CHECK(strcmp(workflow->project.template_id, "notes") == 0);
    CHECK(!workflow->project.trusted);
    ++probe->adopted;
}
static void Pump(void)
{
    while (g_main_context_iteration(NULL, FALSE)) {}
    g_usleep(1000U);
}
static void WaitReady(UmiEducationGtkPanel *panel)
{
    gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    while (panel->projectExporting && g_get_monotonic_time() < deadline) Pump();
    CHECK(!panel->projectExporting);
}
static void WaitReleased(ExportProbe *probe)
{
    gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    while (!probe->released && g_get_monotonic_time() < deadline) Pump();
    CHECK(probe->released == 1U);
}
static void CloseDuringPublication(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object;
    (void)spec;
    ExportProbe *probe = data;
    UmiEducationGtkPanel *panel = probe->panel;
    probe->panel = NULL;
    UmiEducationGtkDestroy(panel);
}
static void CheckCanonical(const char *destination)
{
    for (size_t index = 0U; index < UmiEducationProjectFileCount("notes"); ++index) {
        UmiEducationProjectFile resource;
        CHECK(UmiEducationProjectFileAt("notes", index, &resource) == UMI_STATUS_OK);
        char path[UMI_PATH_CAPACITY];
        FixturePath(path, destination, resource.name);
        size_t count = 0U;
        unsigned char *bytes = FixtureRead(path, &count);
        CHECK(count == resource.size && memcmp(bytes, resource.bytes, count) == 0);
        free(bytes);
    }
}
static int NativeClose(ExportProbe *probe)
{
    guint before = g_list_model_get_n_items(gtk_window_get_toplevels());
    CHECK(UmiEducationGtkOpenWithProjectOpener(NULL, Adopt, probe, Released) == UMI_STATUS_OK);
    CHECK(g_list_model_get_n_items(gtk_window_get_toplevels()) == before + 1U);
    GtkWindow *window = NULL;
    for (guint index = 0U; index < before + 1U; ++index) {
        GtkWindow *candidate = g_list_model_get_item(gtk_window_get_toplevels(), index);
        if (Find(GTK_WIDGET(candidate), "education.project-path") != NULL) {
            window = candidate;
            break;
        }
        g_object_unref(candidate);
    }
    CHECK(window != NULL);
    GtkWidget *path = Find(GTK_WIDGET(window), "education.project-path");
    GtkWidget *export = g_object_ref(Find(GTK_WIDGET(window), "education.export-project"));
    gtk_editable_set_text(GTK_EDITABLE(path), probe->destination);
    g_signal_emit_by_name(export, "clicked");
    /* The extra toplevel reference deliberately postpones finalisation. Removal
     * must retire its panel now rather than wait for this reference to go away. */
    gtk_window_destroy(window);
    g_signal_emit_by_name(export, "clicked");
    WaitReleased(probe);
    CHECK(probe->adopted == 0U);
    CheckCanonical(probe->destination);
    g_object_unref(export);
    g_object_unref(window);
    return 0;
}
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 2);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check()) return 77;
    ExportProbe probe = {0};
    char parent[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY];
    FixtureDirectory(parent);
    FixturePath(probe.destination, parent, "course caf\xc3\xa9");
    FixturePath(other, parent, "other course");
    bool missingParent = strcmp(argv[1], "missing-parent") == 0;
    if (missingParent) {
        FixturePath(other, parent, "missing parent");
        FixturePath(probe.destination, other, "course");
    }
    if (strcmp(argv[1], "native-close") == 0) return NativeClose(&probe);
    probe.panel = UmiEducationGtkCreate();
    CHECK(probe.panel != NULL);
    UmiEducationGtkPanel *panel = probe.panel;
    CHECK(UmiEducationGtkSetProjectOpener(panel, Adopt, &probe, Released) == UMI_STATUS_OK);
    GtkWidget *root = UmiEducationGtkWidget(panel);
    GtkWidget *export = g_object_ref(Find(root, "education.export-project"));
    GtkWidget *open = g_object_ref(Find(root, "education.open-project"));
    if (strcmp(argv[1], "destination") == 0) {
        /* FixturePath intentionally accepts mixed separators. Destination
         * preparation publishes a normalized native path, so compare against
         * that spelling while keeping the same Unicode folder and ownership.
         * This also makes the later export/adoption checks use one path form. */
        char normalized[UMI_PATH_CAPACITY];
        CHECK(umi_path_normalise(probe.destination, normalized, sizeof normalized) == UMI_STATUS_OK);
        memcpy(probe.destination, normalized, strlen(normalized) + 1U);
        GtkWidget *folder = Find(root, "education.project-parent");
        GtkWidget *name = Find(root, "education.project-name");
        GtkWidget *use = Find(root, "education.project-destination");
        CHECK(GTK_IS_ENTRY(folder) && GTK_IS_ENTRY(name) && GTK_IS_BUTTON(use));
        gtk_editable_set_text(GTK_EDITABLE(folder), parent);
        gtk_editable_set_text(GTK_EDITABLE(name), "course caf\xc3\xa9");
        g_signal_emit_by_name(use, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->projectPath)),
            probe.destination) == 0 && !umi_fs_exists(probe.destination));
        /* A rejected name keeps the previously reviewed destination untouched. */
        gtk_editable_set_text(GTK_EDITABLE(name), "../escape");
        g_signal_emit_by_name(use, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->projectPath)),
            probe.destination) == 0);
    } else {
        gtk_editable_set_text(GTK_EDITABLE(panel->projectPath), probe.destination);
    }
    bool existing = strcmp(argv[1], "existing") == 0;
    char sentinel[UMI_PATH_CAPACITY];
    if (existing) {
        CHECK(umi_fs_make_directories(probe.destination) == UMI_STATUS_OK);
        FixturePath(sentinel, probe.destination, "keep.txt");
        CHECK(umi_fs_write_text(sentinel, "original work") == UMI_STATUS_OK);
    }
    g_signal_emit_by_name(export, "clicked");
    CHECK(panel->projectExporting && !gtk_widget_get_sensitive(open));
    if (strcmp(argv[1], "captured") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(panel->projectPath), other);
        for (size_t index = 0U; index < UMI_EDUCATION_LESSONS; ++index) {
            if (strcmp(UmiEducationLessonAt(index)->projectId, "assembly") == 0) {
                gtk_drop_down_set_selected(panel->lessonSelect, (guint)index);
                break;
            }
        }
        /* Signal emission deliberately bypasses button sensitivity. The busy
         * guard must still refuse a second destination before completion. */
        g_signal_emit_by_name(export, "clicked");
        g_signal_emit_by_name(open, "clicked");
        CHECK(probe.adopted == 0U);
    }
    bool closing = strcmp(argv[1], "close") == 0;
    bool publication = strcmp(argv[1], "publication-close") == 0;
    if (closing) {
        UmiEducationGtkDestroy(panel);
        probe.panel = NULL;
        g_signal_emit_by_name(export, "clicked");
        g_signal_emit_by_name(open, "clicked");
        WaitReleased(&probe);
        CheckCanonical(probe.destination);
    } else if (publication) {
        g_signal_connect(panel->status, "notify::label", G_CALLBACK(CloseDuringPublication), &probe);
        WaitReleased(&probe);
        CheckCanonical(probe.destination);
    } else {
        CHECK(existing || missingParent || strcmp(argv[1], "destination") == 0 || strcmp(argv[1], "captured") == 0);
        WaitReady(panel);
        CHECK(!umi_fs_exists(other));
        if (missingParent) {
            CHECK(!panel->exportedProjectReady && !gtk_widget_get_sensitive(open));
            CHECK(!umi_fs_exists(probe.destination));
            CHECK(strstr(gtk_label_get_text(panel->exportedProjectLabel), "0 files") != NULL);
        } else if (existing) {
            CHECK(!panel->exportedProjectReady);
            CHECK(!gtk_widget_get_sensitive(open));
            size_t size = 0U;
            unsigned char *bytes = FixtureRead(sentinel, &size);
            CHECK(size == strlen("original work") && memcmp(bytes, "original work", size) == 0);
            free(bytes);
        } else {
            CheckCanonical(probe.destination);
            g_signal_emit_by_name(open, "clicked");
            CHECK(probe.adopted == 1U);
        }
        UmiEducationGtkDestroy(panel);
        probe.panel = NULL;
    }
    CHECK(probe.released == 1U);
    g_object_unref(open);
    g_object_unref(export);
    return 0;
}
#include "../native_process/utf8_entry.inc"
