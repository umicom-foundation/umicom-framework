/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_clock_semantics.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the clock semantics cross-target capability.
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

#include "umicom/platform/cross_target/clock_semantics.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtClockSemantics s={true,true,true,UINT64_C(1000000),1000U};CHECK(umi_ct_clock_semantics_validate(&s)==UMI_STATUS_OK);CHECK(umi_ct_clock_ticks_to_ns(&s,1000U)==1000000U);return 0;}
