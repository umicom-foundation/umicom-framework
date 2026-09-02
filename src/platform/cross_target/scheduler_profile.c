/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/scheduler_profile.c
 *
 * PURPOSE:
 *   Describe portable scheduler policy inputs for Umicom OS and runtime conformance.
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

#include "umicom/platform/cross_target/scheduler_profile.h"

/*
 * Check that ct scheduler profile satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_scheduler_profile_validate(const UmiCtSchedulerProfile*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p->scheduler_class<UMI_CT_SCHED_COOPERATIVE||p->scheduler_class>UMI_CT_SCHED_REALTIME||p->cpu_count==0U||p->priority_levels==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->scheduler_class!=UMI_CT_SCHED_COOPERATIVE&&p->timeslice_us==0U)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
