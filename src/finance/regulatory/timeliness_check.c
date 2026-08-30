/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/timeliness_check.c
 *
 * PURPOSE:
 *   Evaluate source-data timeliness against a maximum regulatory age.
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

#include "umicom/finance/regulatory/timeliness_check.h"

int umi_reg_timeliness_check(int64_t as_of_ms,int64_t now_ms,int64_t maximum_age_ms){ if(as_of_ms<0||now_ms<0||maximum_age_ms<0||as_of_ms>now_ms)return 0; return now_ms-as_of_ms<=maximum_age_ms?1:0; }
