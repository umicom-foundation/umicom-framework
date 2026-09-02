/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/feature_parity.c
 *
 * PURPOSE:
 *   missing, extra and shared capability calculation between canonical and candidate renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/feature_parity.h"

/*
 * Provide the fc feature parity calculate operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_feature_parity_calculate(uint64_t canonical,uint64_t candidate,UmiFcFeatureParity *out_parity){unsigned req,hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_parity==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_parity->shared=canonical&candidate;out_parity->missing=canonical&~candidate;out_parity->extra=candidate&~canonical;req=(unsigned)__builtin_popcountll(canonical);hit=(unsigned)__builtin_popcountll(out_parity->shared);out_parity->score=umi_fc_ratio((size_t)hit,(size_t)req);return UMI_STATUS_OK;}
