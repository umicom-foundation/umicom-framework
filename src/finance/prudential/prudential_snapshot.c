/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/prudential_snapshot.c
 *
 * PURPOSE:
 *   Capture aggregate capital, liquidity and stress readiness evidence.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/prudential_snapshot.h"

UmiStatus umi_pru_prudential_snapshot_init(UmiPrudentialSnapshot *snapshot,uint64_t as_of_millis,double cet1,double total,double lcr,double nsfr,size_t failed,size_t breached) { if(snapshot==NULL||as_of_millis==0U||!umi_pru_number_valid(cet1)||!umi_pru_number_valid(total)||!umi_pru_number_valid(lcr)||!umi_pru_number_valid(nsfr))return UMI_STATUS_INVALID_ARGUMENT; snapshot->as_of_millis=as_of_millis; snapshot->cet1_ratio=cet1; snapshot->total_capital_ratio=total; snapshot->lcr=lcr; snapshot->nsfr=nsfr; snapshot->failed_stress_metrics=failed; snapshot->breached_limits=breached; return UMI_STATUS_OK; }
