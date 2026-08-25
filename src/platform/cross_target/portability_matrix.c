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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/portability_matrix.h"

#include <string.h>
void umi_ct_portability_matrix_init(UmiCtPortabilityMatrix*m){if(m!=NULL)memset(m,0,sizeof(*m));}
UmiStatus umi_ct_portability_matrix_set(UmiCtPortabilityMatrix*m,UmiCtOperatingSystem os,UmiCtPortabilityCapability c,UmiCtSupportLevel s){size_t i;if(m==NULL||os==UMI_CT_OS_UNKNOWN||c==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<m->count;++i)if(m->items[i].os==os&&m->items[i].capability==c){m->items[i].support=s;return UMI_STATUS_OK;}if(m->count>=UMI_CT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;m->items[m->count++]=(UmiCtPortabilityEntry){os,c,s};return UMI_STATUS_OK;}
UmiCtSupportLevel umi_ct_portability_matrix_get(const UmiCtPortabilityMatrix*m,UmiCtOperatingSystem os,UmiCtPortabilityCapability c){size_t i;if(m==NULL)return UMI_CT_SUPPORT_NONE;for(i=0U;i<m->count;++i)if(m->items[i].os==os&&m->items[i].capability==c)return m->items[i].support;return UMI_CT_SUPPORT_NONE;}
