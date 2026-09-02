/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/starter_catalogue.c
 *
 * PURPOSE:
 *   Implement the starter catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/starter_catalogue.c
 *
 * PURPOSE:
 *   Maintain Framework-owned application starter definitions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/starter_catalogue.h"


#include <string.h>
/*
 * Initialise bootstrap starter catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_starter_catalogue_init(UmiBootstrapStarterCatalogue *catalogue) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue != NULL) memset(catalogue, 0, sizeof(*catalogue));
}
/*
 * Find bootstrap starter catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiBootstrapStarterDescriptor *umi_bootstrap_starter_catalogue_find(
    const UmiBootstrapStarterCatalogue *catalogue,
    const char *starter_id) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || starter_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < catalogue->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(catalogue->items[i].starter_id, starter_id) == 0)
            return &catalogue->items[i];
    return NULL;
}
/*
 * Add bootstrap starter catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_starter_catalogue_add(
    UmiBootstrapStarterCatalogue *catalogue,
    const UmiBootstrapStarterDescriptor *starter) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || starter == NULL ||
        !umi_bootstrap_id_valid(starter->starter_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_bootstrap_starter_catalogue_find(catalogue, starter->starter_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *starter;
    return UMI_STATUS_OK;
}
