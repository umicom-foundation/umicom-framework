/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/frontend_conformance.c
 *
 * PURPOSE:
 *   Evaluate semantic capability parity between native-web and other Framework frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/frontend_conformance.h"

static unsigned popcount64(uint64_t v){unsigned n=0U;while(v!=0U){n+=(unsigned)(v&1U);v>>=1U;}return n;}
UmiStatus umi_native_web_frontend_conformance(uint64_t required_flags,const UmiNativeWebBrowserCapability *provided,UmiNativeWebConformanceResult *out_result){unsigned req,hit;if(provided==NULL||out_result==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_result->required_flags=required_flags;out_result->provided_flags=provided->flags;out_result->missing_flags=required_flags&~provided->flags;req=popcount64(required_flags);hit=popcount64(required_flags&provided->flags);out_result->score_percent=req==0U?100U:(unsigned)((100U*hit)/req);out_result->conformant=out_result->missing_flags==0U;return UMI_STATUS_OK;}

