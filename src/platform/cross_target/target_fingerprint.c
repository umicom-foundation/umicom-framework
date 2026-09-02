/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/target_fingerprint.c
 *
 * PURPOSE:
 *   Compute deterministic target/runtime fingerprints for cache, release and conformance evidence.
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

#include "umicom/platform/cross_target/target_fingerprint.h"

/*
 * Provide the ct target fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_target_fingerprint(const UmiCtTarget*t,uint64_t f,const char*a,uint32_t p){uint64_t h;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||a==NULL)return 0U;h=umi_ct_hash_text(0U,t->triple);h=umi_ct_hash_text(h,t->vendor);h=umi_ct_hash_bytes(h,&t->architecture,sizeof(t->architecture));h=umi_ct_hash_bytes(h,&t->operating_system,sizeof(t->operating_system));h=umi_ct_hash_bytes(h,&t->environment,sizeof(t->environment));h=umi_ct_hash_bytes(h,&f,sizeof(f));h=umi_ct_hash_text(h,a);h=umi_ct_hash_bytes(h,&p,sizeof(p));return h;}
