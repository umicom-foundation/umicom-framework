/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_cpu_feature_set.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the cpu feature set cross-target capability.
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

#include "umicom/platform/cross_target/cpu_feature_set.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtCpuFeatureSet a={0},r={0};umi_ct_cpu_feature_set_add(&a,UMI_CT_CPU_ATOMICS);umi_ct_cpu_feature_set_add(&r,UMI_CT_CPU_ATOMICS);umi_ct_cpu_feature_set_add(&r,UMI_CT_CPU_VECTOR);CHECK(umi_ct_cpu_feature_set_has(&a,UMI_CT_CPU_ATOMICS));CHECK(umi_ct_cpu_feature_set_missing(&a,&r)==umi_ct_cpu_feature_bit(UMI_CT_CPU_VECTOR));return 0;}
