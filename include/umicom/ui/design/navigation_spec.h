/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/navigation_spec.h
 *
 * PURPOSE:
 *   Define rail, sidebar and bottom-navigation semantics across desktop, web and mobile compositions.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_NAVIGATION_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_NAVIGATION_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design navigation spec data shared with callers of this public contract.
 */
typedef struct UmiDesignNavigationSpec {
    UmiUiPlacement placement;
    uint16_t item_count;
    int collapsible;
    int show_labels;
} UmiDesignNavigationSpec;

/* Initialise the semantic navigation spec specification. */
UmiStatus umi_design_navigation_spec_init(UmiDesignNavigationSpec *spec, UmiUiPlacement placement, uint16_t item_count, int collapsible, int show_labels);
/* Return one when the semantic specification is internally consistent. */
int umi_design_navigation_spec_valid(const UmiDesignNavigationSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
