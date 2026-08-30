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

UmiStatus umi_ct_scheduler_profile_validate(const UmiCtSchedulerProfile*p){if(p==NULL||p->scheduler_class<UMI_CT_SCHED_COOPERATIVE||p->scheduler_class>UMI_CT_SCHED_REALTIME||p->cpu_count==0U||p->priority_levels==0U)return UMI_STATUS_INVALID_ARGUMENT;if(p->scheduler_class!=UMI_CT_SCHED_COOPERATIVE&&p->timeslice_us==0U)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
