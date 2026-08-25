/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_platform_conformance.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the platform conformance cross-target capability.
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

#include "umicom/platform/cross_target/platform_conformance.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtPlatformConformanceInput i={3U,3U,3U,1U,4U,4U};UmiCtPlatformConformance r=umi_ct_platform_conformance_evaluate(&i);CHECK(r.cpu_gaps==1U);CHECK(r.score<100U);i.cpu_available=3U;r=umi_ct_platform_conformance_evaluate(&i);CHECK(r.score==100U&&r.health==UMI_CT_HEALTH_READY);return 0;}
