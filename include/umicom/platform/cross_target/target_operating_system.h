/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/target_operating_system.h
 *
 * PURPOSE:
 *   Parse supported host and target operating-system identities including Umicom OS and bare-metal targets.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TARGET_OPERATING_SYSTEM_H
#define UMICOM_PLATFORM_CROSS_TARGET_TARGET_OPERATING_SYSTEM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ct_operating_system_parse(const char *text, UmiCtOperatingSystem *out_os);
const char *umi_ct_operating_system_text(UmiCtOperatingSystem os);
bool umi_ct_operating_system_is_unix_like(UmiCtOperatingSystem os);

#ifdef __cplusplus
}
#endif

#endif
