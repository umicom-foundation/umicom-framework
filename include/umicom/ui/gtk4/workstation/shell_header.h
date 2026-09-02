/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/shell_header.h
 *
 * PURPOSE:
 *   Render the reusable application identity shown in an Umicom workstation
 *   header. The component keeps the product name as accessible native text
 *   and selects a contrast-aware SVG mark from the active appearance profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_SHELL_HEADER_H
#define UMICOM_UI_GTK4_WORKSTATION_SHELL_HEADER_H

#include <stdbool.h>
#include <stdint.h>

#include <gtk/gtk.h>

#include "umicom/ui/appearance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the gtk4 workstation shell header data shared with callers of this public
 * contract.
 */
typedef struct UmiGtk4WorkstationShellHeader
    UmiGtk4WorkstationShellHeader;

/**
 * Text and resource values used while creating one application identity.
 *
 * All strings are borrowed during creation and copied into Framework-owned
 * storage. `resource_root` is optional. When it is empty, relative resources
 * are looked up beside the running executable so installed applications do
 * not depend on the terminal's current directory.
 */
typedef struct UmiGtk4WorkstationShellHeaderConfig {
    const char *application_id;
    const char *title;
    const char *subtitle;
    const char *mode_badge;
    const char *resource_root;
    bool compact;
} UmiGtk4WorkstationShellHeaderConfig;

/**
 * A value copy of the identity currently presented by the native header.
 *
 * Tests, status panels and application diagnostics can read this structure
 * without keeping pointers to GTK widgets or mutable Framework state.
 */
typedef struct UmiGtk4WorkstationShellHeaderSnapshot {
    char application_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char subtitle[UMI_UI_TEXT_CAPACITY];
    char mode_badge[UMI_UI_TEXT_CAPACITY];
    char icon_resource[UMI_UI_APPEARANCE_RESOURCE_CAPACITY];
    int icon_visible;
    int compact;
    uint64_t revision;
} UmiGtk4WorkstationShellHeaderSnapshot;

/** Return safe creation values for one named Umicom application. */
UmiGtk4WorkstationShellHeaderConfig
umi_gtk4_ws_shell_header_config_default(
    const char *application_id,
    const char *title);

/**
 * Create an updateable identity controller and its GTK widget.
 *
 * The returned controller stores no business state. The GTK widget becomes
 * owned by its normal GTK parent after the caller appends it to a container.
 */
UmiStatus umi_gtk4_ws_shell_header_create_managed(
    const UmiGtk4WorkstationShellHeaderConfig *config,
    UmiGtk4WorkstationShellHeader **out_header);

/** Release the controller after, or immediately before, its widget tree. */
void umi_gtk4_ws_shell_header_destroy(
    UmiGtk4WorkstationShellHeader *header);

/** Borrow the GTK root that the application places in its top-level bar. */
GtkWidget *umi_gtk4_ws_shell_header_widget(
    UmiGtk4WorkstationShellHeader *header);

/**
 * Select the SVG mark described by a validated appearance profile.
 *
 * A missing packaged image hides only the image. The native application name
 * remains visible, so a packaging problem never removes the whole identity.
 */
UmiStatus umi_gtk4_ws_shell_header_apply_appearance(
    UmiGtk4WorkstationShellHeader *header,
    const UmiUiAppearanceProfile *profile);

/** Update the readable title, workspace subtitle and optional mode badge. */
UmiStatus umi_gtk4_ws_shell_header_set_text(
    UmiGtk4WorkstationShellHeader *header,
    const char *title,
    const char *subtitle,
    const char *mode_badge);

/** Copy observable identity state without exposing mutable GTK objects. */
UmiGtk4WorkstationShellHeaderSnapshot
umi_gtk4_ws_shell_header_snapshot(
    const UmiGtk4WorkstationShellHeader *header);

/**
 * Create the original label-only header API retained for source compatibility.
 *
 * New application workstations should use `create_managed` so their icon can
 * follow appearance changes. Existing callers keep the same ownership rules.
 */
GtkWidget *umi_gtk4_ws_shell_header_create(
    const char *title,
    const char *subtitle,
    const char *mode_badge,
    bool compact);

#ifdef __cplusplus
}
#endif

#endif
