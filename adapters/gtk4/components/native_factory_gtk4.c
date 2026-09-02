/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/components/native_factory_gtk4.c
 *
 * PURPOSE:
 *   Create a public, catalogued GTK4 widget by type name using GtkBuilder.
 *   Applications still request this through Framework and never own toolkit
 *   construction policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/native_factory.h"

#include <string.h>

#include "umicom/ui/gtk4/component_factory.h"
#include "umicom/ui/gtk4/widget_catalogue.h"

/*
 * Initialise gtk4 native widget from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_native_widget_create(
    const char *gtk_type_name,
    const UmiUiComponentSpec *common_spec,
    GError **error)
{
    const UmiGtk4WidgetDefinition *definition;
    GtkBuilder *builder;
    GObject *object;
    GtkWidget *widget;
    char markup[512];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (gtk_type_name == NULL || gtk_type_name[0] == '\0') return NULL;
    definition = umi_gtk4_widget_catalogue_find(gtk_type_name);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || definition->abstract_type ||
        definition->exposure == UMI_GTK4_EXPOSURE_FRAMEWORK_INTERNAL) return NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strchr(gtk_type_name, '<') != NULL ||
        strchr(gtk_type_name, '>') != NULL ||
        strchr(gtk_type_name, '&') != NULL ||
        strchr(gtk_type_name, '\"') != NULL) return NULL;

    (void)g_snprintf(markup, sizeof(markup),
        "<interface><object class=\"%s\" id=\"umicom-native-widget\"/></interface>",
        gtk_type_name);
    builder = gtk_builder_new();
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!gtk_builder_add_from_string(builder, markup, -1, error)) {
        g_object_unref(builder);
        return NULL;
    }
    object = gtk_builder_get_object(builder, "umicom-native-widget");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (object == NULL || !GTK_IS_WIDGET(object)) {
        g_object_unref(builder);
        return NULL;
    }
    widget = GTK_WIDGET(g_object_ref(object));
    g_object_unref(builder);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (common_spec != NULL)
        (void)umi_gtk4_component_apply_common(widget, common_spec);
    return widget;
}
