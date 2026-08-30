/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/calling_convention.h
 *
 * PURPOSE:
 *   Map target architecture/OS combinations onto canonical calling-convention identities.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CALLING_CONVENTION_H
#define UMICOM_PLATFORM_CROSS_TARGET_CALLING_CONVENTION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/abi_descriptor.h"
UmiCtCallingConvention umi_ct_calling_convention_default(UmiCtArchitecture architecture,UmiCtOperatingSystem os);
const char *umi_ct_calling_convention_text(UmiCtCallingConvention convention);

#ifdef __cplusplus
}
#endif

#endif
