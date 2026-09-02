/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/perspective_catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded perspective catalogue so products can contribute task semantics without owning layout infrastructure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/perspective_catalogue.h"

/*
 * Initialise ws perspective catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ws_perspective_catalogue_init(UmiWsPerspectiveCatalogue *catalogue) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue != NULL) *catalogue = (UmiWsPerspectiveCatalogue){0};
}

/*
 * Provide the ws perspective catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_perspective_catalogue_upsert(UmiWsPerspectiveCatalogue *catalogue,
                                               const UmiWsPerspectiveTemplate *perspective) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || perspective == NULL || !umi_ws_id_valid(perspective->perspective_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].perspective_id, perspective->perspective_id) == 0) {
            catalogue->items[index] = *perspective;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_WS_MAX_PERSPECTIVES) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *perspective;
    return UMI_STATUS_OK;
}

/*
 * Find ws perspective catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWsPerspectiveTemplate *umi_ws_perspective_catalogue_find(const UmiWsPerspectiveCatalogue *catalogue,
                                                                  const char *perspective_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || !umi_ws_id_valid(perspective_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].perspective_id, perspective_id) == 0) return &catalogue->items[index];
    }
    return NULL;
}
