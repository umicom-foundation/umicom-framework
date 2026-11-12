/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/appearance_typography_private.h
 *
 * PURPOSE:
 *   Project validated shared interface and code font roles into locale-independent GTK CSS.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GTK4_APPEARANCE_TYPOGRAPHY_PRIVATE_H
#define UMICOM_GTK4_APPEARANCE_TYPOGRAPHY_PRIVATE_H
#include "umicom/ui/appearance.h"
#include <glib.h>

/* Family names are validated by the shared profile contract. Quoting each
 * concrete family keeps periods and leading digits from becoming CSS syntax;
 * the generic fallbacks remain keywords rather than literal font names. */
static char *UmiGtk4FontFamiliesCss(const char *families)
{
    char **parts = g_strsplit(families, ",", -1);
    GString *result = g_string_new(NULL);
    for (size_t i = 0U; parts[i] != NULL; ++i) {
        const char *name = g_strstrip(parts[i]);
        if (i != 0U) g_string_append(result, ", ");
        if (g_ascii_strcasecmp(name, "Sans") == 0 || g_ascii_strcasecmp(name, "sans-serif") == 0)
            g_string_append(result, "sans-serif");
        else if (g_ascii_strcasecmp(name, "Monospace") == 0)
            g_string_append(result, "monospace");
        else if (g_ascii_strcasecmp(name, "Serif") == 0)
            g_string_append(result, "serif");
        else if (g_ascii_strcasecmp(name, "Cursive") == 0)
            g_string_append(result, "cursive");
        else if (g_ascii_strcasecmp(name, "Fantasy") == 0)
            g_string_append(result, "fantasy");
        else if (g_ascii_strcasecmp(name, "system-ui") == 0)
            g_string_append(result, "system-ui");
        else
            g_string_append_printf(result, "\"%s\"", name);
    }
    g_strfreev(parts);
    return g_string_free(result, FALSE);
}

/* This is a renderer fragment, not a new appearance model. Both GTK shells
 * consume the same validated profile, retain user choices and use one code
 * role for editors, monospace text views and explicitly marked code widgets.
 * The caller owns the returned string and releases it with g_free(). */
static char *UmiGtk4AppearanceTypographyCss(const UmiUiAppearanceProfile *profile,
    gboolean workbench)
{
    char reason[192], interface_size[G_ASCII_DTOSTR_BUF_SIZE], editor_size[G_ASCII_DTOSTR_BUF_SIZE];
    if (umi_ui_appearance_profile_validate(profile, reason, sizeof(reason)) != UMI_STATUS_OK)
        return NULL;
    const char *scope = workbench ? "window.umicom-workbench" : ".umicom-appearance-scope";
    char *interface_family = UmiGtk4FontFamiliesCss(profile->interface_font);
    char *editor_family = UmiGtk4FontFamiliesCss(profile->editor_font);
    /* CSS always uses a period. Never change the process locale to format a
     * theme: other threads may be parsing user data at the same time. */
    g_ascii_formatd(interface_size, (gint)sizeof(interface_size), "%.2f",
        profile->interface_font_size * profile->font_scale);
    g_ascii_formatd(editor_size, (gint)sizeof(editor_size), "%.2f",
        profile->editor_font_size * profile->font_scale);
    GString *css = g_string_new(NULL);
    g_string_append_printf(css, "%s { font-family: %s; font-size: %spt; }",
        scope, interface_family, interface_size);
    static const char *const code_roles[] = {
        "textview.umicom-editor", "textview.umicom-editor text",
        "textview.monospace", "textview.monospace text",
        ".umicom-code-font", ".umicom-view-model-row"
    };
    for (size_t i = 0U; i < G_N_ELEMENTS(code_roles); ++i) {
        if (i != 0U) g_string_append(css, ",");
        g_string_append_printf(css, "%s %s", scope, code_roles[i]);
    }
    g_string_append_printf(css, " { font-family: %s; font-size: %spt; }", editor_family, editor_size);
    g_free(interface_family);
    g_free(editor_family);
    return g_string_free(css, FALSE);
}
#endif
