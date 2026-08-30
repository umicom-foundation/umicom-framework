/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_target_fingerprint.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the target fingerprint cross-target capability.
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

#include "umicom/platform/cross_target/target_fingerprint.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtTarget t={0};uint64_t a,b;CHECK(umi_ct_copy(t.triple,sizeof(t.triple),"riscv64-umicom-umicom-native")==UMI_STATUS_OK);CHECK(umi_ct_copy(t.vendor,sizeof(t.vendor),"umicom")==UMI_STATUS_OK);t.architecture=UMI_CT_ARCH_RISCV64;t.operating_system=UMI_CT_OS_UMICOM;t.environment=UMI_CT_ENV_UMICOM;a=umi_ct_target_fingerprint(&t,7U,"lp64d",4096U);b=umi_ct_target_fingerprint(&t,7U,"lp64d",4096U);CHECK(a!=0U&&a==b);CHECK(a!=umi_ct_target_fingerprint(&t,3U,"lp64d",4096U));return 0;}
