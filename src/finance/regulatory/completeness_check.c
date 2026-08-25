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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/completeness_check.h"

UmiStatus umi_reg_completeness_check(size_t required,size_t populated,double *out_score){ if(out_score==NULL||populated>required)return UMI_STATUS_INVALID_ARGUMENT; if(required==0U){*out_score=1.0;return UMI_STATUS_OK;} *out_score=(double)populated/(double)required; return UMI_STATUS_OK; }
