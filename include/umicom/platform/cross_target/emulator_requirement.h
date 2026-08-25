/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/emulator_requirement.h
 *
 * PURPOSE:
 *   Describe optional emulator requirements for cross-target tests and boot validation.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_EMULATOR_REQUIREMENT_H
#define UMICOM_PLATFORM_CROSS_TARGET_EMULATOR_REQUIREMENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtEmulatorRequirement { char emulator_id[UMI_CT_ID_CAPACITY]; UmiCtArchitecture architecture; uint32_t minimum_major; bool system_mode; bool user_mode; bool required_for_ci; } UmiCtEmulatorRequirement;
typedef struct UmiCtEmulatorEvidence { char emulator_id[UMI_CT_ID_CAPACITY]; UmiCtArchitecture architecture; uint32_t major; bool system_mode; bool user_mode; } UmiCtEmulatorEvidence;
bool umi_ct_emulator_requirement_satisfied(const UmiCtEmulatorRequirement *requirement,const UmiCtEmulatorEvidence *evidence);

#ifdef __cplusplus
}
#endif

#endif
