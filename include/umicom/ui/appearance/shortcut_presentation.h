/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/shortcut_presentation.h
 *
 * PURPOSE:
 *   Describe platform-neutral command shortcut hints for menus, toolbars and palettes.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_SHORTCUT_PRESENTATION_H
#define UMICOM_UI_APPEARANCE_SHORTCUT_PRESENTATION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance shortcut presentation data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceShortcutPresentation {
    char action_id[UMI_APPEARANCE_ID_CAPACITY];
    char accelerator_id[UMI_APPEARANCE_ID_CAPACITY];
    char display_text[UMI_APPEARANCE_TEXT_CAPACITY];
    bool discoverable;
} UmiAppearanceShortcutPresentation;

/* Initialise one shortcut presentation record with deterministic defaults. */
UmiStatus umi_appearance_shortcut_presentation_init(UmiAppearanceShortcutPresentation *item);
/* Validate the required production invariants for this shortcut presentation. */
int umi_appearance_shortcut_presentation_is_valid(const UmiAppearanceShortcutPresentation *item);

#ifdef __cplusplus
}
#endif
#endif
