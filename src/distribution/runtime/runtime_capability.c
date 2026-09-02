/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/runtime_capability.c
 *
 * PURPOSE:
 *   runtime capability bitsets used to gate application launches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_capability.h"

/*
 * Initialise dr runtime capability from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_runtime_capability_init(UmiDrRuntimeCapability *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!=NULL)*c=(UmiDrRuntimeCapability){0};}
/*
 * Provide the dr runtime capability missing operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_runtime_capability_missing(const UmiDrRuntimeCapability *c){return c==NULL?UINT64_MAX:(c->required & ~c->available);}
/*
 * Provide the dr runtime capability satisfied operation used by this module and its client
 * applications.
 */
bool umi_dr_runtime_capability_satisfied(const UmiDrRuntimeCapability *c){return c!=NULL && umi_dr_runtime_capability_missing(c)==0U;}
