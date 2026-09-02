/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/breakpoint_catalogue.h
 *
 * PURPOSE:
 *   Catalogue canonical Design-System breakpoints and resolve the active viewport range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_BREAKPOINT_CATALOGUE_H
#define UMICOM_UI_ADAPTIVE_BREAKPOINT_CATALOGUE_H
#include "umicom/ui/adaptive/types.h"
#include "umicom/ui/design/breakpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive breakpoint catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiAdaptiveBreakpointCatalogue {
    UmiDesignBreakpoint items[UMI_ADAPTIVE_MAX_BREAKPOINTS];
    size_t count;
} UmiAdaptiveBreakpointCatalogue;
/* Initialise an empty breakpoint catalogue. */
void umi_adaptive_breakpoint_catalogue_init(UmiAdaptiveBreakpointCatalogue *catalogue);
/* Add a unique canonical breakpoint definition. */
UmiStatus umi_adaptive_breakpoint_catalogue_add(UmiAdaptiveBreakpointCatalogue *catalogue,
                                                const UmiDesignBreakpoint *breakpoint);
/* Resolve the first matching canonical breakpoint for a logical width. */
UmiStatus umi_adaptive_breakpoint_catalogue_resolve(const UmiAdaptiveBreakpointCatalogue *catalogue,
                                                    int32_t width,
                                                    UmiDesignBreakpoint *out_breakpoint);

#ifdef __cplusplus
}
#endif
#endif
