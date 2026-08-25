/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/boot_phase.h
 *
 * PURPOSE:
 *   Define deterministic Umicom OS boot phases and legal phase progression.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_BOOT_PHASE_H
#define UMICOM_PLATFORM_CROSS_TARGET_BOOT_PHASE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiCtBootPhase { UMI_CT_BOOT_RESET=0, UMI_CT_BOOT_EARLY=1, UMI_CT_BOOT_MEMORY=2, UMI_CT_BOOT_INTERRUPTS=3, UMI_CT_BOOT_DEVICES=4, UMI_CT_BOOT_SERVICES=5, UMI_CT_BOOT_USERSPACE=6, UMI_CT_BOOT_READY=7 } UmiCtBootPhase;
const char *umi_ct_boot_phase_text(UmiCtBootPhase phase);
bool umi_ct_boot_phase_transition_valid(UmiCtBootPhase from,UmiCtBootPhase to);

#ifdef __cplusplus
}
#endif

#endif
