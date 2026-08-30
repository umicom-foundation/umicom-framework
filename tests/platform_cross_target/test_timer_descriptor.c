/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_timer_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the timer descriptor cross-target capability.
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

#include "umicom/platform/cross_target/timer_descriptor.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtTimerDescriptor d={"mtime",UINT64_C(10000000),64U,true,true,false};CHECK(umi_ct_timer_descriptor_validate(&d)==UMI_STATUS_OK);CHECK(umi_ct_timer_ns_to_ticks(&d,UINT64_C(1000000000))==UINT64_C(10000000));return 0;}
