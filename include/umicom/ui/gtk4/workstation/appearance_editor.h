/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/appearance_editor.h
 *
 * PURPOSE:
 *   Expose a reusable GTK4 appearance chooser for application themes, fonts,
 *   density, semantic colours and persistent user preferences.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_APPEARANCE_EDITOR_H
#define UMICOM_UI_GTK4_WORKSTATION_APPEARANCE_EDITOR_H

#include <gtk/gtk.h>

#include "umicom/ui/appearance.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiGtk4AppearanceEditor UmiGtk4AppearanceEditor;

/** Creation values are borrowed only during creation and copied internally. */
typedef struct UmiGtk4AppearanceEditorConfig {
  const char *application_id;
  const char *initial_profile_id;
} UmiGtk4AppearanceEditorConfig;

/** A snapshot lets thin applications observe appearance without GTK access. */
typedef struct UmiGtk4AppearanceEditorSnapshot {
  char active_profile_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
  size_t profile_count;
  UmiUiDensity density;
  double font_scale;
  uint64_t revision;
} UmiGtk4AppearanceEditorSnapshot;

/** Return safe creation defaults with the shared dark profile selected. */
UmiGtk4AppearanceEditorConfig umi_gtk4_appearance_editor_config_default(const char *application_id);

/** Create the editor and apply its initial profile to the supplied root. */
UmiStatus umi_gtk4_appearance_editor_create(GtkWidget *scope_root,
                                            const UmiGtk4AppearanceEditorConfig *config,
                                            UmiGtk4AppearanceEditor **out_editor);

/** Remove the style provider and release the editor's owned model and paths. */
void umi_gtk4_appearance_editor_destroy(UmiGtk4AppearanceEditor *editor);

/** Borrow the compact Appearance menu button for an application header. */
GtkWidget *umi_gtk4_appearance_editor_widget(UmiGtk4AppearanceEditor *editor);

/** Select a standard or custom profile and save the user's choice. */
UmiStatus umi_gtk4_appearance_editor_select(UmiGtk4AppearanceEditor *editor,
                                            const char *profile_id);

/** Replace the editable custom profile after normal Framework validation. */
UmiStatus umi_gtk4_appearance_editor_apply_custom(UmiGtk4AppearanceEditor *editor,
                                                  const UmiUiAppearanceProfile *profile);

/** Copy the complete active profile for tests and non-GTK application code. */
UmiStatus umi_gtk4_appearance_editor_active(const UmiGtk4AppearanceEditor *editor,
                                            UmiUiAppearanceProfile *out_profile);

/** Copy small observable facts without exposing mutable editor state. */
UmiGtk4AppearanceEditorSnapshot
umi_gtk4_appearance_editor_snapshot(const UmiGtk4AppearanceEditor *editor);

#ifdef __cplusplus
}
#endif

#endif
