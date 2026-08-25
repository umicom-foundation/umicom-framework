/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/cross_target_snapshot.c
 *
 * PURPOSE:
 *   Aggregate immutable cross-target platform evidence for diagnostics, release gates and remote execution.
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

#include "umicom/platform/cross_target/cross_target_snapshot.h"

UmiStatus umi_ct_cross_target_snapshot_validate(const UmiCtCrossTargetSnapshot*s){if(s==NULL||s->target.architecture==UMI_CT_ARCH_UNKNOWN||s->abi[0]=='\0'||s->cpu_count==0U||s->page_size==0U||s->fingerprint==0U)return UMI_STATUS_INVALID_ARGUMENT;if(s->health.health==UMI_CT_HEALTH_BLOCKED)return UMI_STATUS_UNAVAILABLE;return UMI_STATUS_OK;}
