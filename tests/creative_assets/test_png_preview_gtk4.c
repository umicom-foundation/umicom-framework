/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_assets/test_png_preview_gtk4.c
 * PURPOSE: Exercise explicit PNG preview, captured-byte ownership and stale-preview retirement in the shared asset controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../media/png_fixture.h"
#include "umicom/ui/gtk4/creative_assets.h"
#include "umicom/creative_workspace/asset_archive.h"
#include "umicom/media/png_image.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
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
static GdkPaintable *Paintable(GtkWidget *root)
{
    GtkWidget *picture = Find(root, "creative.assets.picture");
    CHECK(picture != NULL);
    return gtk_picture_get_paintable(GTK_PICTURE(picture));
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"preview",       "no-auto-preview", "wrong-label", "bad-image",
                           "retain-cancel", "clear",           "replace",     "failed-capture",
                           "busy",          "archive-preview", "independent", "retained-control"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!UmiMediaPngAvailable() || !gtk_init_check())
        return 77;
    TestPngFixture input = PngFixture("rgba");
    GtkWidget *root = UmiCreativeAssetsGtkCreate();
    g_object_ref_sink(root);
    CHECK(!gtk_widget_get_sensitive(Find(root, "creative.assets.preview")));
    if (strcmp(mode, "archive-preview") == 0)
    {
        UmiCreativeAsset *original = NULL, *restored = NULL;
        unsigned char *archive = NULL;
        size_t size = 0U;
        CHECK(UmiCreativeAssetCapture("archived", UMI_CREATIVE_ASSET_IMAGE, input.bytes, input.size, NULL,
                                      &original) == UMI_STATUS_OK);
        CHECK(UmiCreativeAssetArchiveEncode(original, NULL, &archive, &size) == UMI_STATUS_OK);
        CHECK(UmiCreativeAssetArchiveDecode(archive, size, NULL, &restored) == UMI_STATUS_OK);
        UmiCreativeAssetDestroy(original);
        UmiCreativeAssetArchiveFree(archive);
        const void *bytes = NULL;
        size_t count = 0U;
        CHECK(UmiCreativeAssetBytes(restored, &bytes, &count) == UMI_STATUS_OK);
        CHECK(UmiCreativeAssetsGtkCapture(root, "archived", UMI_CREATIVE_ASSET_IMAGE, bytes, count) ==
              UMI_STATUS_OK);
        UmiCreativeAssetDestroy(restored);
    }
    else
        CHECK(UmiCreativeAssetsGtkCapture(
                  root, strcmp(mode, "wrong-label") == 0 ? "not-an-image.txt" : "image",
                  strcmp(mode, "wrong-label") == 0 ? UMI_CREATIVE_ASSET_DOCUMENT : UMI_CREATIVE_ASSET_IMAGE,
                  strcmp(mode, "bad-image") == 0 ? (const void *)"invalid" : (const void *)input.bytes,
                  strcmp(mode, "bad-image") == 0 ? 7U : input.size) == UMI_STATUS_OK);
    CHECK(Paintable(root) == NULL); /* Capture alone must not invoke a decoder. */
    if (strcmp(mode, "no-auto-preview") == 0)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        CHECK(Paintable(root) == NULL);
    }
    else if (strcmp(mode, "retained-control") == 0)
    {
        GtkWidget *button = g_object_ref(Find(root, "creative.assets.preview"));
        GWeakRef weak;
        g_weak_ref_init(&weak, G_OBJECT(root));
        g_object_unref(root);
        root = NULL;
        GObject *remaining = g_weak_ref_get(&weak);
        CHECK(remaining == NULL);
        g_weak_ref_clear(&weak);
        g_signal_emit_by_name(button, "clicked");
        g_object_unref(button);
    }
    else
    {
        Click(root, "creative.assets.preview");
        UmiCreativeAssetInfo info;
        CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_BUSY);
        if (strcmp(mode, "busy") == 0)
        {
            Click(root, "creative.assets.clear");
            Click(root, "creative.assets.preview");
            CHECK(UmiCreativeAssetsGtkCapture(root, "busy", UMI_CREATIVE_ASSET_BINARY, "x", 1U) ==
                  UMI_STATUS_BUSY);
        }
        Wait(root);
        CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK);
        if (strcmp(mode, "bad-image") == 0)
            CHECK(Paintable(root) == NULL && info.byte_count == 7U);
        else
        {
            GdkPaintable *image = Paintable(root);
            CHECK(image != NULL);
            CHECK(gdk_paintable_get_intrinsic_width(image) == 2 &&
                  gdk_paintable_get_intrinsic_height(image) == 2);
            if (strcmp(mode, "retain-cancel") == 0)
            {
                Click(root, "creative.assets.preview");
                Click(root, "creative.assets.cancel");
                Wait(root);
                CHECK(Paintable(root) == image && UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK &&
                      info.byte_count == input.size);
            }
            if (strcmp(mode, "clear") == 0)
            {
                Click(root, "creative.assets.clear");
                CHECK(Paintable(root) == NULL &&
                      UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_NOT_FOUND);
            }
            if (strcmp(mode, "replace") == 0)
            {
                CHECK(UmiCreativeAssetsGtkCapture(root, "text", UMI_CREATIVE_ASSET_DOCUMENT, "text", 4U) ==
                      UMI_STATUS_OK);
                CHECK(Paintable(root) == NULL);
            }
            if (strcmp(mode, "failed-capture") == 0)
            {
                CHECK(UmiCreativeAssetsGtkCapture(root, "", UMI_CREATIVE_ASSET_DOCUMENT, "text", 4U) ==
                      UMI_STATUS_INVALID_ARGUMENT);
                CHECK(Paintable(root) == image);
            }
            if (strcmp(mode, "independent") == 0)
            {
                GtkWidget *other = UmiCreativeAssetsGtkCreate();
                g_object_ref_sink(other);
                CHECK(Paintable(other) == NULL);
                g_object_unref(other);
            }
        }
    }
    if (root != NULL)
        g_object_unref(root);
    return 0;
}
