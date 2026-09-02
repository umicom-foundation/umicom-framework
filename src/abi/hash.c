/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/hash.c
 *
 * PURPOSE:
 *   Implement a deterministic FNV-1a style contract fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/hash.h"
#include <stddef.h>
#include <stdint.h>
/* Provide the abi hash bytes operation used by this module and its client applications. */
uint64_t umi_abi_hash_bytes(const void *data,size_t size){const unsigned char *p=(const unsigned char*)data;uint64_t h=UINT64_C(1469598103934665603);size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(data==NULL&&size!=0U)return 0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<size;++i){h^=(uint64_t)p[i];h*=UINT64_C(1099511628211);}return h;}
/* Provide the abi hash text operation used by this module and its client applications. */
uint64_t umi_abi_hash_text(const char *text){size_t n=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(text==NULL)return 0U;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(text[n]!='\0')++n;return umi_abi_hash_bytes(text,n);}
/* Provide the abi hash combine operation used by this module and its client applications. */
uint64_t umi_abi_hash_combine(uint64_t left,uint64_t right){return (left^(right+UINT64_C(0x9e3779b97f4a7c15)+(left<<6U)+(left>>2U)));}
