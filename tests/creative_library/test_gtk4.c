/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_library/test_gtk4.c
 * PURPOSE: Exercise library controls, explicit replacement and worker completion across native widget lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/ui/gtk4/creative_library.h"
#include "umicom/ui/gtk4/creative_assets.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
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
static void Field(GtkWidget *root, const char *id, const char *value)
{
    GtkWidget *field = Find(root, id);
    CHECK(field != NULL);
    gtk_editable_set_text(GTK_EDITABLE(field), value);
}
static void Click(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id);
    CHECK(button != NULL);
    g_signal_emit_by_name(button, "clicked");
}
static UmiCreativeAssetLibraryInfo Info(GtkWidget *root)
{
    UmiCreativeAssetLibraryInfo info;
    CHECK(UmiCreativeAssetLibraryGtkInspect(root, &info) == UMI_STATUS_OK);
    return info;
}
static void Wait(GtkWidget *root)
{
    gint64 deadline = g_get_monotonic_time() + 10000000;
    UmiCreativeAssetLibraryInfo info;
    while (UmiCreativeAssetLibraryGtkInspect(root, &info) == UMI_STATUS_BUSY)
    {
        CHECK(g_get_monotonic_time() < deadline);
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000U);
    }
}
static void Import(GtkWidget *root, const char *path, const char *id)
{
    Field(root, "creative.library.source", path);
    Field(root, "creative.library.id", id);
    Field(root, "creative.library.label", id);
    Click(root, "creative.library.import");
    Wait(root);
}
int main(int argc, char **argv)
{
    const char *cases[] = {"import",
                           "duplicate",
                           "reorder",
                           "remove-undo",
                           "save-reopen",
                           "corrupt-retains",
                           "cancel-import",
                           "cancel-open",
                           "busy",
                           "open-approval",
                           "approval-invalidation",
                           "export",
                           "existing-export",
                           "editor-copy",
                           "independent",
                           "retained-control",
                           "close-busy",
                           "title"};
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    GtkWidget *editor = UmiCreativeAssetsGtkCreate();
    g_object_ref_sink(editor);
    GtkWidget *root = UmiCreativeAssetLibraryGtkCreate(editor);
    CHECK(root != NULL);
    g_object_ref_sink(root);
    char directory[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY],
        destination[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(source, directory, "asset.bin");
    FixturePath(path, directory, "assets.umilibrary");
    FixturePath(destination, directory, "export.bin");
    CHECK(UmiRootedFileWrite(directory, "asset.bin", "a\0bc", 4U) == UMI_STATUS_OK);
    CHECK(Info(root).asset_count == 0U);
    Import(root, source, "first");
    CHECK(Info(root).asset_count == 1U && Info(root).byte_count == 4U);
    Field(root, "creative.library.archive", path);
    Field(root, "creative.library.destination", destination);
    if (strcmp(mode, "import") == 0)
    {
        UmiCreativeAssetLibraryEntry entry;
        CHECK(UmiCreativeAssetLibraryGtkAt(root, 0U, &entry) == UMI_STATUS_OK &&
              strcmp(entry.id, "first") == 0 && entry.asset.byte_count == 4U);
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        Import(root, source, "first");
        CHECK(Info(root).asset_count == 1U && Info(root).byte_count == 4U);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "creative.library.status"))), "retained") !=
              NULL);
    }
    else if (strcmp(mode, "reorder") == 0)
    {
        Import(root, source, "second");
        Click(root, "creative.library.up");
        UmiCreativeAssetLibraryEntry entry;
        CHECK(UmiCreativeAssetLibraryGtkAt(root, 0U, &entry) == UMI_STATUS_OK &&
              strcmp(entry.id, "second") == 0);
        Click(root, "creative.library.down");
        CHECK(UmiCreativeAssetLibraryGtkAt(root, 1U, &entry) == UMI_STATUS_OK &&
              strcmp(entry.id, "second") == 0);
    }
    else if (strcmp(mode, "remove-undo") == 0)
    {
        Click(root, "creative.library.remove");
        CHECK(Info(root).asset_count == 0U);
        Click(root, "creative.library.undo");
        CHECK(Info(root).asset_count == 1U && Info(root).byte_count == 4U);
        CHECK(umi_fs_exists(source));
    }
    else if (strcmp(mode, "save-reopen") == 0 || strcmp(mode, "corrupt-retains") == 0 ||
             strcmp(mode, "cancel-open") == 0)
    {
        Click(root, "creative.library.save");
        Wait(root);
        CHECK(umi_fs_exists(path));
        Import(root, source, "second");
        CHECK(Info(root).asset_count == 2U);
        if (strcmp(mode, "corrupt-retains") == 0)
            CHECK(UmiRootedFileWrite(directory, "assets.umilibrary", "corrupt", 7U) == UMI_STATUS_OK);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "creative.library.confirm")), TRUE);
        Click(root, "creative.library.open");
        if (strcmp(mode, "cancel-open") == 0)
            Click(root, "creative.library.cancel");
        Wait(root);
        CHECK(Info(root).asset_count == (strcmp(mode, "save-reopen") == 0 ? 1U : 2U));
    }
    else if (strcmp(mode, "cancel-import") == 0 || strcmp(mode, "busy") == 0 ||
             strcmp(mode, "close-busy") == 0)
    {
        Field(root, "creative.library.id", "second");
        Click(root, "creative.library.import");
        UmiCreativeAssetLibraryInfo info;
        CHECK(UmiCreativeAssetLibraryGtkInspect(root, &info) == UMI_STATUS_BUSY);
        if (strcmp(mode, "cancel-import") == 0)
        {
            Click(root, "creative.library.cancel");
            Wait(root);
            CHECK(Info(root).asset_count == 1U);
        }
        else if (strcmp(mode, "busy") == 0)
        {
            Click(root, "creative.library.remove");
            Click(root, "creative.library.import");
            Wait(root);
            CHECK(Info(root).asset_count == 2U);
        }
        else
        {
            GWeakRef weak;
            g_weak_ref_init(&weak, G_OBJECT(root));
            g_object_unref(root);
            root = NULL;
            GObject *remaining = g_weak_ref_get(&weak);
            CHECK(remaining == NULL);
            g_weak_ref_clear(&weak);
            /* Let a queued cancellation completion retire its owned data. It
             * must not dereference the closed widget or send bytes elsewhere. */
            gint64 deadline = g_get_monotonic_time() + 250000;
            while (g_get_monotonic_time() < deadline)
            {
                while (g_main_context_iteration(NULL, FALSE))
                {
                }
                g_usleep(1000U);
            }
            CHECK(!umi_fs_exists(path) && !umi_fs_exists(destination));
        }
    }
    else if (strcmp(mode, "open-approval") == 0 || strcmp(mode, "approval-invalidation") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(Find(root, "creative.library.open")));
        Click(root, "creative.library.open");
        CHECK(Info(root).asset_count == 1U);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "creative.library.confirm")), TRUE);
        CHECK(gtk_widget_get_sensitive(Find(root, "creative.library.open")));
        if (strcmp(mode, "approval-invalidation") == 0)
        {
            Field(root, "creative.library.archive", source);
            CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, "creative.library.confirm"))));
            CHECK(!gtk_widget_get_sensitive(Find(root, "creative.library.open")));
        }
    }
    else if (strcmp(mode, "export") == 0 || strcmp(mode, "existing-export") == 0)
    {
        if (strcmp(mode, "existing-export") == 0)
            CHECK(UmiRootedFileWrite(directory, "export.bin", "keep", 4U) == UMI_STATUS_OK);
        Click(root, "creative.library.export");
        Wait(root);
        size_t size = 0U;
        unsigned char *bytes = FixtureRead(destination, &size);
        CHECK(size == 4U && memcmp(bytes, strcmp(mode, "export") == 0 ? "a\0bc" : "keep", 4U) == 0);
        free(bytes);
        CHECK(Info(root).asset_count == 1U);
    }
    else if (strcmp(mode, "editor-copy") == 0)
    {
        Click(root, "creative.library.edit");
        Wait(root);
        UmiCreativeAssetInfo info;
        CHECK(UmiCreativeAssetsGtkInspect(editor, &info) == UMI_STATUS_OK && info.byte_count == 4U &&
              strcmp(info.label, "first") == 0);
        Click(editor, "creative.assets.clear");
        CHECK(Info(root).asset_count == 1U);
    }
    else if (strcmp(mode, "independent") == 0)
    {
        GtkWidget *other = UmiCreativeAssetLibraryGtkCreate(NULL);
        g_object_ref_sink(other);
        CHECK(Info(other).asset_count == 0U && Info(root).asset_count == 1U);
        g_object_unref(other);
    }
    else if (strcmp(mode, "retained-control") == 0)
    {
        GtkWidget *button = g_object_ref(Find(root, "creative.library.save"));
        GWeakRef weak;
        g_weak_ref_init(&weak, G_OBJECT(root));
        g_object_unref(root);
        root = NULL;
        GObject *remaining = g_weak_ref_get(&weak);
        CHECK(remaining == NULL);
        g_weak_ref_clear(&weak);
        g_signal_emit_by_name(button, "clicked");
        g_object_unref(button);
        CHECK(!umi_fs_exists(path));
    }
    else if (strcmp(mode, "title") == 0)
    {
        Field(root, "creative.library.title", "New title");
        Click(root, "creative.library.rename");
        CHECK(strcmp(Info(root).title, "New title") == 0);
        Field(root, "creative.library.title", "");
        Click(root, "creative.library.rename");
        CHECK(strcmp(Info(root).title, "New title") == 0);
    }
    if (root != NULL)
        g_object_unref(root);
    g_object_unref(editor);
    return 0;
}
