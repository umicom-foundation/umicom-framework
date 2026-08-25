/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_syscall_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the syscall policy cross-target capability.
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

#include "umicom/platform/cross_target/syscall_policy.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtSyscallPolicy p={0U,127U,UMI_CT_SYSCALL_PRIVILEGED,true};UmiCtSyscallDescriptor d={3U,"file.read",0U,3U};CHECK(umi_ct_syscall_policy_allows(&p,&d));d.flags=UMI_CT_SYSCALL_PRIVILEGED;CHECK(!umi_ct_syscall_policy_allows(&p,&d));return 0;}
