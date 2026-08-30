/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/runtime_bundle.c
 *
 * PURPOSE:
 *   Describe runtime files that must travel with an application package.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The runtime bundle lets packaging code distinguish the main executable from required DLLs, data files and resources.
 */

#include "umicom/delivery/runtime_bundle.h"
#include "delivery_internal.h"
#include <string.h>

void umi_runtime_bundle_init(UmiRuntimeBundle *bundle)
{
    if (bundle != NULL) (void)memset(bundle, 0, sizeof(*bundle));
}
UmiStatus umi_runtime_bundle_add(UmiRuntimeBundle *bundle,
                                 const char *path)
{
    if (bundle == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (bundle->count >= UMI_DELIVERY_MAX_ARTIFACTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_delivery_copy_text(bundle->files[bundle->count],
                               UMI_DELIVERY_PATH_CAPACITY,
                               path) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++bundle->count;
    return UMI_STATUS_OK;
}
int umi_runtime_bundle_contains(const UmiRuntimeBundle *bundle,
                                const char *path)
{
    size_t i;
    if (bundle == NULL || path == NULL) return 0;
    for (i = 0U; i < bundle->count; ++i) {
        if (strcmp(bundle->files[i], path) == 0) return 1;
    }
    return 0;
}
