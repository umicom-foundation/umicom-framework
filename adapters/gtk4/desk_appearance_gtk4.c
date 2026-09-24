/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desk_appearance_gtk4.c
 *
 * PURPOSE:
 *   Project Framework appearance tokens onto Umicom Desk so Desk, Studio,
 *   Trader, Bank and other GTK4 products share one visual language.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/desk_appearance.h"

#include <string.h>

#define UMI_DESK_APPEARANCE_PROVIDER_KEY "umicom-desk-appearance-provider"

/* Release a previous display provider before replacing its semantic palette. */
static void replace_display_provider(
    GdkDisplay *display,
    GtkCssProvider *provider)
{
    GtkCssProvider *previous;

    if (display == NULL || provider == NULL) return;
    previous = (GtkCssProvider *)g_object_get_data(
        G_OBJECT(display), UMI_DESK_APPEARANCE_PROVIDER_KEY);
    if (previous != NULL) {
        gtk_style_context_remove_provider_for_display(
            display, GTK_STYLE_PROVIDER(previous));
        g_object_set_data(
            G_OBJECT(display), UMI_DESK_APPEARANCE_PROVIDER_KEY, NULL);
    }

    gtk_style_context_add_provider_for_display(
        display,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_set_data_full(
        G_OBJECT(display),
        UMI_DESK_APPEARANCE_PROVIDER_KEY,
        g_object_ref(provider),
        g_object_unref);
}

/*
 * Apply the same semantic colours used by Framework workstations to Desk.
 * Widget selectors are deliberately limited to Desk classes so this provider
 * cannot restyle unrelated GTK applications on the same desktop session.
 */
UmiStatus umi_gtk4_desk_apply_appearance(
    GtkWindow *window,
    const UmiUiAppearanceProfile *profile)
{
    GtkCssProvider *provider;
    GdkDisplay *display;
    GtkSettings *settings;
    char *css;
    char reason[256];
    UmiStatus status;
    int prefer_dark;

    if (window == NULL || !GTK_IS_WINDOW(window) || profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_appearance_profile_validate(
        profile, reason, sizeof(reason));
    if (status != UMI_STATUS_OK) return status;

    display = gtk_widget_get_display(GTK_WIDGET(window));
    if (display == NULL) return UMI_STATUS_INVALID_STATE;

    css = g_strdup_printf(
        "@define-color umi_desk_background %s;"
        "@define-color umi_desk_surface %s;"
        "@define-color umi_desk_raised %s;"
        "@define-color umi_desk_hover %s;"
        "@define-color umi_desk_foreground %s;"
        "@define-color umi_desk_muted %s;"
        "@define-color umi_desk_border %s;"
        "@define-color umi_desk_border_strong %s;"
        "@define-color umi_desk_accent %s;"
        "@define-color umi_desk_accent_surface %s;"
        "@define-color umi_desk_success %s;"
        "@define-color umi_desk_warning %s;"
        "@define-color umi_desk_danger %s;"
        ".umicom-desk-root,.umicom-desk-context-root{"
        "background:@umi_desk_background;color:@umi_desk_foreground;}"
        ".umicom-desk-global-bar,.umicom-desk-bottom{"
        "background:@umi_desk_surface;color:@umi_desk_foreground;}"
        ".umicom-desk-home,.umicom-desk-application-chooser{"
        "background:@umi_desk_background;color:@umi_desk_foreground;}"
        ".umicom-desk-home-tile{"
        "background:@umi_desk_surface;border:1px solid @umi_desk_border;"
        "border-radius:8px;}"
        ".umicom-desk-home-tile:hover{"
        "background:@umi_desk_raised;border-color:@umi_desk_border_strong;}"
        ".umicom-desk-launch-choice{"
        "padding:8px 10px;background:@umi_desk_surface;"
        "border:1px solid @umi_desk_border;border-radius:6px;}"
        ".umicom-desk-launch-choice:hover{background:@umi_desk_raised;}"
        ".umicom-desk-application-strip,.umicom-desk-layout-strip{"
        "background:@umi_desk_surface;}"
        "button.umicom-desk-application-button{"
        "min-height:30px;padding:4px 8px;border-radius:5px;"
        "color:@umi_desk_foreground;}"
        "button.umicom-desk-application-button:hover{background:@umi_desk_hover;}"
        "button.umicom-desk-application-button.running{"
        "background:@umi_desk_raised;border-bottom:2px solid @umi_desk_success;}"
        "button.umicom-desk-application-button.active{"
        "background:@umi_desk_accent_surface;border-bottom:2px solid @umi_desk_accent;}"
        "button.umicom-desk-application-button.requires-attention{"
        "border-bottom:2px solid @umi_desk_warning;}"
        ".umicom-desk-workbench{"
        "background:@umi_desk_surface;border-color:@umi_desk_border;}"
        ".umicom-desk-global-button{color:@umi_desk_foreground;}"
        ".umicom-desk-global-button:hover{background:@umi_desk_hover;}"
        ".dim-label{color:@umi_desk_muted;}",
        profile->background,
        profile->surface,
        profile->raised_surface,
        profile->hover_surface,
        profile->foreground,
        profile->muted_foreground,
        profile->border,
        profile->strong_border,
        profile->accent,
        profile->accent_surface,
        profile->success,
        profile->warning,
        profile->danger);
    if (css == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    provider = gtk_css_provider_new();
    if (provider == NULL) {
        g_free(css);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    gtk_css_provider_load_from_string(provider, css);
    g_free(css);
    replace_display_provider(display, provider);
    g_object_unref(provider);

    prefer_dark = profile->mode == UMI_UI_THEME_MODE_DARK ||
        profile->mode == UMI_UI_THEME_MODE_HIGH_CONTRAST;
    settings = gtk_settings_get_for_display(display);
    if (settings != NULL) {
        g_object_set(
            settings,
            "gtk-application-prefer-dark-theme",
            prefer_dark != 0,
            NULL);
    }
    return UMI_STATUS_OK;
}
