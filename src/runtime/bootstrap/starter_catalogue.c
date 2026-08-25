/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/starter_catalogue.c
 *
 * PURPOSE:
 *   Maintain Framework-owned application starter definitions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/starter_catalogue.h"


#include <string.h>
void umi_bootstrap_starter_catalogue_init(UmiBootstrapStarterCatalogue *catalogue) {
    if (catalogue != NULL) memset(catalogue, 0, sizeof(*catalogue));
}
const UmiBootstrapStarterDescriptor *umi_bootstrap_starter_catalogue_find(
    const UmiBootstrapStarterCatalogue *catalogue,
    const char *starter_id) {
    size_t i;
    if (catalogue == NULL || starter_id == NULL) return NULL;
    for (i = 0U; i < catalogue->count; ++i)
        if (strcmp(catalogue->items[i].starter_id, starter_id) == 0)
            return &catalogue->items[i];
    return NULL;
}
UmiStatus umi_bootstrap_starter_catalogue_add(
    UmiBootstrapStarterCatalogue *catalogue,
    const UmiBootstrapStarterDescriptor *starter) {
    if (catalogue == NULL || starter == NULL ||
        !umi_bootstrap_id_valid(starter->starter_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_bootstrap_starter_catalogue_find(catalogue, starter->starter_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *starter;
    return UMI_STATUS_OK;
}
