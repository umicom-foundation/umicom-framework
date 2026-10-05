/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_assets/test_image_edit_gtk4.c
 * PURPOSE: Exercise crop/orientation controls, PNG exports and source-preserving native asset behavior.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "../media/png_fixture.h"
#include "umicom/ui/gtk4/creative_assets.h"
#include "umicom/media/png_image.h"
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
static void Click(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id);
    CHECK(button != NULL);
    g_signal_emit_by_name(button, "clicked");
}
static void Number(GtkWidget *root, const char *id, double value)
{
    GtkWidget *spin = Find(root, id);
    CHECK(spin != NULL);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), value);
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
static GdkPaintable *Image(GtkWidget *root)
{
    GtkWidget *picture = Find(root, "creative.assets.picture");
    CHECK(picture != NULL);
    return gtk_picture_get_paintable(GTK_PICTURE(picture));
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *cases[] = {"crop",
                                            "rotate",
                                            "crop-rotate",
                                            "reset",
                                            "invalid-crop",
                                            "form-retirement",
                                            "pending-form-change",
                                            "export",
                                            "export-existing",
                                            "raw-copy",
                                            "unchanged-source"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!UmiMediaPngAvailable() || !UmiMediaPngEncoderAvailable() || !gtk_init_check())
        return 77;
    GtkWidget *root = UmiCreativeAssetsGtkCreate();
    g_object_ref_sink(root);
    TestPngFixture input = PngFixture("rgba");
    CHECK(UmiCreativeAssetsGtkCapture(root, "source", UMI_CREATIVE_ASSET_IMAGE, input.bytes, input.size) ==
          UMI_STATUS_OK);
    Number(root, "creative.assets.crop-x", strcmp(mode, "invalid-crop") == 0 ? 3.0 : 1.0);
    Number(root, "creative.assets.crop-width", 1.0);
    GtkWidget *orientation = Find(root, "creative.assets.orientation");
    CHECK(orientation != NULL);
    bool turn = strcmp(mode, "crop") != 0;
    if (turn)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(orientation), 1U);
    if (strcmp(mode, "rotate") == 0)
    {
        Number(root, "creative.assets.crop-x", 0.0);
        Number(root, "creative.assets.crop-width", 0.0);
    }
    if (strcmp(mode, "reset") == 0)
        Click(root, "creative.assets.image-reset");
    Click(root, "creative.assets.preview");
    if (strcmp(mode, "pending-form-change") == 0)
    {
        /* Returning to the same values must not revive an already cancelled
         * preview. The active request retains its cancellation token. */
        Number(root, "creative.assets.crop-x", 0.0);
        Number(root, "creative.assets.crop-x", 1.0);
    }
    Wait(root);
    UmiCreativeAssetInfo info;
    CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK && info.byte_count == input.size &&
          strcmp(info.label, "source") == 0);
    if (strcmp(mode, "invalid-crop") == 0 || strcmp(mode, "pending-form-change") == 0)
        CHECK(Image(root) == NULL);
    else
    {
        GdkPaintable *image = Image(root);
        CHECK(image != NULL);
        int width = strcmp(mode, "crop") == 0 ? 1 : 2;
        int height =
            strcmp(mode, "reset") == 0 || strcmp(mode, "rotate") == 0 || strcmp(mode, "crop") == 0 ? 2 : 1;
        CHECK(gdk_paintable_get_intrinsic_width(image) == width &&
              gdk_paintable_get_intrinsic_height(image) == height);
        if (strcmp(mode, "form-retirement") == 0)
        {
            Number(root, "creative.assets.crop-y", 1.0);
            CHECK(Image(root) == NULL);
        }
        if (strcmp(mode, "export") == 0 || strcmp(mode, "export-existing") == 0 ||
            strcmp(mode, "raw-copy") == 0 || strcmp(mode, "unchanged-source") == 0)
        {
            char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
            FixtureDirectory(directory);
            FixturePath(path, directory, "image.png");
            GtkWidget *destination = Find(root, "creative.assets.destination");
            CHECK(destination != NULL);
            gtk_editable_set_text(GTK_EDITABLE(destination), path);
            if (strcmp(mode, "export-existing") == 0)
                CHECK(UmiRootedFileWrite(directory, "image.png", "keep", 4U) == UMI_STATUS_OK);
            Click(root,
                  strcmp(mode, "raw-copy") == 0 ? "creative.assets.write" : "creative.assets.image-save");
            Wait(root);
            CHECK(UmiCreativeAssetsGtkInspect(root, &info) == UMI_STATUS_OK &&
                  info.byte_count == input.size && strcmp(info.label, "source") == 0);
            size_t size = 0U;
            unsigned char *bytes = FixtureRead(path, &size);
            if (strcmp(mode, "export-existing") == 0)
                CHECK(size == 4U && memcmp(bytes, "keep", 4U) == 0);
            else if (strcmp(mode, "raw-copy") == 0)
                CHECK(size == input.size && memcmp(bytes, input.bytes, size) == 0);
            else
            {
                UmiMediaImageSurface *decoded = NULL;
                CHECK(UmiMediaPngDecode(bytes, size, NULL, &decoded) == UMI_STATUS_OK);
                UmiMediaImageSurfaceSnapshot snapshot;
                CHECK(umi_media_image_surface_snapshot(decoded, &snapshot) == UMI_STATUS_OK &&
                      snapshot.width == 2U && snapshot.height == 1U);
                UmiMediaRgbaPixel left, right;
                CHECK(umi_media_image_surface_get_pixel(decoded, 0U, 0U, &left) == UMI_STATUS_OK &&
                      left.red == 255U && left.green == 255U && left.blue == 255U && left.alpha == 255U);
                CHECK(umi_media_image_surface_get_pixel(decoded, 1U, 0U, &right) == UMI_STATUS_OK &&
                      right.red == 0U && right.green == 255U && right.blue == 0U && right.alpha == 128U);
                umi_media_image_surface_destroy(decoded);
                if (strcmp(mode, "unchanged-source") == 0)
                {
                    char raw[UMI_PATH_CAPACITY];
                    FixturePath(raw, directory, "original.png");
                    gtk_editable_set_text(GTK_EDITABLE(destination), raw);
                    Click(root, "creative.assets.write");
                    Wait(root);
                    size_t original_size = 0U;
                    unsigned char *original = FixtureRead(raw, &original_size);
                    CHECK(original_size == input.size && memcmp(original, input.bytes, input.size) == 0);
                    free(original);
                }
            }
            free(bytes);
        }
    }
    g_object_unref(root);
    return 0;
}
