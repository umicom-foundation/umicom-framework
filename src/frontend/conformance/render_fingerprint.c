/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/render_fingerprint.c
 *
 * PURPOSE:
 *   stable render fingerprints independent of toolkit-specific object addresses.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/render_fingerprint.h"

uint64_t umi_fc_render_fingerprint_begin(const char *surface_id){return umi_fc_hash_text(surface_id);}
uint64_t umi_fc_render_fingerprint_add(uint64_t seed,const char *role,const char *semantic_id,int32_t order){uint64_t h=umi_fc_hash_combine(seed,umi_fc_hash_text(role));h=umi_fc_hash_combine(h,umi_fc_hash_text(semantic_id));return umi_fc_hash_combine(h,(uint64_t)(uint32_t)order);}
