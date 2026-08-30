/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/readiness.h
 *
 * PURPOSE:
 *   Define readiness and liveness checks that can be aggregated without exposing
 *   product-specific health implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_READINESS_H
#define UMICOM_DIAGNOSTICS_READINESS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_READINESS_ID_CAPACITY 128U
#define UMI_READINESS_MESSAGE_CAPACITY 256U
#define UMI_READINESS_MAX 512U
typedef struct UmiReadinessCheck { char check_id[UMI_READINESS_ID_CAPACITY]; int ready; char message[UMI_READINESS_MESSAGE_CAPACITY]; uint64_t updated_at_ns; } UmiReadinessCheck;
typedef struct UmiReadinessRegistry UmiReadinessRegistry;
UmiStatus umi_readiness_registry_create(UmiReadinessRegistry **out_registry);
void umi_readiness_registry_destroy(UmiReadinessRegistry *registry);
UmiStatus umi_readiness_registry_update(UmiReadinessRegistry *registry,
                                        const char *check_id,
                                        int ready,
                                        const char *message,
                                        uint64_t now_ns);
int umi_readiness_registry_ready(const UmiReadinessRegistry *registry);
size_t umi_readiness_registry_count(const UmiReadinessRegistry *registry);
UmiStatus umi_readiness_registry_at(const UmiReadinessRegistry *registry,
                                    size_t index,
                                    UmiReadinessCheck *out_check);
#ifdef __cplusplus
}
#endif

#endif
