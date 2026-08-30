/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/collection_binding.c
 *
 * PURPOSE:
 *   Implement the collection binding behavior for
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
 * File: src/runtime/bootstrap/collection_binding.c
 *
 * PURPOSE:
 *   Build ordered collections of service identifiers for multi-bind dependencies.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/collection_binding.h"


#include <string.h>
void umi_bootstrap_collection_binding_init(UmiBootstrapIdList *collection) {
    if (collection != NULL) memset(collection, 0, sizeof(*collection));
}
UmiStatus umi_bootstrap_collection_binding_add(UmiBootstrapIdList *collection,
                                               const char *service_id) {
    size_t i;
    if (collection == NULL || !umi_bootstrap_id_valid(service_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < collection->count; ++i)
        if (strcmp(collection->ids[i], service_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (collection->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)umi_bootstrap_copy_text(collection->ids[collection->count++],
        UMI_BOOTSTRAP_ID_CAPACITY, service_id);
    return UMI_STATUS_OK;
}
