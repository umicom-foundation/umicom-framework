/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/syscall_catalogue.h
 *
 * PURPOSE:
 *   Maintain unique syscall number/name registrations for ABI stability checks.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_SYSCALL_CATALOGUE_H
#define UMICOM_PLATFORM_CROSS_TARGET_SYSCALL_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/syscall_descriptor.h"
/**
 * Represent the ct syscall catalogue data shared with callers of this public contract.
 */
typedef struct UmiCtSyscallCatalogue { UmiCtSyscallDescriptor items[UMI_CT_MAX_ITEMS]; size_t count; } UmiCtSyscallCatalogue;
/**
 * Initialise ct syscall catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_ct_syscall_catalogue_init(UmiCtSyscallCatalogue *catalogue);
/**
 * Add ct syscall catalogue only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ct_syscall_catalogue_add(UmiCtSyscallCatalogue *catalogue,const UmiCtSyscallDescriptor *descriptor);
/**
 * Provide the ct syscall catalogue find number operation used by this module and its
 * client applications.
 */
const UmiCtSyscallDescriptor *umi_ct_syscall_catalogue_find_number(const UmiCtSyscallCatalogue *catalogue,uint32_t number);

#ifdef __cplusplus
}
#endif

#endif
