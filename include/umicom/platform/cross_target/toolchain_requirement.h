/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/toolchain_requirement.h
 *
 * PURPOSE:
 *   Describe the validated external toolchain capabilities required by a target without reimplementing tool discovery.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TOOLCHAIN_REQUIREMENT_H
#define UMICOM_PLATFORM_CROSS_TARGET_TOOLCHAIN_REQUIREMENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ct toolchain requirement data shared with callers of this public contract.
 */
typedef struct UmiCtToolchainRequirement { char family[32]; uint32_t minimum_major; uint32_t minimum_minor; bool compiler; bool assembler; bool linker; bool debugger; } UmiCtToolchainRequirement;
/**
 * Represent the ct toolchain evidence data shared with callers of this public contract.
 */
typedef struct UmiCtToolchainEvidence { char family[32]; uint32_t major; uint32_t minor; bool compiler; bool assembler; bool linker; bool debugger; } UmiCtToolchainEvidence;
/**
 * Provide the ct toolchain requirement satisfied operation used by this module and its
 * client applications.
 */
bool umi_ct_toolchain_requirement_satisfied(const UmiCtToolchainRequirement *requirement,const UmiCtToolchainEvidence *evidence);

#ifdef __cplusplus
}
#endif

#endif
