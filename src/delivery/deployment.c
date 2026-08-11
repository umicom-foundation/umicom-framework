/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/deployment.c
 *
 * PURPOSE:
 *   Record one deployment attempt and its destination environment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Deployment is distinct from publishing: a published package can exist without being activated on a machine.
 */

#include "umicom/delivery/deployment.h"
#include "delivery_internal.h"
#include <string.h>
UmiStatus umi_deployment_init(UmiDeployment *deployment,
                              const char *deployment_id,
                              const char *release_id,
                              const char *target,
                              uint64_t generation)
{
    if (deployment == NULL || deployment_id == NULL ||
        release_id == NULL || target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(deployment, 0, sizeof(*deployment));
    deployment->generation = generation;
    deployment->status = UMI_EVIDENCE_UNKNOWN;
    if (umi_delivery_copy_text(deployment->deployment_id, sizeof(deployment->deployment_id), deployment_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(deployment->release_id, sizeof(deployment->release_id), release_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(deployment->target, sizeof(deployment->target), target) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
