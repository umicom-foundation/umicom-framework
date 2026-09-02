/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/boot_service.c
 *
 * PURPOSE:
 *   Bind OS services to boot phases and validate dependency phase ordering.
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

#include "umicom/platform/cross_target/boot_service.h"

/* Check that ct boot service satisfies its contract before another service relies on it. */
UmiStatus umi_ct_boot_service_validate(const UmiCtBootService*s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_ct_id_valid(s->service_id)||(unsigned)s->phase>7U||s->timeout_ms==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/*
 * Check that ct boot dependency phase satisfies its contract before another service relies
 * on it.
 */
bool umi_ct_boot_dependency_phase_valid(const UmiCtBootService*s,const UmiCtBootService*d){return s!=NULL&&d!=NULL&&(unsigned)d->phase<=(unsigned)s->phase;}
