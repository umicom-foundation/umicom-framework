/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/atomic_semantics.c
 *
 * PURPOSE:
 *   Describe lock-free atomic widths and memory ordering guarantees for cross-target lock-free algorithms.
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

#include "umicom/platform/cross_target/atomic_semantics.h"

bool umi_ct_atomic_width_lock_free(const UmiCtAtomicSemantics*s,uint32_t b){if(s==NULL)return false;switch(b){case 8:return s->width8;case 16:return s->width16;case 32:return s->width32;case 64:return s->width64;case 128:return s->width128;default:return false;}}
UmiCtSupportLevel umi_ct_atomic_semantics_support(const UmiCtAtomicSemantics*s,uint32_t b,bool q){if(s==NULL||!s->acquire_release||!umi_ct_atomic_width_lock_free(s,b))return UMI_CT_SUPPORT_NONE;if(q&&!s->sequential_consistency)return UMI_CT_SUPPORT_DEGRADED;return UMI_CT_SUPPORT_NATIVE;}
