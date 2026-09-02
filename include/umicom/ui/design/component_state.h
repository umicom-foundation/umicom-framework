/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_state.h
 *
 * PURPOSE:
 *   Represent composable component state flags used consistently by all frontend renderers.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_STATE_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_STATE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named design component state flag values accepted by this public contract.
 */
typedef enum UmiDesignComponentStateFlag { UMI_DESIGN_STATE_NONE=0U, UMI_DESIGN_STATE_HOVERED=1U<<0, UMI_DESIGN_STATE_PRESSED=1U<<1, UMI_DESIGN_STATE_FOCUSED=1U<<2, UMI_DESIGN_STATE_DISABLED=1U<<3, UMI_DESIGN_STATE_SELECTED=1U<<4, UMI_DESIGN_STATE_INVALID=1U<<5, UMI_DESIGN_STATE_BUSY=1U<<6 } UmiDesignComponentStateFlag;
/**
 * Represent the design component state data shared with callers of this public contract.
 */
typedef struct UmiDesignComponentState { uint32_t flags; } UmiDesignComponentState;
/* Add a state flag without disturbing existing component state. */
void umi_design_component_state_add(UmiDesignComponentState *state, UmiDesignComponentStateFlag flag);
/* Return one when a component state contains the requested flag. */
int umi_design_component_state_has(const UmiDesignComponentState *state, UmiDesignComponentStateFlag flag);

#ifdef __cplusplus
}
#endif

#endif
