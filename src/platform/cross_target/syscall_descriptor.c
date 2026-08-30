/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/syscall_descriptor.c
 *
 * PURPOSE:
 *   Describe stable Umicom OS syscall numbers, domains and side-effect policy.
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

#include "umicom/platform/cross_target/syscall_descriptor.h"

UmiStatus umi_ct_syscall_descriptor_validate(const UmiCtSyscallDescriptor*d){if(d==NULL||!umi_ct_id_valid(d->name)||d->argument_count>8U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
