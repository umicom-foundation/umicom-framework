/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/component_style_binding.h
 *
 * PURPOSE:
 *   Bind a semantic component and state map to a Framework style identity.
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
#ifndef UMICOM_UI_APPEARANCE_COMPONENT_STYLE_BINDING_H
#define UMICOM_UI_APPEARANCE_COMPONENT_STYLE_BINDING_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance component style binding data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceComponentStyleBinding {
    char component_id[UMI_APPEARANCE_ID_CAPACITY];
    char style_id[UMI_APPEARANCE_ID_CAPACITY];
    char state_map_id[UMI_APPEARANCE_ID_CAPACITY];
} UmiAppearanceComponentStyleBinding;

/* Initialise one component style binding record with deterministic defaults. */
UmiStatus umi_appearance_component_style_binding_init(UmiAppearanceComponentStyleBinding *item);
/* Validate the required production invariants for this component style binding. */
int umi_appearance_component_style_binding_is_valid(const UmiAppearanceComponentStyleBinding *item);

#ifdef __cplusplus
}
#endif
#endif
