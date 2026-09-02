/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/breakpoint_catalogue.c
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
#include "umicom/ui/adaptive/breakpoint_catalogue.h"
#include <string.h>

/* Reset the bounded catalogue for deterministic application startup. */
void umi_adaptive_breakpoint_catalogue_init(UmiAdaptiveBreakpointCatalogue *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue != NULL) {
        memset(catalogue, 0, sizeof *catalogue);
    }
}

/* Add breakpoints by value while rejecting duplicates and capacity overflow. */
UmiStatus umi_adaptive_breakpoint_catalogue_add(UmiAdaptiveBreakpointCatalogue *catalogue,
                                                const UmiDesignBreakpoint *breakpoint)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || breakpoint == NULL || breakpoint->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].id, breakpoint->id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_ADAPTIVE_MAX_BREAKPOINTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    catalogue->items[catalogue->count++] = *breakpoint;
    return UMI_STATUS_OK;
}

/* Resolve using canonical breakpoint matching so CSS-like logic is not duplicated. */
UmiStatus umi_adaptive_breakpoint_catalogue_resolve(const UmiAdaptiveBreakpointCatalogue *catalogue,
                                                    int32_t width,
                                                    UmiDesignBreakpoint *out_breakpoint)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_breakpoint == NULL || width < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_design_breakpoint_matches(&catalogue->items[index], width) != 0) {
            *out_breakpoint = catalogue->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
