/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/deployment.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_DEPLOYMENT_H
#define INCLUDE_UMICOM_DELIVERY_DEPLOYMENT_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeployment {
    char deployment_id[UMI_DELIVERY_ID_CAPACITY];
    char release_id[UMI_DELIVERY_ID_CAPACITY];
    char target[UMI_DELIVERY_ID_CAPACITY];
    uint64_t generation;
    UmiEvidenceStatus status;
} UmiDeployment;
UmiStatus umi_deployment_init(UmiDeployment *deployment,
                              const char *deployment_id,
                              const char *release_id,
                              const char *target,
                              uint64_t generation);

#ifdef __cplusplus
}
#endif

#endif
