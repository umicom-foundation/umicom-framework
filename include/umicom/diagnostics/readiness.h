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
/**
 * Represent the readiness check data shared with callers of this public contract.
 */
typedef struct UmiReadinessCheck { char check_id[UMI_READINESS_ID_CAPACITY]; int ready; char message[UMI_READINESS_MESSAGE_CAPACITY]; uint64_t updated_at_ns; } UmiReadinessCheck;
/**
 * Represent the readiness registry data shared with callers of this public contract.
 */
typedef struct UmiReadinessRegistry UmiReadinessRegistry;
/**
 * Initialise readiness registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_readiness_registry_create(UmiReadinessRegistry **out_registry);
/**
 * Release or reset state held by readiness registry so the same storage can be reused
 * safely.
 */
void umi_readiness_registry_destroy(UmiReadinessRegistry *registry);
/**
 * Provide the readiness registry update operation used by this module and its client
 * applications.
 */
UmiStatus umi_readiness_registry_update(UmiReadinessRegistry *registry,
                                        const char *check_id,
                                        int ready,
                                        const char *message,
                                        uint64_t now_ns);
/**
 * Provide the readiness registry ready operation used by this module and its client
 * applications.
 */
int umi_readiness_registry_ready(const UmiReadinessRegistry *registry);
/**
 * Return the number of records represented by readiness registry without changing their
 * state.
 */
size_t umi_readiness_registry_count(const UmiReadinessRegistry *registry);
/**
 * Find readiness registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_readiness_registry_at(const UmiReadinessRegistry *registry,
                                    size_t index,
                                    UmiReadinessCheck *out_check);
#ifdef __cplusplus
}
#endif

#endif
