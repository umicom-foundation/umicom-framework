/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cash_planning/portfolio.h
 *
 * PURPOSE:
 *   Project reviewed cash plans together without mixing currencies or scales.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CASH_PLANNING_PORTFOLIO_H
#define UMICOM_CASH_PLANNING_PORTFOLIO_H

#include "umicom/cash_planning/plan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CASH_PLAN_PORTFOLIO_CAPACITY 16U

typedef struct UmiCashPlanPortfolioRow {
    UmiMoney opening;
    UmiMoney inflow;
    UmiMoney outflow;
    UmiMoney closing;
    size_t plan_count;
    size_t plans_with_shortfalls;
} UmiCashPlanPortfolioRow;

typedef struct UmiCashPlanPortfolio {
    UmiFinancialDate start, end;
    size_t plan_count, group_count;
    uint64_t revisions[UMI_CASH_PLAN_PORTFOLIO_CAPACITY];
    UmiCashPlanPortfolioRow groups[UMI_CASH_PLAN_PORTFOLIO_CAPACITY];
} UmiCashPlanPortfolio;

/* Project 1..16 distinct live plans on their owner thread. All inclusive date
 * ranges must match. The caller selects independent sources and is responsible
 * for avoiding double counting across different plan objects. Within each
 * currency and scale, checked sums are evaluated in input order. The result
 * lists groups in currency/scale order and revisions in input plan order.
 * A shortfall count refers to individual plans' own buffers and dates.
 *
 * No FX conversion, scale conversion, payment or intraday analysis occurs.
 * A rejected plan, incompatible horizon or overflowing intermediate leaves
 * the complete previous result unchanged. This read-only result owns copies
 * and can outlive the provided plans; revisions describe their current
 * in-memory states. Save source documents through the existing plan owner. */
UmiStatus UmiCashPlanProjectPortfolio(const UmiCashPlan *const *plans, size_t count,
    UmiCashPlanPortfolio *out);

#ifdef __cplusplus
}
#endif
#endif
