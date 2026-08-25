/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/syscall_descriptor.h
 *
 * PURPOSE:
 *   Describe stable Umicom OS syscall numbers, domains and side-effect policy.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_SYSCALL_DESCRIPTOR_H
#define UMICOM_PLATFORM_CROSS_TARGET_SYSCALL_DESCRIPTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t UmiCtSyscallFlags;
enum { UMI_CT_SYSCALL_BLOCKING=1U<<0, UMI_CT_SYSCALL_PRIVILEGED=1U<<1, UMI_CT_SYSCALL_RESTARTABLE=1U<<2, UMI_CT_SYSCALL_SIDE_EFFECT=1U<<3 };
typedef struct UmiCtSyscallDescriptor { uint32_t number; char name[UMI_CT_NAME_CAPACITY]; UmiCtSyscallFlags flags; uint8_t argument_count; } UmiCtSyscallDescriptor;
UmiStatus umi_ct_syscall_descriptor_validate(const UmiCtSyscallDescriptor *descriptor);

#ifdef __cplusplus
}
#endif

#endif
