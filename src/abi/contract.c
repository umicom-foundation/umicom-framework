/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/contract.c
 *
 * PURPOSE:
 *   Compute one fingerprint from descriptor identity and public contract records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/contract.h"
#include "umicom/abi/hash.h"
uint64_t umi_abi_contract_fingerprint(const UmiAbiDescriptor *d){uint64_t h;size_t i;if(umi_abi_descriptor_validate(d)!=UMI_STATUS_OK)return 0U;h=umi_abi_hash_text(d->abi_id);h=umi_abi_hash_combine(h,(uint64_t)d->abi_version);for(i=0U;i<d->feature_count;++i){h=umi_abi_hash_combine(h,umi_abi_hash_text(d->features[i].feature_id));h=umi_abi_hash_combine(h,(uint64_t)d->features[i].version);}for(i=0U;i<d->symbol_count;++i)h=umi_abi_hash_combine(h,umi_abi_hash_text(d->symbols[i].name));for(i=0U;i<d->layout_count;++i){h=umi_abi_hash_combine(h,umi_abi_hash_text(d->layouts[i].name));h=umi_abi_hash_combine(h,(uint64_t)d->layouts[i].size);}return h;}
