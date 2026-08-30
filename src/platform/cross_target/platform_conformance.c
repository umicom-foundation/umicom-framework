/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/platform_conformance.c
 *
 * PURPOSE:
 *   Score target support across required portability, CPU and OS service capabilities.
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

#include "umicom/platform/cross_target/platform_conformance.h"

static uint32_t pop64(uint64_t x){uint32_t n=0U;while(x){n+=(uint32_t)(x&1U);x>>=1U;}return n;}
UmiCtPlatformConformance umi_ct_platform_conformance_evaluate(const UmiCtPlatformConformanceInput*i){UmiCtPlatformConformance r={0U,0U,0U,0U,0U,UMI_CT_HEALTH_UNKNOWN};uint32_t total,ok;if(i==NULL)return r;r.portability_gaps=pop64(i->portability_required&~i->portability_available);r.cpu_gaps=pop64(i->cpu_required&~i->cpu_available);r.service_gaps=i->ready_services>=i->required_services?0U:i->required_services-i->ready_services;r.blockers=r.portability_gaps+r.cpu_gaps+r.service_gaps;total=pop64(i->portability_required)+pop64(i->cpu_required)+i->required_services;if(total==0U){r.score=100U;r.health=UMI_CT_HEALTH_READY;return r;}ok=total-r.blockers;r.score=(uint8_t)((ok*100U)/total);r.health=r.blockers==0U?UMI_CT_HEALTH_READY:(r.score>=75U?UMI_CT_HEALTH_DEGRADED:UMI_CT_HEALTH_BLOCKED);return r;}
