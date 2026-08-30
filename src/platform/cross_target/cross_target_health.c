/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/cross_target_health.c
 *
 * PURPOSE:
 *   Aggregate cross-toolchain, sysroot, emulator and runtime conformance evidence into one readiness state.
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

UmiCtCrossTargetHealth umi_ct_cross_target_health_evaluate(const UmiCtCrossTargetHealthInput*i){UmiCtCrossTargetHealth h={UMI_CT_HEALTH_UNKNOWN,0U,0U,0U};unsigned checks=1U,ok=0U;if(i==NULL)return h;h.warnings=i->warnings;if(i->runtime_conformant)ok++;else h.blockers++;if(i->toolchain_required){checks++;if(i->toolchain_ready)ok++;else h.blockers++;}if(i->sysroot_required){checks++;if(i->sysroot_ready)ok++;else h.blockers++;}if(i->emulator_required){checks++;if(i->emulator_ready)ok++;else h.blockers++;}h.readiness_percent=(uint8_t)((ok*100U)/checks);h.health=h.blockers?UMI_CT_HEALTH_BLOCKED:(h.warnings?UMI_CT_HEALTH_DEGRADED:UMI_CT_HEALTH_READY);return h;}
