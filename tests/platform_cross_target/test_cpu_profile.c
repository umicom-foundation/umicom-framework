/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_cpu_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the cpu profile cross-target capability.
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

#include "umicom/platform/cross_target/cpu_profile.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtCpuProfile p={"rv64-base",UMI_CT_ARCH_RISCV64,64U,{1U},2U};UmiCtCpuFeatureSet a={1U};CHECK(umi_ct_cpu_profile_validate(&p)==UMI_STATUS_OK);CHECK(umi_ct_cpu_profile_matches(&p,UMI_CT_ARCH_RISCV64,64U,4U,&a));CHECK(!umi_ct_cpu_profile_matches(&p,UMI_CT_ARCH_RISCV64,64U,1U,&a));return 0;}
