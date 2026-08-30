/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/breakpoint_inheritance.h
 *
 * PURPOSE:
 *   Resolve bounded parent chains for adaptive variants without accepting inheritance cycles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_BREAKPOINT_INHERITANCE_H
#define UMICOM_DESIGNER_ADAPTIVE_BREAKPOINT_INHERITANCE_H
#include "umicom/designer/adaptive/variant_set.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerBreakpointInheritance {
    char variant_ids[UMI_DESIGNER_ADAPTIVE_MAX_VARIANTS][UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    size_t count;
} UmiDesignerBreakpointInheritance;
/* Resolve variant-to-parent inheritance from leaf to root and reject cycles. */
UmiStatus umi_designer_breakpoint_inheritance_resolve(const UmiDesignerAdaptiveVariantSet *set,
                                                      const char *leaf_variant_id,
                                                      UmiDesignerBreakpointInheritance *out_chain);

#ifdef __cplusplus
}
#endif
#endif
