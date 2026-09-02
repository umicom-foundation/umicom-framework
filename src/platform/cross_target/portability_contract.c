/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/portability_contract.c
 *
 * PURPOSE:
 *   Declare required platform semantics for a Framework subsystem so cross-target gaps are explicit.
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

#include "umicom/platform/cross_target/portability_contract.h"

/*
 * Provide the ct portability missing operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_portability_missing(const UmiCtPortabilityContract*c,uint64_t a){return c==NULL?UINT64_MAX:(c->required&~a);}bool umi_ct_portability_satisfied(const UmiCtPortabilityContract*c,uint64_t a){return umi_ct_portability_missing(c,a)==0U;}
