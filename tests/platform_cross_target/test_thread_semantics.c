/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_thread_semantics.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the thread semantics cross-target capability.
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

#include "umicom/platform/cross_target/thread_semantics.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtThreadSemantics s={true,true,false,true,true,64U};CHECK(umi_ct_thread_semantics_support(&s,false,true)==UMI_CT_SUPPORT_NATIVE);CHECK(umi_ct_thread_semantics_support(&s,true,true)==UMI_CT_SUPPORT_DEGRADED);return 0;}
