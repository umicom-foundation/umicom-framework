/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/cpu_feature.c
 *
 * PURPOSE:
 *   Define stable generic and RISC-V CPU feature bits used by runtime capability negotiation.
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

#include "umicom/platform/cross_target/cpu_feature.h"

/*
 * Provide the ct cpu feature bit operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_cpu_feature_bit(UmiCtCpuFeature f){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if((unsigned)f>=(unsigned)UMI_CT_CPU_COUNT)return 0U;return UINT64_C(1)<<(unsigned)f;}
/*
 * Provide the ct cpu feature text operation used by this module and its client
 * applications.
 */
const char*umi_ct_cpu_feature_text(UmiCtCpuFeature f){static const char*n[]={"atomics","float","double","compressed","vector","bitmanip","crypto","hypervisor","user-interrupts","mmu"};return (unsigned)f<(unsigned)UMI_CT_CPU_COUNT?n[(unsigned)f]:"unknown";}
