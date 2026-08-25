/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/runtime_capability.c
 *
 * PURPOSE:
 *   runtime capability bitsets used to gate application launches.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_capability.h"

void umi_dr_runtime_capability_init(UmiDrRuntimeCapability *c){if(c!=NULL)*c=(UmiDrRuntimeCapability){0};}
uint64_t umi_dr_runtime_capability_missing(const UmiDrRuntimeCapability *c){return c==NULL?UINT64_MAX:(c->required & ~c->available);}
bool umi_dr_runtime_capability_satisfied(const UmiDrRuntimeCapability *c){return c!=NULL && umi_dr_runtime_capability_missing(c)==0U;}
