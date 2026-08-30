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
void umi_bootstrap_auto_configuration_catalogue_init(
    UmiBootstrapAutoConfigurationCatalogue *catalogue) {
    if (catalogue != NULL) memset(catalogue, 0, sizeof(*catalogue));
}
const UmiBootstrapAutoConfiguration *umi_bootstrap_auto_configuration_catalogue_find(
    const UmiBootstrapAutoConfigurationCatalogue *catalogue,
    const char *configuration_id) {
    size_t i;
    if (catalogue == NULL || configuration_id == NULL) return NULL;
    for (i = 0U; i < catalogue->count; ++i)
        if (strcmp(catalogue->items[i].configuration_id, configuration_id) == 0)
            return &catalogue->items[i];
    return NULL;
}
UmiStatus umi_bootstrap_auto_configuration_catalogue_add(
    UmiBootstrapAutoConfigurationCatalogue *catalogue,
    const UmiBootstrapAutoConfiguration *configuration) {
    if (catalogue == NULL || configuration == NULL ||
        !umi_bootstrap_id_valid(configuration->configuration_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_bootstrap_auto_configuration_catalogue_find(
            catalogue, configuration->configuration_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *configuration;
    return UMI_STATUS_OK;
}
