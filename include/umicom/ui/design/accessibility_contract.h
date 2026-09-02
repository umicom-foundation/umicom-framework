/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/accessibility_contract.h
 *
 * PURPOSE:
 *   Capture keyboard, focus, labelling and minimum-target accessibility requirements per component.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_ACCESSIBILITY_CONTRACT_H
#define INCLUDE_UMICOM_UI_DESIGN_ACCESSIBILITY_CONTRACT_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the design accessibility contract data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignAccessibilityContract { char accessible_name[UMI_DESIGN_TEXT_CAPACITY]; int keyboard_operable; int focus_visible; int screen_reader_named; int32_t min_target_width; int32_t min_target_height; } UmiDesignAccessibilityContract;
/* Initialise a reusable accessibility contract for an interactive semantic component. */
UmiStatus umi_design_accessibility_contract_init(UmiDesignAccessibilityContract *contract, const char *accessible_name, int32_t min_target_width, int32_t min_target_height);
/* Return one when minimum labelling, keyboard and target-size requirements are met. */
int umi_design_accessibility_contract_passes(const UmiDesignAccessibilityContract *contract);

#ifdef __cplusplus
}
#endif

#endif
