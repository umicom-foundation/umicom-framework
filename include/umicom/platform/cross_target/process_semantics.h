/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/process_semantics.h
 *
 * PURPOSE:
 *   Describe process creation, signals and job/process-group capabilities across host and Umicom OS targets.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_PROCESS_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_PROCESS_SEMANTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtProcessSemantics { bool spawn; bool fork; bool signals; bool process_groups; bool job_objects; bool inherited_handles; } UmiCtProcessSemantics;
UmiCtProcessSemantics umi_ct_process_semantics_default(UmiCtOperatingSystem os);
UmiCtSupportLevel umi_ct_process_semantics_support(const UmiCtProcessSemantics *s,bool require_fork,bool require_groups);

#ifdef __cplusplus
}
#endif

#endif
