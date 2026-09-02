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

/*
 * Provide the ct cross target health evaluate operation used by this module and its client
 * applications.
 */
UmiCtCrossTargetHealth umi_ct_cross_target_health_evaluate(const UmiCtCrossTargetHealthInput*i){UmiCtCrossTargetHealth h={UMI_CT_HEALTH_UNKNOWN,0U,0U,0U};unsigned checks=1U,ok=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==NULL)return h;h.warnings=i->warnings;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->runtime_conformant)ok++;/* Use this fallback path when the earlier condition does not apply. */ else h.blockers++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->toolchain_required){checks++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->toolchain_ready)ok++;/* Use this fallback path when the earlier condition does not apply. */ else h.blockers++;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->sysroot_required){checks++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->sysroot_ready)ok++;/* Use this fallback path when the earlier condition does not apply. */ else h.blockers++;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->emulator_required){checks++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->emulator_ready)ok++;/* Use this fallback path when the earlier condition does not apply. */ else h.blockers++;}h.readiness_percent=(uint8_t)((ok*100U)/checks);h.health=h.blockers?UMI_CT_HEALTH_BLOCKED:(h.warnings?UMI_CT_HEALTH_DEGRADED:UMI_CT_HEALTH_READY);return h;}
