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
#include "../base/value_construction_internal.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise deployment from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
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
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_deployment_init_build_value(UmiDeployment *deployment,
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

UMI_DEFINE_STAGED_VALUE_INIT(umi_deployment_init, UmiDeployment, deployment,
    (UmiDeployment *deployment, const char *deployment_id, const char *release_id, const char *target, uint64_t generation),
    umi_deployment_init_build_value(candidate, deployment_id, release_id, target, generation))
