/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_portability_contract.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the portability contract cross-target capability.
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

#include "umicom/platform/cross_target/portability_contract.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtPortabilityContract c={"core",UMI_CT_PORT_PATHS|UMI_CT_PORT_CLOCK,0U};CHECK(umi_ct_portability_satisfied(&c,UMI_CT_PORT_PATHS|UMI_CT_PORT_CLOCK|UMI_CT_PORT_MEMORY));CHECK(umi_ct_portability_missing(&c,UMI_CT_PORT_PATHS)==UMI_CT_PORT_CLOCK);return 0;}
