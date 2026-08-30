/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/target_probe.c
 *
 * PURPOSE:
 *   Record host/target probe evidence without hard-coding OS-specific probing in application repositories.
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

#include "umicom/platform/cross_target/target_probe.h"

UmiStatus umi_ct_target_probe_validate(const UmiCtTargetProbe*p){if(p==NULL||p->target.architecture==UMI_CT_ARCH_UNKNOWN||p->cpu_count==0U||p->page_size==0U||p->confidence>100U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
uint8_t umi_ct_target_probe_score(const UmiCtTargetProbe*p,const UmiCtTarget*e){unsigned s=0U;if(p==NULL||e==NULL)return 0U;if(p->target.architecture==e->architecture)s+=35U;if(p->target.operating_system==e->operating_system)s+=35U;if(p->target.environment==e->environment)s+=20U;if(p->target.pointer_bits==e->pointer_bits)s+=10U;return (uint8_t)((s*(unsigned)p->confidence)/100U);}
