/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/appearance_gtk4.c
 *
 * PURPOSE:
 *   Render the active appearance profile and professional GTK4 theme picker.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"

#include <string.h>

static const char *mode_icon(UmiUiThemeMode mode)
{
    switch (mode) {
        case UMI_UI_THEME_MODE_LIGHT: return "weather-clear-symbolic";
        case UMI_UI_THEME_MODE_DARK: return "weather-clear-night-symbolic";
        case UMI_UI_THEME_MODE_HIGH_CONTRAST:
            return "preferences-desktop-accessibility-symbolic";
        case UMI_UI_THEME_MODE_SYSTEM:
        default: return "video-display-symbolic";
    }
}

static const char *density_name(UmiUiDensity density)
{
    switch (density) {
        case UMI_UI_DENSITY_COMPACT: return "Compact";
        case UMI_UI_DENSITY_SPACIOUS: return "Spacious";
        case UMI_UI_DENSITY_COMFORTABLE:
        default: return "Comfortable";
    }
}

static void sync_active_context(UmiUiWorkbench *workbench,
                                const UmiUiAppearanceProfile *profile)
{
    UmiUiContextStore *context = umi_ui_workbench_context(workbench);
    const char *density = "comfortable";
    if (profile->density == UMI_UI_DENSITY_COMPACT) density = "compact";
    if (profile->density == UMI_UI_DENSITY_SPACIOUS) density = "spacious";
    (void)umi_ui_context_set_string(context, "studio.ui.theme",
                                    profile->profile_id);
    (void)umi_ui_context_set_string(context, "studio.ui.density", density);
    (void)umi_ui_context_set_string(context, "studio.ui.font.interface",
                                    profile->interface_font);
    (void)umi_ui_context_set_string(context, "studio.ui.font.editor",
                                    profile->editor_font);
    (void)umi_ui_context_set_integer(
        context, "studio.ui.font.scale-per-thousand",
        (int64_t)(profile->font_scale * 1000.0));
    (void)umi_ui_context_set_boolean(context, "studio.ui.reduce-motion",
                                     profile->reduce_motion);
}

static void on_appearance_selected(GtkButton *button, gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiAppearanceProfile active;
    const char *profile_id;
    if (adapter == NULL || adapter->shell == NULL) return;
    profile_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-appearance-profile-id");
    if (profile_id == NULL) return;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    if (umi_ui_appearance_model_set_active(
            umi_ui_workbench_appearance(workbench), profile_id) !=
        UMI_STATUS_OK ||
        umi_ui_appearance_model_active(
            umi_ui_workbench_appearance(workbench), &active) !=
        UMI_STATUS_OK) {
        return;
    }
    sync_active_context(workbench, &active);
    gtk_label_set_text(GTK_LABEL(adapter->appearance_label), active.label);
    gtk_menu_button_popdown(GTK_MENU_BUTTON(adapter->appearance_button));
    (void)umi_gtk4_apply_theme(adapter, workbench);
    /* Rebuild the picker so its selected indicator is correct next time the
     * user opens it; the old popover is released by GtkMenuButton. */
    (void)umi_gtk4_refresh_appearance(adapter, workbench);
}

static GtkWidget *brand_header(UmiUiWorkbench *workbench)
{
    UmiUiContextSnapshot value;
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *title;
    GtkWidget *subtitle;

    gtk_widget_add_css_class(box, "umicom-appearance-header");
    if (umi_ui_context_get(umi_ui_workbench_context(workbench),
                           "studio.brand.logo-path", &value) ==
            UMI_STATUS_OK &&
        value.kind == UMI_UI_CONTEXT_STRING &&
        g_file_test(value.string_value, G_FILE_TEST_IS_REGULAR)) {
        GtkWidget *logo = gtk_picture_new_for_filename(value.string_value);
        gtk_picture_set_content_fit(GTK_PICTURE(logo), GTK_CONTENT_FIT_CONTAIN);
        gtk_widget_set_size_request(logo, 210, 52);
        gtk_widget_add_css_class(logo, "umicom-brand-logo");
        gtk_box_append(GTK_BOX(box), logo);
    }
    title = gtk_label_new("Appearance and Theme Centre");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_widget_add_css_class(title, "umicom-appearance-title");
    subtitle = gtk_label_new(
        "Live colour, density and typography profiles for this workbench");
    gtk_label_set_xalign(GTK_LABEL(subtitle), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(subtitle), TRUE);
    gtk_widget_add_css_class(subtitle, "umicom-appearance-description");
    gtk_box_append(GTK_BOX(box), title);
    gtk_box_append(GTK_BOX(box), subtitle);
    return box;
}

static GtkWidget *profile_row(UmiGtk4Adapter *adapter,
                              const UmiUiAppearanceProfile *profile)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 9);
    GtkWidget *icon = gtk_image_new_from_icon_name(mode_icon(profile->mode));
    GtkWidget *copy = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    GtkWidget *label = gtk_label_new(profile->label);
    char detail[192U];
    GtkWidget *description;
    GtkWidget *selected;

    gtk_image_set_pixel_size(GTK_IMAGE(icon), 20);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_widget_add_css_class(label, "umicom-appearance-item-title");
    (void)g_snprintf(detail, sizeof(detail), "%s · %.0f%% · %s",
                     density_name(profile->density),
                     profile->font_scale * 100.0,
                     profile->description);
    description = gtk_label_new(detail);
    gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
    gtk_label_set_ellipsize(GTK_LABEL(description), PANGO_ELLIPSIZE_END);
    gtk_widget_add_css_class(description, "umicom-appearance-description");
    gtk_widget_set_hexpand(copy, TRUE);
    gtk_box_append(GTK_BOX(copy), label);
    gtk_box_append(GTK_BOX(copy), description);
    gtk_box_append(GTK_BOX(row), icon);
    gtk_box_append(GTK_BOX(row), copy);
    if (profile->active) {
        selected = gtk_image_new_from_icon_name("object-select-symbolic");
        gtk_image_set_pixel_size(GTK_IMAGE(selected), 17);
        gtk_widget_add_css_class(selected, "accent");
        gtk_box_append(GTK_BOX(row), selected);
        gtk_widget_add_css_class(button, "active");
    }
    gtk_button_set_child(GTK_BUTTON(button), row);
    gtk_widget_add_css_class(button, "umicom-appearance-item");
    g_object_set_data_full(G_OBJECT(button), "umicom-appearance-profile-id",
                           g_strdup(profile->profile_id), g_free);
    g_signal_connect(button, "clicked", G_CALLBACK(on_appearance_selected),
                     adapter);
    return button;
}

UmiStatus umi_gtk4_refresh_appearance(UmiGtk4Adapter *adapter,
                                     UmiUiWorkbench *workbench)
{
    UmiUiAppearanceModel *model;
    UmiUiAppearanceProfile profile;
    GtkWidget *popover;
    GtkWidget *content;
    GtkWidget *separator;
    size_t index;

    if (adapter == NULL || adapter->appearance_button == NULL ||
        workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model = umi_ui_workbench_appearance(workbench);
    if (umi_ui_appearance_model_active(model, &profile) == UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(adapter->appearance_label), profile.label);
    } else {
        gtk_label_set_text(GTK_LABEL(adapter->appearance_label), "Appearance");
    }

    popover = gtk_popover_new();
    gtk_widget_add_css_class(popover, "umicom-appearance-popover");
    content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_append(GTK_BOX(content), brand_header(workbench));
    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(content), separator);
    for (index = 0U; index < umi_ui_appearance_model_count(model); ++index) {
        if (umi_ui_appearance_model_at(model, index, &profile) ==
            UMI_STATUS_OK) {
            gtk_box_append(GTK_BOX(content), profile_row(adapter, &profile));
        }
    }
    gtk_popover_set_child(GTK_POPOVER(popover), content);
    gtk_menu_button_set_popover(GTK_MENU_BUTTON(adapter->appearance_button),
                                popover);
    return UMI_STATUS_OK;
}
