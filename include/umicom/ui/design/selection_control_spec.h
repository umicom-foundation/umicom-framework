/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/selection_control_spec.h
 *
 * PURPOSE:
 *   Define checkbox, radio, switch and drop-down selection semantics.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_SELECTION_CONTROL_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_SELECTION_CONTROL_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design selection control spec data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignSelectionControlSpec {
    UmiUiComponentKind kind;
    int selected;
    int multiple;
    int tri_state;
} UmiDesignSelectionControlSpec;

/* Initialise the semantic selection control spec specification. */
UmiStatus umi_design_selection_control_spec_init(UmiDesignSelectionControlSpec *spec, UmiUiComponentKind kind, int selected, int multiple, int tri_state);
/* Return one when the semantic specification is internally consistent. */
int umi_design_selection_control_spec_valid(const UmiDesignSelectionControlSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
