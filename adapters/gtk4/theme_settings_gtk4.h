/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/theme_settings_gtk4.h
 * PURPOSE: Apply the native colour preference using properties supported by the GTK runtime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GTK4_THEME_SETTINGS_INTERNAL_H
#define UMICOM_GTK4_THEME_SETTINGS_INTERNAL_H

#include <gtk/gtk.h>

/* Centralise the display preference so editor, desk and workbench cannot drift.
 * Resolve the enum through its property: builds with older headers can still
 * use a newer GTK runtime without writing the deprecated boolean property.
 * Settings remain borrowed and must be used on their GTK owner thread. */
static inline void umi_gtk4_settings_prefer_dark(GtkSettings *settings, gboolean prefer_dark)
{
    if (settings == NULL) return;
    GParamSpec *property = g_object_class_find_property(
        G_OBJECT_GET_CLASS(settings), "gtk-interface-color-scheme");
    if (property != NULL && G_IS_PARAM_SPEC_ENUM(property) &&
        (property->flags & G_PARAM_WRITABLE) != 0U) {
        GEnumClass *values = g_type_class_ref(G_PARAM_SPEC_VALUE_TYPE(property));
        const GEnumValue *choice = g_enum_get_value_by_name(values,
            prefer_dark ? "GTK_INTERFACE_COLOR_SCHEME_DARK" : "GTK_INTERFACE_COLOR_SCHEME_LIGHT");
        if (choice != NULL) {
            g_object_set(settings, "gtk-interface-color-scheme", choice->value, NULL);
            g_type_class_unref(values);
            return;
        }
        g_type_class_unref(values);
    }
    /* Older GTK runtimes have only the boolean preference. Keep that supported
     * fallback active; no version number is guessed from the build machine. */
    g_object_set(settings, "gtk-application-prefer-dark-theme", prefer_dark != FALSE, NULL);
}
#endif
