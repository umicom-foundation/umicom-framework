/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/scheduler_profile.h
 *
 * PURPOSE:
 *   Describe portable scheduler policy inputs for Umicom OS and runtime conformance.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_SCHEDULER_PROFILE_H
#define UMICOM_PLATFORM_CROSS_TARGET_SCHEDULER_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiCtSchedulerClass { UMI_CT_SCHED_COOPERATIVE=1, UMI_CT_SCHED_PREEMPTIVE=2, UMI_CT_SCHED_REALTIME=3 } UmiCtSchedulerClass;
typedef struct UmiCtSchedulerProfile { UmiCtSchedulerClass scheduler_class; uint32_t cpu_count; uint32_t timeslice_us; uint8_t priority_levels; bool affinity; bool load_balancing; } UmiCtSchedulerProfile;
UmiStatus umi_ct_scheduler_profile_validate(const UmiCtSchedulerProfile *profile);

#ifdef __cplusplus
}
#endif

#endif
