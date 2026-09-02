/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_cross_target_health.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the cross target health cross-target capability.
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

#include "umicom/platform/cross_target/cross_target_health.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiCtCrossTargetHealthInput i={true,true,true,true,true,false,true,0U};UmiCtCrossTargetHealth h=umi_ct_cross_target_health_evaluate(&i);CHECK(h.health==UMI_CT_HEALTH_BLOCKED&&h.blockers==1U);i.emulator_ready=true;h=umi_ct_cross_target_health_evaluate(&i);CHECK(h.health==UMI_CT_HEALTH_READY&&h.readiness_percent==100U);return 0;}
