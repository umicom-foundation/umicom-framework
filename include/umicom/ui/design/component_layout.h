/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/component_layout.h
 *
 * PURPOSE:
 *   Define common row, column, stack and grid layout semantics for compound components.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_COMPONENT_LAYOUT_H
#define INCLUDE_UMICOM_UI_DESIGN_COMPONENT_LAYOUT_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named design component layout kind values accepted by this public contract.
 */
typedef enum UmiDesignComponentLayoutKind { UMI_DESIGN_LAYOUT_ROW=1, UMI_DESIGN_LAYOUT_COLUMN=2, UMI_DESIGN_LAYOUT_STACK=3, UMI_DESIGN_LAYOUT_GRID=4 } UmiDesignComponentLayoutKind;
/**
 * Represent the design component layout data shared with callers of this public contract.
 */
typedef struct UmiDesignComponentLayout { UmiDesignComponentLayoutKind kind; uint16_t columns; double gap; int wrap; } UmiDesignComponentLayout;
/* Initialise a validated compound-component layout description. */
UmiStatus umi_design_component_layout_init(UmiDesignComponentLayout *layout, UmiDesignComponentLayoutKind kind, uint16_t columns, double gap, int wrap);

#ifdef __cplusplus
}
#endif

#endif
