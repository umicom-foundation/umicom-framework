/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/regulatory_snapshot.c
 *
 * PURPOSE:
 *   Aggregate report, submission, capital and liquidity readiness evidence.
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

#include "umicom/finance/regulatory/regulatory_snapshot.h"

/*
 * Initialise reg regulatory snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_reg_regulatory_snapshot_init(UmiRegulatorySnapshot *s,size_t due,size_t ready,size_t rejected,double capital,double liquidity){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||ready>due||!umi_reg_number_valid(capital)||!umi_reg_number_valid(liquidity))return UMI_STATUS_INVALID_ARGUMENT;s->reports_due=due;s->reports_ready=ready;s->rejected_submissions=rejected;s->capital_ratio=capital;s->liquidity_ratio=liquidity;return UMI_STATUS_OK; }
/*
 * Provide the reg regulatory snapshot ready operation used by this module and its client
 * applications.
 */
int umi_reg_regulatory_snapshot_ready(const UmiRegulatorySnapshot *s,double minc,double minl){ return s!=NULL&&umi_reg_number_valid(minc)&&umi_reg_number_valid(minl)&&s->reports_ready==s->reports_due&&s->rejected_submissions==0U&&s->capital_ratio>=minc&&s->liquidity_ratio>=minl?1:0; }
