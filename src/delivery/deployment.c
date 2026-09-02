/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/deployment.c
 *
 * PURPOSE:
 *   Record one deployment attempt and its destination environment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Deployment is distinct from publishing: a published package can exist without being activated on a machine.
 */

#include "umicom/delivery/deployment.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise deployment from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_deployment_init(UmiDeployment *deployment,
                              const char *deployment_id,
                              const char *release_id,
                              const char *target,
                              uint64_t generation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (deployment == NULL || deployment_id == NULL ||
        release_id == NULL || target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(deployment, 0, sizeof(*deployment));
    deployment->generation = generation;
    deployment->status = UMI_EVIDENCE_UNKNOWN;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(deployment->deployment_id, sizeof(deployment->deployment_id), deployment_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(deployment->release_id, sizeof(deployment->release_id), release_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(deployment->target, sizeof(deployment->target), target) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
