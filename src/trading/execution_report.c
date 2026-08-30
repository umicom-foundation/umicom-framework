/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/execution_report.c
 *
 * PURPOSE:
 *   Validate fill/execution reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of execution report. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/execution_report.h"
#include "umicom/finance/identifier.h"
int umi_execution_report_valid(const UmiExecutionReport *r){return r!=NULL&&umi_financial_id_valid(&r->execution_id)&&umi_financial_id_valid(&r->client_order_id)&&r->fill_quantity>0.0&&r->fill_price>0.0;}
