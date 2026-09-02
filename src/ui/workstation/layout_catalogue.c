/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded catalogue of reusable and user-customisable workstation layout templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/layout_catalogue.h"

/*
 * Initialise ws layout catalogue from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_layout_catalogue_init(UmiWsLayoutCatalogue *catalogue) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue != NULL) *catalogue = (UmiWsLayoutCatalogue){0};
}

/*
 * Provide the ws layout catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_layout_catalogue_upsert(UmiWsLayoutCatalogue *catalogue, const UmiWsLayoutTemplate *layout) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || layout == NULL || !umi_ws_id_valid(layout->layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->layouts[index].layout_id, layout->layout_id) == 0) {
            catalogue->layouts[index] = *layout;
            ++catalogue->revision;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_WS_MAX_LAYOUTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->layouts[catalogue->count++] = *layout;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}

/*
 * Find ws layout catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsLayoutTemplate *umi_ws_layout_catalogue_find(const UmiWsLayoutCatalogue *catalogue, const char *layout_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || !umi_ws_id_valid(layout_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->layouts[index].layout_id, layout_id) == 0) return &catalogue->layouts[index];
    }
    return NULL;
}

/*
 * Provide the ws layout catalogue count domain operation used by this module and its
 * client applications.
 */
size_t umi_ws_layout_catalogue_count_domain(const UmiWsLayoutCatalogue *catalogue, UmiWsApplicationDomain domain) {
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (catalogue->layouts[index].domain == domain) ++count;
    return count;
}
