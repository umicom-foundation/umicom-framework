/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/syscall_policy.h
 *
 * PURPOSE:
 *   Apply allow/deny policy to syscall descriptors using explicit flag and number ranges.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_SYSCALL_POLICY_H
#define UMICOM_PLATFORM_CROSS_TARGET_SYSCALL_POLICY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/syscall_descriptor.h"
typedef struct UmiCtSyscallPolicy { uint32_t minimum_number; uint32_t maximum_number; UmiCtSyscallFlags denied_flags; bool default_allow; } UmiCtSyscallPolicy;
bool umi_ct_syscall_policy_allows(const UmiCtSyscallPolicy *policy,const UmiCtSyscallDescriptor *descriptor);

#ifdef __cplusplus
}
#endif

#endif
