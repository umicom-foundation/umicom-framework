/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/feature_parity.c
 *
 * PURPOSE:
 *   missing, extra and shared capability calculation between canonical and candidate renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/feature_parity.h"

UmiStatus umi_fc_feature_parity_calculate(uint64_t canonical,uint64_t candidate,UmiFcFeatureParity *out_parity){unsigned req,hit;if(out_parity==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_parity->shared=canonical&candidate;out_parity->missing=canonical&~candidate;out_parity->extra=candidate&~canonical;req=(unsigned)__builtin_popcountll(canonical);hit=(unsigned)__builtin_popcountll(out_parity->shared);out_parity->score=umi_fc_ratio((size_t)hit,(size_t)req);return UMI_STATUS_OK;}
