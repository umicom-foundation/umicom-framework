/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_theme_settings_gtk4.c
 * PURPOSE: Check native light and dark preference selection on the installed GTK runtime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/theme_settings_gtk4.h"
#include <stdio.h>
#include <string.h>

/* Each CTest invocation has its own process and default settings. Verify the
 * selected property directly so a quiet no-op cannot pass as a theme change. */
int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "light") != 0 && strcmp(argv[1], "dark") != 0))
        return 2;
    if (!gtk_init_check()) return 77;
    GtkSettings *settings = gtk_settings_get_default();
    if (settings == NULL) return 77;
    gboolean dark = strcmp(argv[1], "dark") == 0;
    umi_gtk4_settings_prefer_dark(NULL, dark);
    umi_gtk4_settings_prefer_dark(settings, dark);
    GParamSpec *property = g_object_class_find_property(
        G_OBJECT_GET_CLASS(settings), "gtk-interface-color-scheme");
    if (property != NULL && G_IS_PARAM_SPEC_ENUM(property)) {
        gint actual = 0;
        GEnumClass *values = g_type_class_ref(G_PARAM_SPEC_VALUE_TYPE(property));
        const GEnumValue *expected = g_enum_get_value_by_name(values,
            dark ? "GTK_INTERFACE_COLOR_SCHEME_DARK" : "GTK_INTERFACE_COLOR_SCHEME_LIGHT");
        g_object_get(settings, "gtk-interface-color-scheme", &actual, NULL);
        int result = expected != NULL && actual == expected->value ? 0 : 1;
        g_type_class_unref(values);
        if (result != 0) fputs("Native colour scheme did not match the requested mode\n", stderr);
        return result;
    }
    gboolean actual = !dark;
    g_object_get(settings, "gtk-application-prefer-dark-theme", &actual, NULL);
    return actual == dark ? 0 : 1;
}
