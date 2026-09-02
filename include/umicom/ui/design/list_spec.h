/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/list_spec.h
 *
 * PURPOSE:
 *   Define virtualised list selection, estimated size and row-density semantics.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_LIST_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_LIST_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design list spec data shared with callers of this public contract.
 */
typedef struct UmiDesignListSpec {
    size_t estimated_items;
    UmiDesignDensity density;
    int virtualised;
    int multi_select;
} UmiDesignListSpec;

/* Initialise the semantic list spec specification. */
UmiStatus umi_design_list_spec_init(UmiDesignListSpec *spec, size_t estimated_items, UmiDesignDensity density, int virtualised, int multi_select);
/* Return one when the semantic specification is internally consistent. */
int umi_design_list_spec_valid(const UmiDesignListSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
