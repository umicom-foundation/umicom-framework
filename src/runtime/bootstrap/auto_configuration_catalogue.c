/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/auto_configuration_catalogue.c
 *
 * PURPOSE:
 *   Implement the auto configuration catalogue behavior for
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
 * File: src/runtime/bootstrap/auto_configuration_catalogue.c
 *
 * PURPOSE:
 *   Maintain a deterministic catalogue of auto-configuration definitions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/auto_configuration_catalogue.h"


#include <string.h>
/*
 * Initialise bootstrap auto configuration catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_bootstrap_auto_configuration_catalogue_init(
    UmiBootstrapAutoConfigurationCatalogue *catalogue) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue != NULL) memset(catalogue, 0, sizeof(*catalogue));
}
/*
 * Find bootstrap auto configuration catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiBootstrapAutoConfiguration *umi_bootstrap_auto_configuration_catalogue_find(
    const UmiBootstrapAutoConfigurationCatalogue *catalogue,
    const char *configuration_id) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || configuration_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < catalogue->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(catalogue->items[i].configuration_id, configuration_id) == 0)
            return &catalogue->items[i];
    return NULL;
}
/*
 * Add bootstrap auto configuration catalogue only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_bootstrap_auto_configuration_catalogue_add(
    UmiBootstrapAutoConfigurationCatalogue *catalogue,
    const UmiBootstrapAutoConfiguration *configuration) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || configuration == NULL ||
        !umi_bootstrap_id_valid(configuration->configuration_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_bootstrap_auto_configuration_catalogue_find(
            catalogue, configuration->configuration_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *configuration;
    return UMI_STATUS_OK;
}
