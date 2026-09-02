/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/conformance_case.c
 *
 * PURPOSE:
 *   deterministic conformance-case inputs used by automated frontend tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_case.h"

/*
 * Provide the fc conformance case make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_conformance_case_make(const char *id,uint64_t required,uint64_t actual,bool degradation_allowed,UmiFcConformanceCase *out_case){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_case==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out_case=(UmiFcConformanceCase){0};st=umi_fc_copy_text(out_case->id,sizeof(out_case->id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;out_case->required=required;out_case->actual=actual;out_case->degradation_allowed=degradation_allowed;return UMI_STATUS_OK;}
/*
 * Provide the fc conformance case missing operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_conformance_case_missing(const UmiFcConformanceCase *item){return item==NULL?0U:item->required & ~item->actual;}
