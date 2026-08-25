/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_cross_target/test_scheduler_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the scheduler profile cross-target capability.
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

#include "umicom/platform/cross_target/scheduler_profile.h"

#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void){UmiCtSchedulerProfile p={UMI_CT_SCHED_PREEMPTIVE,4U,2000U,32U,true,true};CHECK(umi_ct_scheduler_profile_validate(&p)==UMI_STATUS_OK);p.timeslice_us=0U;CHECK(umi_ct_scheduler_profile_validate(&p)==UMI_STATUS_INVALID_STATE);return 0;}
