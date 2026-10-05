/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cash_planning/plan_private.h
 * PURPOSE: Keep plan storage private and share validation within the cash planning owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CASH_PLANNING_PRIVATE_H
#define UMICOM_CASH_PLANNING_PRIVATE_H
#include "umicom/cash_planning/plan.h"
struct UmiCashPlan
{
    UmiCashPlanConfig config;
    UmiCashPlanEntry entries[UMI_CASH_PLAN_CAPACITY];
    size_t count;
    uint64_t revision;
};
#endif
