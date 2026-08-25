/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/thread_semantics.c
 *
 * PURPOSE:
 *   Describe TLS, affinity and priority features used by Framework worker and service runtimes.
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

#include "umicom/platform/cross_target/thread_semantics.h"

UmiCtSupportLevel umi_ct_thread_semantics_support(const UmiCtThreadSemantics*s,bool a,bool p){if(s==NULL||!s->threads||!s->tls)return UMI_CT_SUPPORT_NONE;if((a&&!s->affinity)||(p&&!s->priority))return UMI_CT_SUPPORT_DEGRADED;return UMI_CT_SUPPORT_NATIVE;}
