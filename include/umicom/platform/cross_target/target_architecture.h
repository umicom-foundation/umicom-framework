/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/target_architecture.h
 *
 * PURPOSE:
 *   Parse and describe CPU architecture identities without duplicating compiler discovery.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TARGET_ARCHITECTURE_H
#define UMICOM_PLATFORM_CROSS_TARGET_TARGET_ARCHITECTURE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ct_architecture_parse(const char *text, UmiCtArchitecture *out_architecture);
const char *umi_ct_architecture_text(UmiCtArchitecture architecture);
uint32_t umi_ct_architecture_pointer_bits(UmiCtArchitecture architecture);
bool umi_ct_architecture_is_riscv(UmiCtArchitecture architecture);

#ifdef __cplusplus
}
#endif

#endif
