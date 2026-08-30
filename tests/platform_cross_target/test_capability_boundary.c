/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_capability_boundary.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the capability boundary cross-target capability.
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

#include "umicom/platform/cross_target/capability_boundary.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtCapabilityBoundary b={"net",UMI_CT_CAP_NETWORK,UMI_CT_CAP_ADMIN};CHECK(umi_ct_capability_boundary_allows(&b,UMI_CT_CAP_NETWORK));CHECK(!umi_ct_capability_boundary_allows(&b,UMI_CT_CAP_NETWORK|UMI_CT_CAP_ADMIN));return 0;}
