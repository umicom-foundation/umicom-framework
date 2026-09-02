/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/tree_spec.h
 *
 * PURPOSE:
 *   Define virtualised hierarchical tree depth, selection and checkbox semantics.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_TREE_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_TREE_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design tree spec data shared with callers of this public contract.
 */
typedef struct UmiDesignTreeSpec {
    uint16_t maximum_depth;
    int virtualised;
    int multi_select;
    int checkboxes;
} UmiDesignTreeSpec;

/* Initialise the semantic tree spec specification. */
UmiStatus umi_design_tree_spec_init(UmiDesignTreeSpec *spec, uint16_t maximum_depth, int virtualised, int multi_select, int checkboxes);
/* Return one when the semantic specification is internally consistent. */
int umi_design_tree_spec_valid(const UmiDesignTreeSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
