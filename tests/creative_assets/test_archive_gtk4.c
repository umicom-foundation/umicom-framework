/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_assets/test_archive_gtk4.c
 * PURPOSE: Exercise native archive controls across panel lifetimes while retaining previous captures on refused loads.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/ui/gtk4/creative_assets.h"
#include "umicom/creative_workspace/asset_archive.h"
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
static void Field(GtkWidget *root, const char *id, const char *text)
{
    GtkWidget *field = Find(root, id);
    CHECK(field != NULL);
    gtk_editable_set_text(GTK_EDITABLE(field), text);
}
static void Click(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id);
    CHECK(button != NULL);
    g_signal_emit_by_name(button, "clicked");
}
static void Wait(GtkWidget *root)
{
    gint64 deadline = g_get_monotonic_time() + 5000000;
    UmiCreativeAssetInfo info;
    while (UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_BUSY)
    {
        CHECK(g_get_monotonic_time() < deadline);
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000U);
    }
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"reopen",      "corrupt-retains", "cancelled-retains", "save-existing",
                           "busy",        "no-auto-open",    "raw-copy",          "invalid-path",
                           "independent", "retained-control"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(cases[i], mode) == 0);
    if (known != 1U)
        return 2;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    GtkWidget *root = UmiCreativeAssetsGtkCreate();
    g_object_ref_sink(root);
    CHECK(!gtk_widget_get_sensitive(Find(root, "creative.assets.archive-save")));
    const unsigned char payload[] = {0U, 255U, 10U, 128U};
    CHECK(UmiCreativeAssetsGtkCapture(root, "caf\xc3\xa9", UMI_CREATIVE_ASSET_IMAGE, payload,
                                      sizeof(payload)) == UMI_STATUS_OK);
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], copy[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(path, directory, "capture.umiasset");
    FixturePath(copy, directory, "copy.bin");
    Field(root, "creative.assets.archive-path", path);
    if (strcmp(mode, "save-existing") == 0)
    {
        CHECK(UmiRootedFileWrite(directory, "capture.umiasset", "keep", 4U) == UMI_STATUS_OK);
        Click(root, "creative.assets.archive-save");
        Wait(root);
        size_t actual_size = 0U;
        unsigned char *actual = FixtureRead(path, &actual_size);
        CHECK(actual_size == 4U && memcmp(actual, "keep", 4U) == 0);
        free(actual);
        UmiCreativeAssetInfo info;
        CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK &&
              info.byte_count == sizeof(payload));
    }
    else if (strcmp(mode, "retained-control") == 0)
    {
        GtkWidget *button = g_object_ref(Find(root, "creative.assets.archive-save"));
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
    else
    {
        Click(root, "creative.assets.archive-save");
        Wait(root);
        UmiCreativeAsset *saved = NULL;
        CHECK(UmiCreativeAssetArchiveLoad(path, sizeof(payload), NULL, &saved) == UMI_STATUS_OK);
        const void *view = NULL;
        size_t size = 0U;
        CHECK(UmiCreativeAssetBytes(saved, &view, &size) == UMI_STATUS_OK && size == sizeof(payload) &&
              memcmp(view, payload, size) == 0);
        UmiCreativeAssetDestroy(saved);
        /* Reopen from a new panel to prove persistence is not an in-memory
         * handoff from the original panel or its worker. */
        g_object_unref(root);
        root = UmiCreativeAssetsGtkCreate();
        g_object_ref_sink(root);
        CHECK(UmiCreativeAssetsGtkCapture(root, "previous", UMI_CREATIVE_ASSET_DOCUMENT, "old", 3U) ==
              UMI_STATUS_OK);
        Field(root, "creative.assets.archive-path",
              strcmp(mode, "invalid-path") == 0 ? "relative.umiasset" : path);
        UmiCreativeAssetInfo info;
        if (strcmp(mode, "no-auto-open") == 0)
        {
            while (g_main_context_iteration(NULL, FALSE))
            {
            }
            CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK &&
                  strcmp(info.label, "previous") == 0);
        }
        else
        {
            if (strcmp(mode, "corrupt-retains") == 0)
                CHECK(UmiRootedFileWrite(directory, "capture.umiasset", "bad", 3U) == UMI_STATUS_OK);
            Click(root, "creative.assets.archive-open");
            CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_BUSY);
            if (strcmp(mode, "cancelled-retains") == 0)
                Click(root, "creative.assets.cancel");
            if (strcmp(mode, "busy") == 0)
            {
                Click(root, "creative.assets.archive-save");
                Click(root, "creative.assets.clear");
                CHECK(UmiCreativeAssetsGtkCapture(root, "blocked", UMI_CREATIVE_ASSET_BINARY, "x", 1U) ==
                      UMI_STATUS_BUSY);
            }
            Wait(root);
            CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK);
            if (strcmp(mode, "corrupt-retains") == 0 || strcmp(mode, "cancelled-retains") == 0 ||
                strcmp(mode, "invalid-path") == 0)
                CHECK(strcmp(info.label, "previous") == 0 && info.byte_count == 3U);
            else
            {
                CHECK(strcmp(info.label, "caf\xc3\xa9") == 0 &&
                      info.declared_kind == UMI_CREATIVE_ASSET_IMAGE && info.source_path[0] == '\0');
                if (strcmp(mode, "independent") == 0)
                {
                    GtkWidget *second = UmiCreativeAssetsGtkCreate();
                    g_object_ref_sink(second);
                    CHECK(UmiCreativeAssetsGtkInspect(second, &info) == UMI_STATUS_NOT_FOUND);
                    g_object_unref(second);
                }
                if (strcmp(mode, "raw-copy") == 0)
                {
                    Field(root, "creative.assets.destination", copy);
                    Click(root, "creative.assets.write");
                    Wait(root);
                    size_t actual_size = 0U;
                    unsigned char *actual = FixtureRead(copy, &actual_size);
                    CHECK(actual_size == sizeof(payload) && memcmp(actual, payload, actual_size) == 0);
                    free(actual);
                }
            }
        }
    }
    if (root != NULL)
        g_object_unref(root);
    return 0;
}
