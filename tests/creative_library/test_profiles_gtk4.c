/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_library/test_profiles_gtk4.c
 * PURPOSE: Check that creative products retain their existing Assets editor and compose the same shared library.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../creative_library/check.h"
#include "umicom/ui/gtk4/creative_workspace.h"
#include "umicom/ui/gtk4/creative_library.h"
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
int main(int argc, char **argv)
{
    const char *profiles[] = {"media", "music", "cad", "kitchen", "games", "web-studio", "mobile-studio"};
    if (argc != 2 || !Known(argv[1], profiles, sizeof(profiles) / sizeof(profiles[0])))
        return 2;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    UmiCreativeGtkPanel *panel = NULL;
    CHECK(UmiCreativeGtkPanelCreate(NULL, argv[1], &panel) == UMI_STATUS_OK);
    GtkWidget *root = UmiCreativeGtkPanelWidget(panel), *library = Find(root, "creative.library");
    CHECK(library != NULL && Find(root, "creative.assets") != NULL && Find(root, "creative.canvas") != NULL);
    if (strcmp(argv[1], "media") == 0 || strcmp(argv[1], "music") == 0)
        CHECK(Find(root, "creative.arrangement") != NULL);
    UmiCreativeAssetLibraryInfo info;
    CHECK(UmiCreativeAssetLibraryGtkInspect(library, &info) == UMI_STATUS_OK && info.asset_count == 0U);
    GtkWidget *button = g_object_ref(Find(root, "creative.library.save"));
    UmiCreativeGtkPanelDestroy(panel);
    g_signal_emit_by_name(button, "clicked");
    g_object_unref(button);
    return 0;
}
