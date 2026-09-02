/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/theme_resolution.h
 *
 * PURPOSE:
 *   Record deterministic system/application/workspace/component theme resolution evidence.
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
#ifndef UMICOM_UI_APPEARANCE_THEME_RESOLUTION_H
#define UMICOM_UI_APPEARANCE_THEME_RESOLUTION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance theme resolution data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceThemeResolution {
    char requested_pack_id[UMI_APPEARANCE_ID_CAPACITY];
    char resolved_pack_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiAppearanceScope winning_scope;
    uint32_t inherited_layers;
} UmiAppearanceThemeResolution;

/* Initialise one theme resolution record with deterministic defaults. */
UmiStatus umi_appearance_theme_resolution_init(UmiAppearanceThemeResolution *item);
/* Validate the required production invariants for this theme resolution. */
int umi_appearance_theme_resolution_is_valid(const UmiAppearanceThemeResolution *item);
/* Resolve precedence from component through workspace/application to system. */
UmiStatus umi_appearance_theme_resolution_choose(UmiAppearanceThemeResolution *item, const char *system_id, const char *application_id, const char *workspace_id, const char *component_id);

#ifdef __cplusplus
}
#endif
#endif
