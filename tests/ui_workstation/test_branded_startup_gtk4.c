/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_branded_startup_gtk4.c
 *
 * PURPOSE:
 *   Verify vector startup branding, progress, status and retained native widget ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/shell_header.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); return 1; } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, name);
        if (found != NULL) return found;
    }
    return NULL;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    UmiGtk4WorkstationStartupSplashConfig config =
        umi_gtk4_ws_startup_splash_config_default("org.umicom.startup-test", "Umicom Trader");
    config.resource_root = "/unavailable-startup-resources";
    config.icon_resource = "missing.svg";
    config.status = "Loading";
    config.mode_badge = "";
    UmiGtk4WorkstationStartupSplash *splash = NULL;
    gboolean legacy = strcmp(argv[1], "legacy") == 0;
    CHECK((legacy ? umi_gtk4_ws_startup_splash_create(&config, &splash)
        : umi_gtk4_ws_startup_splash_create_branded(&config, &splash)) == UMI_STATUS_OK);
    GtkWidget *root = umi_gtk4_ws_startup_splash_widget(splash);
    CHECK(root != NULL);
    g_object_ref(root);
    UmiGtk4WorkstationStartupSplashSnapshot snapshot = umi_gtk4_ws_startup_splash_snapshot(splash);
    CHECK(strcmp(snapshot.title, "Umicom Trader") == 0);
    if (legacy) {
        CHECK(Find(root, "umicom-branded-startup") == NULL);
        CHECK(Find(root, "umicom.brand.logo") == NULL);
        CHECK(snapshot.icon_visible == 0);
    } else if (strcmp(argv[1], "brand") == 0) {
        CHECK(Find(root, "umicom-branded-startup") == root);
        CHECK(GTK_IS_DRAWING_AREA(Find(root, "umicom.brand.logo")));
        CHECK(snapshot.icon_visible == 1 && snapshot.mode_badge[0] == '\0');
    } else if (strcmp(argv[1], "status") == 0) {
        CHECK(umi_gtk4_ws_startup_splash_set_status(splash, "Choose account", "") == UMI_STATUS_OK);
        snapshot = umi_gtk4_ws_startup_splash_snapshot(splash);
        CHECK(strcmp(snapshot.status, "Choose account") == 0);
        CHECK(snapshot.icon_visible == 1);
    } else if (strcmp(argv[1], "progress") == 0) {
        CHECK(umi_gtk4_ws_startup_splash_set_progress(splash, 0.5, 1) == UMI_STATUS_OK);
        snapshot = umi_gtk4_ws_startup_splash_snapshot(splash);
        CHECK(snapshot.progress == 0.5 && snapshot.progress_visible == 1);
        CHECK(umi_gtk4_ws_startup_splash_set_progress(splash, 1.0, 0) == UMI_STATUS_OK);
        snapshot = umi_gtk4_ws_startup_splash_snapshot(splash);
        CHECK(snapshot.progress == 1.0 && snapshot.progress_visible == 0);
    } else if (strcmp(argv[1], "retained") == 0) {
        umi_gtk4_ws_startup_splash_destroy(splash); splash = NULL;
        CHECK(Find(root, "umicom.brand.logo") != NULL);
        CHECK(gtk_widget_get_first_child(root) != NULL);
    } else if (strcmp(argv[1], "invalid") == 0) {
        UmiGtk4WorkstationStartupSplash *other = NULL;
        CHECK(umi_gtk4_ws_startup_splash_create_branded(NULL, &other) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(other == NULL);
        CHECK(umi_gtk4_ws_startup_splash_create_branded(&config, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    umi_gtk4_ws_startup_splash_destroy(splash);
    g_object_unref(root);
    return 0;
}
