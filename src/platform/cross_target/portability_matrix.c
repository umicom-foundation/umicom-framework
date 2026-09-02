/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/portability_matrix.c
 *
 * PURPOSE:
 *   Record support levels for platform semantics across target operating systems.
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

#include "umicom/platform/cross_target/portability_matrix.h"

#include <string.h>
/*
 * Initialise ct portability matrix from caller-provided values so later operations receive
 * a known state.
 */
void umi_ct_portability_matrix_init(UmiCtPortabilityMatrix*m){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL)memset(m,0,sizeof(*m));}
/*
 * Copy ct portability matrix into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ct_portability_matrix_set(UmiCtPortabilityMatrix*m,UmiCtOperatingSystem os,UmiCtPortabilityCapability c,UmiCtSupportLevel s){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||os==UMI_CT_OS_UNKNOWN||c==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<m->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->items[i].os==os&&m->items[i].capability==c){m->items[i].support=s;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;m->items[m->count++]=(UmiCtPortabilityEntry){os,c,s};return UMI_STATUS_OK;}
/*
 * Provide the ct portability matrix get operation used by this module and its client
 * applications.
 */
UmiCtSupportLevel umi_ct_portability_matrix_get(const UmiCtPortabilityMatrix*m,UmiCtOperatingSystem os,UmiCtPortabilityCapability c){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return UMI_CT_SUPPORT_NONE;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<m->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->items[i].os==os&&m->items[i].capability==c)return m->items[i].support;return UMI_CT_SUPPORT_NONE;}
