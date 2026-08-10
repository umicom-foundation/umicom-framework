/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/health.h
 *
 * PURPOSE:
 *   Define reusable component-health state, immutable snapshots and a bounded
 *   health registry for modules, workers, adapters, applications and suites.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef enum UmiHealthState {
    UMI_HEALTH_UNKNOWN = 0,
    UMI_HEALTH_STARTING = 1,
    UMI_HEALTH_READY = 2,
    UMI_HEALTH_DEGRADED = 3,
    UMI_HEALTH_STOPPING = 4,
    UMI_HEALTH_STOPPED = 5,
    UMI_HEALTH_FAILED = 6
} UmiHealthState;

typedef struct UmiHealthSnapshot {
    char component_id[UMI_HEALTH_COMPONENT_CAPACITY];
    UmiHealthState state;
    char message[UMI_HEALTH_MESSAGE_CAPACITY];
    uint64_t updated_at_ns;
    uint64_t sequence;
} UmiHealthSnapshot;

typedef struct UmiHealthRegistry UmiHealthRegistry;

UmiStatus umi_health_registry_create(UmiHealthRegistry **out_registry);
void umi_health_registry_destroy(UmiHealthRegistry *registry);
UmiStatus umi_health_registry_update(UmiHealthRegistry *registry,
                                     const char *component_id,
                                     UmiHealthState state,
                                     const char *message,
                                     uint64_t updated_at_ns);
UmiStatus umi_health_registry_get(const UmiHealthRegistry *registry,
                                  const char *component_id,
                                  UmiHealthSnapshot *out_snapshot);
size_t umi_health_registry_count(const UmiHealthRegistry *registry);
UmiStatus umi_health_registry_at(const UmiHealthRegistry *registry,
                                 size_t index,
                                 UmiHealthSnapshot *out_snapshot);
UmiHealthState umi_health_registry_overall(const UmiHealthRegistry *registry);
const char *umi_health_state_text(UmiHealthState state);

#ifdef __cplusplus
}
#endif

#endif
