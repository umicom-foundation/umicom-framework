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

/*
 * The runtime bundle lets packaging code distinguish the main executable from required DLLs, data files and resources.
 */

#include "umicom/delivery/runtime_bundle.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise runtime bundle from caller-provided values so later operations receive a
 * known state.
 */
void umi_runtime_bundle_init(UmiRuntimeBundle *bundle)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bundle != NULL) (void)memset(bundle, 0, sizeof(*bundle));
}
/* Add runtime bundle only after its inputs and available capacity have been checked. */
UmiStatus umi_runtime_bundle_add(UmiRuntimeBundle *bundle,
                                 const char *path)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bundle == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (bundle->count >= UMI_DELIVERY_MAX_ARTIFACTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_delivery_copy_text(bundle->files[bundle->count],
                               UMI_DELIVERY_PATH_CAPACITY,
                               path) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++bundle->count;
    return UMI_STATUS_OK;
}
/*
 * Provide the runtime bundle contains operation used by this module and its client
 * applications.
 */
int umi_runtime_bundle_contains(const UmiRuntimeBundle *bundle,
                                const char *path)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bundle == NULL || path == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < bundle->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(bundle->files[i], path) == 0) return 1;
    }
    return 0;
}
