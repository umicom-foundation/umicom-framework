/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/conformance_result.c
 *
 * PURPOSE:
 *   normalised pass, degraded and fail outcomes for one conformance case.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_result.h"

UmiStatus umi_fc_conformance_result_from_case(const UmiFcConformanceCase *item,UmiFcConformanceResult *out_result){unsigned req,hit; if(item==NULL||out_result==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_result=(UmiFcConformanceResult){0};(void)umi_fc_copy_text(out_result->case_id,sizeof(out_result->case_id),item->id);out_result->missing=item->required & ~item->actual;req=(unsigned)__builtin_popcountll(item->required);hit=(unsigned)__builtin_popcountll(item->required & item->actual);out_result->score=umi_fc_ratio((size_t)hit,(size_t)req);out_result->outcome=out_result->missing==0U?UMI_FC_PASS:(item->degradation_allowed?UMI_FC_DEGRADED:UMI_FC_FAIL);return UMI_STATUS_OK;}
