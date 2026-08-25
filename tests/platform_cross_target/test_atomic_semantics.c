/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_atomic_semantics.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the atomic semantics cross-target capability.
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

#include "umicom/platform/cross_target/atomic_semantics.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtAtomicSemantics s={true,true,true,true,false,true,true};CHECK(umi_ct_atomic_width_lock_free(&s,64U));CHECK(!umi_ct_atomic_width_lock_free(&s,128U));CHECK(umi_ct_atomic_semantics_support(&s,64U,true)==UMI_CT_SUPPORT_NATIVE);return 0;}
