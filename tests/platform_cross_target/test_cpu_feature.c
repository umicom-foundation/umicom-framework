/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_cpu_feature.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the cpu feature cross-target capability.
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

#include "umicom/platform/cross_target/cpu_feature.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){CHECK(umi_ct_cpu_feature_bit(UMI_CT_CPU_VECTOR)==UINT64_C(16));CHECK(umi_ct_cpu_feature_bit((UmiCtCpuFeature)99)==0U);CHECK(umi_ct_cpu_feature_text(UMI_CT_CPU_ATOMICS)[0]=='a');return 0;}
