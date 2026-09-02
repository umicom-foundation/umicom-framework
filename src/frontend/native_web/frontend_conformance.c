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

/* Provide the popcount64 operation used by this module and its client applications. */
static unsigned popcount64(uint64_t v){unsigned n=0U;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(v!=0U){n+=(unsigned)(v&1U);v>>=1U;}return n;}
/*
 * Provide the native web frontend conformance operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_frontend_conformance(uint64_t required_flags,const UmiNativeWebBrowserCapability *provided,UmiNativeWebConformanceResult *out_result){unsigned req,hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(provided==NULL||out_result==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_result->required_flags=required_flags;out_result->provided_flags=provided->flags;out_result->missing_flags=required_flags&~provided->flags;req=popcount64(required_flags);hit=popcount64(required_flags&provided->flags);out_result->score_percent=req==0U?100U:(unsigned)((100U*hit)/req);out_result->conformant=out_result->missing_flags==0U;return UMI_STATUS_OK;}

