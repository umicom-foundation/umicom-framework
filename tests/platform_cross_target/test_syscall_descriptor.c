/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_syscall_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the syscall descriptor cross-target capability.
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

#include "umicom/platform/cross_target/syscall_descriptor.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtSyscallDescriptor d={1U,"thread.yield",UMI_CT_SYSCALL_RESTARTABLE,0U};CHECK(umi_ct_syscall_descriptor_validate(&d)==UMI_STATUS_OK);d.argument_count=9U;CHECK(umi_ct_syscall_descriptor_validate(&d)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
