/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/theme_scope.h
 *
 * PURPOSE:
 *   Describe the semantic scope at which a theme override is applied.
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
#ifndef UMICOM_UI_APPEARANCE_THEME_SCOPE_H
#define UMICOM_UI_APPEARANCE_THEME_SCOPE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceThemeScope {
    char scope_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiAppearanceScope scope;
    char owner_id[UMI_APPEARANCE_ID_CAPACITY];
} UmiAppearanceThemeScope;

/* Initialise one theme scope record with deterministic defaults. */
UmiStatus umi_appearance_theme_scope_init(UmiAppearanceThemeScope *item);
/* Validate the required production invariants for this theme scope. */
int umi_appearance_theme_scope_is_valid(const UmiAppearanceThemeScope *item);

#ifdef __cplusplus
}
#endif
#endif
