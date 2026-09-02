/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/health.h
 *
 * PURPOSE:
 *   Define reusable component-health state, immutable snapshots and a bounded
 *   health registry for modules, workers, adapters, applications and suites.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_HEALTH_H
#define UMICOM_RUNTIME_HEALTH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_HEALTH_COMPONENT_CAPACITY 128U
#define UMI_HEALTH_MESSAGE_CAPACITY 256U
#define UMI_HEALTH_REGISTRY_MAX 512U

/**
 * List the named health state values accepted by this public contract.
 */
typedef enum UmiHealthState {
    UMI_HEALTH_UNKNOWN = 0,
    UMI_HEALTH_STARTING = 1,
    UMI_HEALTH_READY = 2,
    UMI_HEALTH_DEGRADED = 3,
    UMI_HEALTH_STOPPING = 4,
    UMI_HEALTH_STOPPED = 5,
    UMI_HEALTH_FAILED = 6
} UmiHealthState;

/**
 * Represent the health snapshot data shared with callers of this public contract.
 */
typedef struct UmiHealthSnapshot {
    char component_id[UMI_HEALTH_COMPONENT_CAPACITY];
    UmiHealthState state;
    char message[UMI_HEALTH_MESSAGE_CAPACITY];
    uint64_t updated_at_ns;
    uint64_t sequence;
} UmiHealthSnapshot;

/**
 * Represent the health registry data shared with callers of this public contract.
 */
typedef struct UmiHealthRegistry UmiHealthRegistry;

/**
 * Initialise health registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_health_registry_create(UmiHealthRegistry **out_registry);
/**
 * Release or reset state held by health registry so the same storage can be reused safely.
 */
void umi_health_registry_destroy(UmiHealthRegistry *registry);
/**
 * Provide the health registry update operation used by this module and its client
 * applications.
 */
UmiStatus umi_health_registry_update(UmiHealthRegistry *registry,
                                     const char *component_id,
                                     UmiHealthState state,
                                     const char *message,
                                     uint64_t updated_at_ns);
/**
 * Provide the health registry get operation used by this module and its client
 * applications.
 */
UmiStatus umi_health_registry_get(const UmiHealthRegistry *registry,
                                  const char *component_id,
                                  UmiHealthSnapshot *out_snapshot);
/**
 * Return the number of records represented by health registry without changing their
 * state.
 */
size_t umi_health_registry_count(const UmiHealthRegistry *registry);
/**
 * Find health registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_health_registry_at(const UmiHealthRegistry *registry,
                                 size_t index,
                                 UmiHealthSnapshot *out_snapshot);
/**
 * Provide the health registry overall operation used by this module and its client
 * applications.
 */
UmiHealthState umi_health_registry_overall(const UmiHealthRegistry *registry);
/**
 * Provide the health state text operation used by this module and its client applications.
 */
const char *umi_health_state_text(UmiHealthState state);

#ifdef __cplusplus
}
#endif

#endif
