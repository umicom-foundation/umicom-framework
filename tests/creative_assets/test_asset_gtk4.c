/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_assets/test_asset_gtk4.c
 * PURPOSE: Exercise the real shared asset controls, captured bytes, asynchronous ownership and creative product composition.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/ui/gtk4/creative_assets.h"
#include "umicom/ui/gtk4/creative_workspace.h"
#include "umicom/platform/rooted_files.h"
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
               *cases[] = {
                   "capture",       "invalid-capture", "independent", "clear",      "retained-control",
                   "load",          "failed-load",     "busy",        "cancel",     "copy",
                   "pending-close", "media",           "music",       "web-studio", "mobile-studio"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    if (strcmp(mode, "media") == 0 || strcmp(mode, "music") == 0 || strcmp(mode, "web-studio") == 0 ||
        strcmp(mode, "mobile-studio") == 0)
    {
        UmiCreativeGtkPanel *panel = NULL;
        CHECK(UmiCreativeGtkPanelCreate(NULL, mode, &panel) == UMI_STATUS_OK);
        CHECK(Find(UmiCreativeGtkPanelWidget(panel), "creative.assets") != NULL);
        UmiCreativeGtkPanelDestroy(panel);
        return 0;
    }
    GtkWidget *root = UmiCreativeAssetsGtkCreate();
    g_object_ref_sink(root);
    UmiCreativeAssetInfo info;
    CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_NOT_FOUND);
    CHECK(!gtk_widget_get_sensitive(Find(root, "creative.assets.write")));
    CHECK(UmiCreativeAssetsGtkCapture(root, "old", UMI_CREATIVE_ASSET_DOCUMENT, "old", 3U) == UMI_STATUS_OK);
    CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK && info.byte_count == 3U);
    if (strcmp(mode, "invalid-capture") == 0)
    {
        CHECK(UmiCreativeAssetsGtkCapture(root, "", UMI_CREATIVE_ASSET_DOCUMENT, "new", 3U) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK && strcmp(info.label, "old") == 0);
    }
    else if (strcmp(mode, "independent") == 0)
    {
        GtkWidget *second = UmiCreativeAssetsGtkCreate();
        g_object_ref_sink(second);
        CHECK(UmiCreativeAssetsGtkInspect(second, &info) == UMI_STATUS_NOT_FOUND);
        g_object_unref(second);
    }
    else if (strcmp(mode, "clear") == 0)
    {
        Click(root, "creative.assets.clear");
        CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_NOT_FOUND);
        CHECK(!gtk_widget_get_sensitive(Find(root, "creative.assets.write")));
    }
    else if (strcmp(mode, "retained-control") == 0)
    {
        GtkWidget *button = g_object_ref(Find(root, "creative.assets.clear"));
        g_object_unref(root);
        root = NULL;
        g_signal_emit_by_name(button, "clicked");
        g_object_unref(button);
    }
    else if (strcmp(mode, "capture") != 0)
    {
        char directory[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY];
        FixtureDirectory(directory);
        FixturePath(source, directory, "source.bin");
        FixturePath(destination, directory, "new.bin");
        const unsigned char bytes[] = {0U, 255U, 10U, 0U, 128U};
        CHECK(UmiRootedFileWrite(directory, "source.bin", bytes, sizeof(bytes)) == UMI_STATUS_OK);
        Field(root, "creative.assets.source", strcmp(mode, "failed-load") == 0 ? destination : source);
        Field(root, "creative.assets.label", "new capture");
        Field(root, "creative.assets.destination", destination);
        Click(root, "creative.assets.load");
        CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_BUSY);
        if (strcmp(mode, "busy") == 0)
        {
            CHECK(UmiCreativeAssetsGtkCapture(root, "blocked", UMI_CREATIVE_ASSET_BINARY, "x", 1U) ==
                  UMI_STATUS_BUSY);
            Click(root, "creative.assets.load");
            Click(root, "creative.assets.clear");
        }
        if (strcmp(mode, "cancel") == 0)
            Click(root, "creative.assets.cancel");
        if (strcmp(mode, "pending-close") == 0)
        {
            GWeakRef weak;
            g_weak_ref_init(&weak, G_OBJECT(root));
            g_object_unref(root);
            root = NULL;
            GObject *remaining = g_weak_ref_get(&weak);
            CHECK(remaining == NULL);
            g_weak_ref_clear(&weak);
            /* Drain late worker delivery without a surviving widget target. */
            gint64 until = g_get_monotonic_time() + 100000;
            while (g_get_monotonic_time() < until)
            {
                while (g_main_context_iteration(NULL, FALSE))
                {
                }
                g_usleep(1000U);
            }
        }
        else
        {
            Wait(root);
            CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK);
            if (strcmp(mode, "failed-load") == 0 || strcmp(mode, "cancel") == 0)
                CHECK(strcmp(info.label, "old") == 0 && info.byte_count == 3U);
            else
                CHECK(strcmp(info.label, "new capture") == 0 && info.byte_count == sizeof(bytes));
            if (strcmp(mode, "copy") == 0)
            {
                CHECK(UmiRootedFileWrite(directory, "source.bin", "changed", 7U) == UMI_STATUS_OK);
                Click(root, "creative.assets.write");
                Wait(root);
                CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK &&
                      info.byte_count == sizeof(bytes));
                size_t size = 0U;
                unsigned char *actual = FixtureRead(destination, &size);
                CHECK(size == sizeof(bytes) && memcmp(actual, bytes, size) == 0);
                free(actual);
            }
        }
    }
    if (root != NULL)
        g_object_unref(root);
    return 0;
}
