/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/completeness_check.c
 *
 * PURPOSE:
 *   Compute regulatory record completeness from required and populated fields.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/completeness_check.h"

/*
 * Provide the reg completeness check operation used by this module and its client
 * applications.
 */
UmiStatus umi_reg_completeness_check(size_t required,size_t populated,double *out_score){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_score==NULL||populated>required)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(required==0U){*out_score=1.0;return UMI_STATUS_OK;} *out_score=(double)populated/(double)required; return UMI_STATUS_OK; }
