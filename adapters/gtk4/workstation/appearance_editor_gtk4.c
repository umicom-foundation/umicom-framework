/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/appearance_editor_gtk4.c
 *
 * PURPOSE:
 *   Render shared appearance presets and safe custom colour, font and density
 *   controls for any GTK4 application workbench.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/appearance_editor.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/appearance_catalogue.h"
#include "umicom/ui/appearance_persistence.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/drop_down.h"

#define UMI_GTK4_APPEARANCE_COLOUR_COUNT 14U

typedef enum UmiGtk4AppearanceColourField {
  UMI_GTK4_COLOUR_BACKGROUND = 0,
  UMI_GTK4_COLOUR_SURFACE,
  UMI_GTK4_COLOUR_RAISED_SURFACE,
  UMI_GTK4_COLOUR_HOVER_SURFACE,
  UMI_GTK4_COLOUR_EDITOR_BACKGROUND,
  UMI_GTK4_COLOUR_FOREGROUND,
  UMI_GTK4_COLOUR_MUTED_FOREGROUND,
  UMI_GTK4_COLOUR_BORDER,
  UMI_GTK4_COLOUR_STRONG_BORDER,
  UMI_GTK4_COLOUR_ACCENT,
  UMI_GTK4_COLOUR_ACCENT_SURFACE,
  UMI_GTK4_COLOUR_SUCCESS,
  UMI_GTK4_COLOUR_WARNING,
  UMI_GTK4_COLOUR_DANGER
} UmiGtk4AppearanceColourField;

struct UmiGtk4AppearanceEditor {
  UmiUiAppearanceModel *model;
  GtkWidget *scope_root;
  GtkWidget *button;
  GtkWidget *profile_dropdown;
  GtkWidget *density_dropdown;
  GtkWidget *font_scale;
  GtkWidget *interface_font;
  GtkWidget *editor_font;
  GtkWidget *colour_entries[UMI_GTK4_APPEARANCE_COLOUR_COUNT];
  GtkWidget *status;
  GtkCssProvider *provider;
  GdkDisplay *display;
  char *settings_path;
  UmiGtk4AppearanceChangedHandler changed_handler;
  void *changed_user_data;
  int changing_controls;
  uint64_t revision;
};

static const char *COLOUR_LABELS[UMI_GTK4_APPEARANCE_COLOUR_COUNT] = {
    "Background",     "Surface",    "Raised surface", "Hover surface", "Editor",
    "Text",           "Muted text", "Border",         "Strong border", "Accent",
    "Accent surface", "Success",    "Warning",        "Danger"};

/* Copy validated text into a fixed profile field and report truncation. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source) {
  int written;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (destination == NULL || capacity == 0U || source == NULL) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  written = snprintf(destination, capacity, "%s", source);
  return written < 0 || (size_t)written >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/* Return a writable semantic colour field selected by the editor row. */
static char *profile_colour(UmiUiAppearanceProfile *profile, UmiGtk4AppearanceColourField field) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (profile == NULL)
    return NULL;
  /* Select the behaviour associated with the requested command or state value. */
  switch (field) {
  case UMI_GTK4_COLOUR_BACKGROUND:
    return profile->background;
  case UMI_GTK4_COLOUR_SURFACE:
    return profile->surface;
  case UMI_GTK4_COLOUR_RAISED_SURFACE:
    return profile->raised_surface;
  case UMI_GTK4_COLOUR_HOVER_SURFACE:
    return profile->hover_surface;
  case UMI_GTK4_COLOUR_EDITOR_BACKGROUND:
    return profile->editor_background;
  case UMI_GTK4_COLOUR_FOREGROUND:
    return profile->foreground;
  case UMI_GTK4_COLOUR_MUTED_FOREGROUND:
    return profile->muted_foreground;
  case UMI_GTK4_COLOUR_BORDER:
    return profile->border;
  case UMI_GTK4_COLOUR_STRONG_BORDER:
    return profile->strong_border;
  case UMI_GTK4_COLOUR_ACCENT:
    return profile->accent;
  case UMI_GTK4_COLOUR_ACCENT_SURFACE:
    return profile->accent_surface;
  case UMI_GTK4_COLOUR_SUCCESS:
    return profile->success;
  case UMI_GTK4_COLOUR_WARNING:
    return profile->warning;
  case UMI_GTK4_COLOUR_DANGER:
    return profile->danger;
  default:
    return NULL;
  }
}

/* Create scoped CSS from a validated profile so one application process can
 * restyle its complete workbench without changing business widgets. */
static char *profile_css(const UmiUiAppearanceProfile *profile) {
  double interface_size;
  int control_height;
  int spacing;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (profile == NULL)
    return NULL;
  interface_size = profile->interface_font_size * profile->font_scale;
  control_height = profile->density == UMI_UI_DENSITY_COMPACT
                       ? 25
                       : (profile->density == UMI_UI_DENSITY_SPACIOUS ? 34 : 29);
  spacing = profile->density == UMI_UI_DENSITY_COMPACT
                ? 3
                : (profile->density == UMI_UI_DENSITY_SPACIOUS ? 8 : 5);

  return g_strdup_printf(
      ".umicom-appearance-scope { background: %s; color: %s; "
      "font-family: %s; font-size: %.2fpt; }"
      ".umicom-appearance-scope .umicom-suite-layout-header {"
      " min-height: %dpx; padding: %dpx 7px; background: %s;"
      " border-bottom: 1px solid %s; }"
      ".umicom-appearance-scope .umicom-workstation-identity {"
      " min-height: 24px; padding: 1px 7px 1px 4px; color: %s;"
      " border: 1px solid alpha(black,0.20); border-radius: 3px;"
      " background: alpha(black,0.08);"
      " box-shadow: inset 0 1px alpha(white,0.05),"
      " inset 0 -1px alpha(black,0.24); }"
      ".umicom-appearance-scope .umicom-workstation-identity-title {"
      " color: %s; font-weight: 700; text-shadow: 0 1px alpha(black,0.42); }"
      ".umicom-appearance-scope .umicom-workstation-identity-icon {"
      " margin-right: 2px; opacity: 0.94; }"
      ".umicom-appearance-scope .umicom-mode-badge {"
      " color: %s; background: %s; border-radius: 3px; padding: 1px 5px; }"
      ".umicom-appearance-scope .umicom-workstation-panel {"
      " background: %s; color: %s; border-color: %s; }"
      ".umicom-appearance-scope .umicom-panel-header {"
      " background: %s; color: %s; border-color: %s; }"
      ".umicom-appearance-scope button,"
      ".umicom-appearance-scope entry,"
      ".umicom-appearance-scope dropdown > button {"
      " min-height: %dpx; color: %s; background: %s;"
      " border: 1px solid %s; border-radius: 3px; }"
      ".umicom-appearance-scope button:hover { background: %s; }"
      ".umicom-appearance-scope button:checked,"
      ".umicom-appearance-scope button.suggested-action {"
      " background: %s; color: %s; border-color: %s; }"
      ".umicom-appearance-scope popover contents {"
      " color: %s; background: %s; border: 1px solid %s; }"
      ".umicom-appearance-scope notebook > header {"
      " background: %s; border-color: %s; }"
      ".umicom-appearance-scope notebook > stack,"
      ".umicom-appearance-scope textview,"
      ".umicom-appearance-scope textview text,"
      ".umicom-appearance-scope .view {"
      " color: %s; background: %s; }"
      ".umicom-appearance-scope .dim-label { color: %s; }"
      ".umicom-appearance-scope separator { background: %s; }"
      ".umicom-appearance-scope .error { color: %s; }",
      profile->background, profile->foreground, profile->interface_font, interface_size,
      control_height, spacing, profile->raised_surface, profile->border,
      profile->foreground, profile->foreground, profile->muted_foreground,
      profile->accent_surface, profile->surface, profile->foreground, profile->border,
      profile->raised_surface, profile->foreground, profile->strong_border, control_height,
      profile->foreground, profile->raised_surface, profile->border, profile->hover_surface,
      profile->accent_surface, profile->foreground, profile->accent, profile->foreground,
      profile->raised_surface, profile->border, profile->raised_surface, profile->border,
      profile->foreground, profile->editor_background, profile->muted_foreground,
      profile->border, profile->danger);
}

/* Apply the active model profile and update the toolkit's dark preference. */
static UmiStatus apply_active_style(UmiGtk4AppearanceEditor *editor) {
  UmiUiAppearanceProfile profile;
  GtkSettings *settings;
  char reason[192U];
  char *css;
  UmiStatus status;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL || editor->provider == NULL) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  status = umi_ui_appearance_model_active(editor->model, &profile);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  status = umi_ui_appearance_profile_validate(&profile, reason, sizeof(reason));
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  css = profile_css(&profile);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (css == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  gtk_css_provider_load_from_string(editor->provider, css);
  g_free(css);
  settings = gtk_settings_get_for_display(editor->display);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (settings != NULL) {
    g_object_set(settings, "gtk-application-prefer-dark-theme",
                 profile.mode == UMI_UI_THEME_MODE_DARK ||
                     profile.mode == UMI_UI_THEME_MODE_HIGH_CONTRAST,
                 NULL);
  }
  editor->revision += 1U;
  /* Observers run only after the profile is valid and visible. This prevents
   * application chrome from changing when CSS application has failed. */
  if (editor->changed_handler != NULL) {
    editor->changed_handler(&profile, editor->changed_user_data);
  }
  return UMI_STATUS_OK;
}

/* Find a profile's zero-based dropdown row. */
static size_t profile_index(const UmiGtk4AppearanceEditor *editor, const char *profile_id) {
  size_t index;
  UmiUiAppearanceProfile profile;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL || profile_id == NULL)
    return SIZE_MAX;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_ui_appearance_model_count(editor->model); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ui_appearance_model_at(editor->model, index, &profile) == UMI_STATUS_OK &&
        strcmp(profile.profile_id, profile_id) == 0) {
      return index;
    }
  }
  return SIZE_MAX;
}

/* Refresh every editable control from the active profile snapshot. */
static void refresh_controls(UmiGtk4AppearanceEditor *editor) {
  UmiUiAppearanceProfile profile;
  size_t index;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL || umi_ui_appearance_model_active(editor->model, &profile) != UMI_STATUS_OK) {
    return;
  }
  editor->changing_controls = 1;
  gtk_editable_set_text(GTK_EDITABLE(editor->interface_font), profile.interface_font);
  gtk_editable_set_text(GTK_EDITABLE(editor->editor_font), profile.editor_font);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->font_scale), profile.font_scale);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(editor->density_dropdown),
                             (guint)(profile.density - UMI_UI_DENSITY_COMPACT));
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < UMI_GTK4_APPEARANCE_COLOUR_COUNT; ++index) {
    gtk_editable_set_text(GTK_EDITABLE(editor->colour_entries[index]),
                          profile_colour(&profile, (UmiGtk4AppearanceColourField)index));
  }
  index = profile_index(editor, profile.profile_id);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (index != SIZE_MAX) {
    gtk_drop_down_set_selected(GTK_DROP_DOWN(editor->profile_dropdown), (guint)index);
  }
  editor->changing_controls = 0;
}

/* Build the per-application settings file path under the user's config area. */
static char *appearance_settings_path(const char *application_id) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (application_id == NULL || !umi_ui_id_is_valid(application_id)) {
    return NULL;
  }
  return g_build_filename(g_get_user_config_dir(), "umicom", application_id, "appearance.ini",
                          NULL);
}

/* Save the active identifier and editable profile after a successful change. */
static void save_preferences(UmiGtk4AppearanceEditor *editor) {
  UmiUiAppearanceProfile active;
  UmiUiAppearanceProfile custom;
  char encoded[UMI_UI_APPEARANCE_TEXT_CAPACITY];
  GKeyFile *settings;
  char *directory;
  char *data;
  gsize length;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL || editor->settings_path == NULL)
    return;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (umi_ui_appearance_model_active(editor->model, &active) != UMI_STATUS_OK ||
      umi_ui_appearance_model_find(editor->model, "umicom-custom", &custom) != UMI_STATUS_OK ||
      umi_ui_appearance_profile_encode(&custom, encoded, sizeof(encoded)) != UMI_STATUS_OK) {
    return;
  }
  settings = g_key_file_new();
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (settings == NULL)
    return;
  g_key_file_set_string(settings, "Appearance", "ActiveProfile", active.profile_id);
  g_key_file_set_string(settings, "Appearance", "CustomProfile", encoded);
  data = g_key_file_to_data(settings, &length, NULL);
  directory = g_path_get_dirname(editor->settings_path);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (data != NULL && directory != NULL && g_mkdir_with_parents(directory, 0700) == 0) {
    /* Preferences contain presentation values only. They never contain
     * account credentials, API keys or trading data. */
    (void)g_file_set_contents(editor->settings_path, data, (gssize)length, NULL);
  }
  g_free(directory);
  g_free(data);
  g_key_file_unref(settings);
}

/* Restore a valid custom profile and active choice, ignoring damaged files. */
static void load_preferences(UmiGtk4AppearanceEditor *editor) {
  GKeyFile *settings;
  char *active_id;
  char *encoded;
  UmiUiAppearanceProfile profile;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL || editor->settings_path == NULL ||
      !g_file_test(editor->settings_path, G_FILE_TEST_IS_REGULAR)) {
    return;
  }
  settings = g_key_file_new();
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (settings == NULL ||
      !g_key_file_load_from_file(settings, editor->settings_path, G_KEY_FILE_NONE, NULL)) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (settings != NULL)
      g_key_file_unref(settings);
    return;
  }
  encoded = g_key_file_get_string(settings, "Appearance", "CustomProfile", NULL);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (encoded != NULL && umi_ui_appearance_profile_decode(encoded, &profile) == UMI_STATUS_OK &&
      strcmp(profile.profile_id, "umicom-custom") == 0) {
    profile.active = 0;
    profile.built_in = 0;
    profile.locked = 0;
    (void)umi_ui_appearance_model_upsert(editor->model, &profile);
  }
  active_id = g_key_file_get_string(settings, "Appearance", "ActiveProfile", NULL);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (active_id != NULL) {
    (void)umi_ui_appearance_model_set_active(editor->model, active_id);
  }
  g_free(active_id);
  g_free(encoded);
  g_key_file_unref(settings);
}

/* Change the active profile when the user selects a preset row. */
static void on_profile_selected(GObject *object, GParamSpec *property, gpointer user_data) {
  UmiGtk4AppearanceEditor *editor = user_data;
  UmiUiAppearanceProfile profile;
  guint selected;

  (void)property;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL || editor->changing_controls)
    return;
  selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (umi_ui_appearance_model_at(editor->model, (size_t)selected, &profile) == UMI_STATUS_OK) {
    (void)umi_gtk4_appearance_editor_select(editor, profile.profile_id);
  }
}

/* Copy text and colour controls into the editable custom profile atomically. */
static void on_apply_custom(GtkButton *button, gpointer user_data) {
  UmiGtk4AppearanceEditor *editor = user_data;
  UmiUiAppearanceProfile profile;
  const char *text;
  size_t index;
  UmiStatus status;

  (void)button;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL)
    return;
  status = umi_ui_appearance_model_find(editor->model, "umicom-custom", &profile);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return;

  text = gtk_editable_get_text(GTK_EDITABLE(editor->interface_font));
  status = copy_text(profile.interface_font, sizeof(profile.interface_font), text);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK) {
    text = gtk_editable_get_text(GTK_EDITABLE(editor->editor_font));
    status = copy_text(profile.editor_font, sizeof(profile.editor_font), text);
  }
  profile.font_scale = gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->font_scale));
  profile.density =
      (UmiUiDensity)(UMI_UI_DENSITY_COMPACT +
                     gtk_drop_down_get_selected(GTK_DROP_DOWN(editor->density_dropdown)));
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; status == UMI_STATUS_OK && index < UMI_GTK4_APPEARANCE_COLOUR_COUNT; ++index) {
    text = gtk_editable_get_text(GTK_EDITABLE(editor->colour_entries[index]));
    status = copy_text(profile_colour(&profile, (UmiGtk4AppearanceColourField)index),
                       UMI_UI_APPEARANCE_COLOUR_CAPACITY, text);
  }
  profile.active = 1;
  profile.built_in = 0;
  profile.locked = 0;
  profile.revision += 1U;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK) {
    status = umi_gtk4_appearance_editor_apply_custom(editor, &profile);
  }
  gtk_label_set_text(GTK_LABEL(editor->status),
                     status == UMI_STATUS_OK ? "Custom appearance applied and saved."
                                             : "Check fonts and use #RRGGBB for every colour.");
}

/* Build one labelled text field used by the advanced semantic colour grid. */
static GtkWidget *build_colour_entry(UmiGtk4AppearanceEditor *editor, GtkGrid *grid, size_t index) {
  GtkWidget *label;
  GtkWidget *entry;
  char automation_id[64];
  int column;
  int row;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL || grid == NULL || index >= UMI_GTK4_APPEARANCE_COLOUR_COUNT) {
    return NULL;
  }
  label = gtk_label_new(COLOUR_LABELS[index]);
  entry = gtk_entry_new();
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (label == NULL || entry == NULL) {
    /* Widgets are still unparented on this error path, so explicitly
     * consume their floating references before reporting the failure. */
    if (label != NULL) {
      g_object_ref_sink(label);
      g_object_unref(label);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry != NULL) {
      g_object_ref_sink(entry);
      g_object_unref(entry);
    }
    return NULL;
  }
  gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
  gtk_entry_set_max_length(GTK_ENTRY(entry), 7);
  /* Width is part of the shared editable interface in GTK4, so use that
   * public contract instead of the removed entry-specific helper. */
  gtk_editable_set_width_chars(GTK_EDITABLE(entry), 9);
  /* A numbered semantic field remains stable when the grid is rearranged. */
  (void)snprintf(
      automation_id,
      sizeof(automation_id),
      "umicom.appearance.colour.%zu",
      index);
  (void)umi_gtk4_automation_tag_widget(entry, automation_id);
  column = index < 7U ? 0 : 2;
  row = (int)(index % 7U);
  gtk_grid_attach(grid, label, column, row, 1, 1);
  gtk_grid_attach(grid, entry, column + 1, row, 1, 1);
  editor->colour_entries[index] = entry;
  return entry;
}

/* Build the popover once; its controls are refreshed whenever a preset changes. */
static GtkWidget *build_editor_popover(UmiGtk4AppearanceEditor *editor) {
  static const char *DENSITIES[] = {"Compact", "Comfortable", "Spacious", NULL};
  GtkWidget *popover = gtk_popover_new();
  GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  GtkWidget *title = gtk_label_new("Appearance");
  GtkWidget *fonts = gtk_grid_new();
  GtkWidget *advanced = gtk_expander_new("Advanced colours");
  GtkWidget *colours = gtk_grid_new();
  GtkWidget *apply = gtk_button_new_with_label("Apply Custom Appearance");
  GtkStringList *profiles = gtk_string_list_new(NULL);
  UmiUiAppearanceProfile profile;
  size_t index;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (popover == NULL || root == NULL || title == NULL || fonts == NULL || advanced == NULL ||
      colours == NULL || apply == NULL || profiles == NULL) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profiles != NULL)
      g_object_unref(profiles);
    return popover;
  }
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_ui_appearance_model_count(editor->model); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ui_appearance_model_at(editor->model, index, &profile) == UMI_STATUS_OK) {
      gtk_string_list_append(profiles, profile.label);
    }
  }
  editor->profile_dropdown =
      umi_ui_gtk4_drop_down_new_take_string_list(profiles);
  editor->density_dropdown = gtk_drop_down_new_from_strings(DENSITIES);
  editor->font_scale = gtk_spin_button_new_with_range(0.75, 2.0, 0.05);
  editor->interface_font = gtk_entry_new();
  editor->editor_font = gtk_entry_new();
  editor->status = gtk_label_new("");

  /* Tag each preference control before it is placed into the popover. */
  (void)umi_gtk4_automation_tag_widget(
      editor->profile_dropdown,
      "umicom.appearance.profile");
  (void)umi_gtk4_automation_tag_widget(
      editor->density_dropdown,
      "umicom.appearance.density");
  (void)umi_gtk4_automation_tag_widget(
      editor->font_scale,
      "umicom.appearance.font-scale");
  (void)umi_gtk4_automation_tag_widget(
      editor->interface_font,
      "umicom.appearance.interface-font");
  (void)umi_gtk4_automation_tag_widget(
      editor->editor_font,
      "umicom.appearance.content-font");
  (void)umi_gtk4_automation_tag_widget(
      apply,
      "umicom.appearance.apply");

  /* Stop before calling GTK APIs when a control allocation failed. This
   * keeps a rare low-memory condition from becoming a null dereference. */
  if (editor->profile_dropdown == NULL || editor->density_dropdown == NULL ||
      editor->font_scale == NULL || editor->interface_font == NULL || editor->editor_font == NULL ||
      editor->status == NULL) {
    g_object_ref_sink(popover);
    g_object_unref(popover);
    return NULL;
  }

  gtk_widget_add_css_class(root, "umicom-appearance-editor");
  gtk_widget_add_css_class(title, "title-4");
  gtk_widget_add_css_class(editor->status, "dim-label");
  gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
  gtk_label_set_xalign(GTK_LABEL(editor->status), 0.0F);
  gtk_label_set_wrap(GTK_LABEL(editor->status), TRUE);
  gtk_widget_set_size_request(root, 520, -1);
  gtk_widget_set_margin_top(root, 12);
  gtk_widget_set_margin_bottom(root, 12);
  gtk_widget_set_margin_start(root, 12);
  gtk_widget_set_margin_end(root, 12);

  gtk_grid_set_row_spacing(GTK_GRID(fonts), 7U);
  gtk_grid_set_column_spacing(GTK_GRID(fonts), 8U);
  gtk_grid_attach(GTK_GRID(fonts), gtk_label_new("Theme"), 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(fonts), editor->profile_dropdown, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(fonts), gtk_label_new("Density"), 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(fonts), editor->density_dropdown, 1, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(fonts), gtk_label_new("Text scale"), 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(fonts), editor->font_scale, 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID(fonts), gtk_label_new("Interface font"), 0, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(fonts), editor->interface_font, 1, 3, 1, 1);
  gtk_grid_attach(GTK_GRID(fonts), gtk_label_new("Content font"), 0, 4, 1, 1);
  gtk_grid_attach(GTK_GRID(fonts), editor->editor_font, 1, 4, 1, 1);

  gtk_grid_set_row_spacing(GTK_GRID(colours), 6U);
  gtk_grid_set_column_spacing(GTK_GRID(colours), 8U);
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < UMI_GTK4_APPEARANCE_COLOUR_COUNT; ++index) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (build_colour_entry(editor, GTK_GRID(colours), index) == NULL) {
      g_object_ref_sink(popover);
      g_object_unref(popover);
      return NULL;
    }
  }
  gtk_expander_set_child(GTK_EXPANDER(advanced), colours);
  gtk_widget_add_css_class(apply, "suggested-action");
  gtk_box_append(GTK_BOX(root), title);
  gtk_box_append(GTK_BOX(root), fonts);
  gtk_box_append(GTK_BOX(root), advanced);
  gtk_box_append(GTK_BOX(root), editor->status);
  gtk_box_append(GTK_BOX(root), apply);
  gtk_popover_set_child(GTK_POPOVER(popover), root);
  g_signal_connect(editor->profile_dropdown, "notify::selected", G_CALLBACK(on_profile_selected),
                   editor);
  g_signal_connect(apply, "clicked", G_CALLBACK(on_apply_custom), editor);
  return popover;
}

/* Return creation defaults shared by Trader and future suite applications. */
UmiGtk4AppearanceEditorConfig
umi_gtk4_appearance_editor_config_default(const char *application_id) {
  UmiGtk4AppearanceEditorConfig config;

  config.application_id = application_id;
  config.initial_profile_id = "umicom-dark";
  return config;
}

/* Create the model, preference store, scoped provider and compact menu button. */
UmiStatus umi_gtk4_appearance_editor_create(GtkWidget *scope_root,
                                            const UmiGtk4AppearanceEditorConfig *config,
                                            UmiGtk4AppearanceEditor **out_editor) {
  UmiGtk4AppearanceEditor *editor;
  GtkWidget *popover;
  UmiStatus status;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (scope_root == NULL || config == NULL || out_editor == NULL ||
      config->application_id == NULL || !umi_ui_id_is_valid(config->application_id)) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  *out_editor = NULL;
  editor = calloc(1U, sizeof(*editor));
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  editor->scope_root = scope_root;
  editor->settings_path = appearance_settings_path(config->application_id);
  editor->revision = 1U;
  status = umi_ui_appearance_model_create(&editor->model);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    goto fail;
  status = umi_ui_appearance_catalogue_populate(editor->model);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    goto fail;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (config->initial_profile_id != NULL) {
    (void)umi_ui_appearance_model_set_active(editor->model, config->initial_profile_id);
  }
  load_preferences(editor);

  editor->display = gtk_widget_get_display(scope_root);
  editor->provider = gtk_css_provider_new();
  editor->button = gtk_menu_button_new();
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor->display == NULL || editor->provider == NULL || editor->button == NULL) {
    status = UMI_STATUS_OUT_OF_MEMORY;
    goto fail;
  }
  /* The editor keeps one reference while the application header keeps its
   * normal parent reference. Either owner can therefore be released first. */
  g_object_ref_sink(editor->button);
  (void)umi_gtk4_automation_tag_widget(
      editor->button,
      "umicom.appearance.menu");
  gtk_widget_add_css_class(scope_root, "umicom-appearance-scope");
  gtk_widget_add_css_class(editor->button, "umicom-appearance-button");
  gtk_menu_button_set_label(GTK_MENU_BUTTON(editor->button), "Appearance");
  gtk_widget_set_tooltip_text(editor->button,
                              "Change theme, fonts, text size, density and colours");
  popover = build_editor_popover(editor);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (popover == NULL) {
    status = UMI_STATUS_OUT_OF_MEMORY;
    goto fail;
  }
  gtk_menu_button_set_popover(GTK_MENU_BUTTON(editor->button), popover);
  gtk_style_context_add_provider_for_display(editor->display, GTK_STYLE_PROVIDER(editor->provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 10U);
  refresh_controls(editor);
  status = apply_active_style(editor);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    goto fail;
  *out_editor = editor;
  return UMI_STATUS_OK;

fail:
  umi_gtk4_appearance_editor_destroy(editor);
  return status;
}

/* Detach the global provider before releasing model and path ownership. */
void umi_gtk4_appearance_editor_destroy(UmiGtk4AppearanceEditor *editor) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL)
    return;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor->display != NULL && editor->provider != NULL) {
    gtk_style_context_remove_provider_for_display(editor->display,
                                                  GTK_STYLE_PROVIDER(editor->provider));
  }
  g_clear_object(&editor->provider);
  g_clear_object(&editor->button);
  umi_ui_appearance_model_destroy(editor->model);
  editor->model = NULL;
  g_free(editor->settings_path);
  editor->settings_path = NULL;
  free(editor);
}

/* Return the borrowed button that an application places in its header. */
GtkWidget *umi_gtk4_appearance_editor_widget(UmiGtk4AppearanceEditor *editor) {
  return editor != NULL ? editor->button : NULL;
}

/* Store one lightweight observer and publish the current profile immediately
 * so newly attached headers do not wait for the user's next theme change. */
UmiStatus umi_gtk4_appearance_editor_set_changed_handler(
    UmiGtk4AppearanceEditor *editor,
    UmiGtk4AppearanceChangedHandler handler,
    void *user_data) {
  UmiUiAppearanceProfile profile;
  UmiStatus status;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  editor->changed_handler = handler;
  editor->changed_user_data = handler != NULL ? user_data : NULL;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (handler == NULL) {
    return UMI_STATUS_OK;
  }
  status = umi_ui_appearance_model_active(editor->model, &profile);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK) {
    handler(&profile, user_data);
  }
  return status;
}

/* Select, render and persist one known profile. */
UmiStatus umi_gtk4_appearance_editor_select(UmiGtk4AppearanceEditor *editor,
                                            const char *profile_id) {
  UmiStatus status;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL || profile_id == NULL) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  status = umi_ui_appearance_model_set_active(editor->model, profile_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = apply_active_style(editor);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK) {
    refresh_controls(editor);
    save_preferences(editor);
  }
  return status;
}

/* Validate and publish a complete editable custom profile. */
UmiStatus umi_gtk4_appearance_editor_apply_custom(UmiGtk4AppearanceEditor *editor,
                                                  const UmiUiAppearanceProfile *profile) {
  UmiUiAppearanceProfile candidate;
  UmiStatus status;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL || profile == NULL || strcmp(profile->profile_id, "umicom-custom") != 0) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  candidate = *profile;
  candidate.active = 1;
  candidate.built_in = 0;
  candidate.locked = 0;
  status = umi_ui_appearance_model_upsert(editor->model, &candidate);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK) {
    status = umi_ui_appearance_model_set_active(editor->model, candidate.profile_id);
  }
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = apply_active_style(editor);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK) {
    refresh_controls(editor);
    save_preferences(editor);
  }
  return status;
}

/* Forward a value snapshot from the editor-owned appearance model. */
UmiStatus umi_gtk4_appearance_editor_active(const UmiGtk4AppearanceEditor *editor,
                                            UmiUiAppearanceProfile *out_profile) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL || out_profile == NULL) {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  return umi_ui_appearance_model_active(editor->model, out_profile);
}

/* Copy compact state used by application tests and status surfaces. */
UmiGtk4AppearanceEditorSnapshot
umi_gtk4_appearance_editor_snapshot(const UmiGtk4AppearanceEditor *editor) {
  UmiGtk4AppearanceEditorSnapshot snapshot;
  UmiUiAppearanceProfile profile;

  (void)memset(&snapshot, 0, sizeof(snapshot));
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (editor == NULL)
    return snapshot;
  snapshot.profile_count = umi_ui_appearance_model_count(editor->model);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (umi_ui_appearance_model_active(editor->model, &profile) == UMI_STATUS_OK) {
    (void)copy_text(snapshot.active_profile_id, sizeof(snapshot.active_profile_id),
                    profile.profile_id);
    snapshot.density = profile.density;
    snapshot.font_scale = profile.font_scale;
  }
  snapshot.revision = editor->revision;
  return snapshot;
}
