/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/target_probe.h
 *
 * PURPOSE:
 *   Record host/target probe evidence without hard-coding OS-specific probing in application repositories.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TARGET_PROBE_H
#define UMICOM_PLATFORM_CROSS_TARGET_TARGET_PROBE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtTargetProbe { UmiCtTarget target; uint32_t cpu_count; uint64_t memory_bytes; uint32_t page_size; uint64_t cpu_features; uint8_t confidence; } UmiCtTargetProbe;
UmiStatus umi_ct_target_probe_validate(const UmiCtTargetProbe *probe);
uint8_t umi_ct_target_probe_score(const UmiCtTargetProbe *probe,const UmiCtTarget *expected);

#ifdef __cplusplus
}
#endif

#endif
