/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/cpu_feature_set.c
 *
 * PURPOSE:
 *   Maintain CPU feature bitsets and compute missing-feature masks for runtime admission.
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

#include "umicom/platform/cross_target/cpu_feature_set.h"

/* Add ct cpu feature set only after its inputs and available capacity have been checked. */
void umi_ct_cpu_feature_set_add(UmiCtCpuFeatureSet*s,UmiCtCpuFeature f){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)s->bits|=umi_ct_cpu_feature_bit(f);}
/*
 * Provide the ct cpu feature set has operation used by this module and its client
 * applications.
 */
bool umi_ct_cpu_feature_set_has(const UmiCtCpuFeatureSet*s,UmiCtCpuFeature f){uint64_t b=umi_ct_cpu_feature_bit(f);return s!=NULL&&b!=0U&&(s->bits&b)==b;}
/*
 * Provide the ct cpu feature set missing operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_cpu_feature_set_missing(const UmiCtCpuFeatureSet*a,const UmiCtCpuFeatureSet*r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||r==NULL)return UINT64_MAX;return r->bits&~a->bits;}
