/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_tool_rail_gtk4.c
 *
 * PURPOSE:
 *   Check named edge tabs, bounded whole-line rotation and accessible captions
 *   using unpresented GTK widgets. No application or external tool is started.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/tool_rail.h"

#include <stdio.h>
#include <string.h>

/* Keep acceptance checks active in release builds as well as debug builds. */
#define REQUIRE(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; \
        goto cleanup; \
    } \
} while (0)

/* Measure the caption itself so platform-specific button padding does not
 * obscure whether the text's long and short axes have been exchanged. */
static int caption_extent(GtkWidget *caption, GtkOrientation orientation)
{
    int natural = 0;
    gtk_widget_measure(caption, orientation, -1, NULL, &natural, NULL, NULL);
    return natural;
}

/* Construct only local widgets. GTK's test accessibility backend permits
 * checking full names without contacting an accessibility service. */
int main(void)
{
    GtkWidget *rail = NULL;
    GtkWidget *vertical;
    GtkWidget *horizontal;
    GtkWidget *long_tab;
    GtkWidget *invalid_rail = NULL;
    GtkWidget *retained = NULL;
    GtkWidget *caption;
    GtkSnapshot *snapshot = NULL;
    GskRenderNode *node = NULL;
    char *accessibility_error = NULL;
    char mutable_title[] = "Object Inspector";
    char long_title[1024];
    const char invalid_utf8[] = { (char)0xff, '\0' };
    int width;
    int height;
    int failed = 0;

    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) {
        (void)puts("SKIP: a GTK display is required for native rail widgets.");
        return 77;
    }
    rail = umi_gtk4_ws_tool_rail_create(GTK_ORIENTATION_VERTICAL);
    REQUIRE(rail != NULL);
    g_object_ref_sink(rail);
    REQUIRE(umi_gtk4_ws_tool_rail_append_label(NULL, "Structure", TRUE, FALSE) == NULL);
    REQUIRE(umi_gtk4_ws_tool_rail_append_label(rail, NULL, TRUE, FALSE) == NULL);
    REQUIRE(umi_gtk4_ws_tool_rail_append_label(rail, "", TRUE, FALSE) == NULL);
    REQUIRE(umi_gtk4_ws_tool_rail_append_label(rail, invalid_utf8, TRUE, FALSE) == NULL);
    REQUIRE(gtk_widget_get_first_child(rail) == NULL);
    invalid_rail = gtk_label_new("Not a rail");
    g_object_ref_sink(invalid_rail);
    REQUIRE(umi_gtk4_ws_tool_rail_append_label(
        invalid_rail, "Structure", TRUE, FALSE) == NULL);

    vertical = umi_gtk4_ws_tool_rail_append_label(rail, mutable_title, TRUE, TRUE);
    horizontal = umi_gtk4_ws_tool_rail_append_label(rail, mutable_title, FALSE, FALSE);
    REQUIRE(GTK_IS_TOGGLE_BUTTON(vertical) && GTK_IS_TOGGLE_BUTTON(horizontal));
    REQUIRE(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(vertical)));
    REQUIRE(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(horizontal)));
    REQUIRE(gtk_widget_has_css_class(vertical, "umicom-tool-rail-item"));
    REQUIRE(gtk_widget_has_css_class(vertical, "umicom-tool-rail-named-item"));
    mutable_title[0] = 'X';
    REQUIRE(strcmp(gtk_widget_get_tooltip_text(vertical), "Object Inspector") == 0);
    accessibility_error = gtk_test_accessible_check_property(
        GTK_ACCESSIBLE(vertical), GTK_ACCESSIBLE_PROPERTY_LABEL, "Object Inspector");
    REQUIRE(accessibility_error == NULL);

    /* Compare equal theme states: a user theme may use a bold active caption. */
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(vertical), FALSE);
    caption = gtk_button_get_child(GTK_BUTTON(vertical));
    REQUIRE(caption != NULL);
    width = caption_extent(caption, GTK_ORIENTATION_HORIZONTAL);
    height = caption_extent(caption, GTK_ORIENTATION_VERTICAL);
    REQUIRE(width > 0 && height > width && height <= 240);
    REQUIRE(width == caption_extent(
        gtk_button_get_child(GTK_BUTTON(horizontal)), GTK_ORIENTATION_VERTICAL));
    REQUIRE(height == caption_extent(
        gtk_button_get_child(GTK_BUTTON(horizontal)), GTK_ORIENTATION_HORIZONTAL));
    gtk_widget_allocate(caption, width, height, -1, NULL);
    snapshot = gtk_snapshot_new();
    GTK_WIDGET_GET_CLASS(caption)->snapshot(caption, snapshot);
    node = gtk_snapshot_free_to_node(snapshot);
    snapshot = NULL;
    REQUIRE(node != NULL);
    gsk_render_node_unref(node);
    node = NULL;

    /* A long name stays available to accessibility while its drawn caption is
     * bounded. The original icon-based append API remains usable alongside it. */
    memset(long_title, 'W', sizeof(long_title) - 1U);
    long_title[sizeof(long_title) - 1U] = '\0';
    long_tab = umi_gtk4_ws_tool_rail_append_label(rail, long_title, TRUE, FALSE);
    REQUIRE(long_tab != NULL);
    REQUIRE(caption_extent(gtk_button_get_child(GTK_BUTTON(long_tab)),
        GTK_ORIENTATION_VERTICAL) <= 240);
    accessibility_error = gtk_test_accessible_check_property(
        GTK_ACCESSIBLE(long_tab), GTK_ACCESSIBLE_PROPERTY_LABEL, long_title);
    REQUIRE(accessibility_error == NULL);
    REQUIRE(umi_gtk4_ws_tool_rail_append(
        rail, "document-open-symbolic", "Open a document", FALSE) != NULL);
    REQUIRE(umi_gtk4_ws_tool_rail_append_label(
        rail, "Structure \xE2\x80\x94 \xCE\x94", TRUE, FALSE) != NULL);

    /* Caption and button own all display data; retaining a tab after the rail
     * dies must not leave a callback or title pointer borrowing that rail. */
    retained = g_object_ref(vertical);
    g_clear_object(&rail);
    REQUIRE(gtk_widget_get_parent(retained) == NULL);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(retained), FALSE);
    REQUIRE(strcmp(gtk_widget_get_tooltip_text(retained), "Object Inspector") == 0);

cleanup:
    g_free(accessibility_error);
    if (snapshot != NULL) g_object_unref(snapshot);
    if (node != NULL) gsk_render_node_unref(node);
    g_clear_object(&retained);
    g_clear_object(&invalid_rail);
    g_clear_object(&rail);
    if (!failed) (void)puts("Native named tool rail checks passed.");
    return failed;
}
