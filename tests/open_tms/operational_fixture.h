/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/operational_fixture.h
 *
 * PURPOSE:
 *   Supply small valid Treasury records and explicit regression failure checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_TREASURY_OPERATIONAL_FIXTURE_H
#define UMICOM_TEST_TREASURY_OPERATIONAL_FIXTURE_H
#include "umicom/open_tms/service.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)
/* Literals supply valid fields without copying production validation logic.
 * Tests use memcpy before refusal when comparing full objects, including padding. */
static inline UmiOpenTmsCashRecord CashRow(void)
{ UmiOpenTmsCashRecord row = {"account", "GBP", 1000, 1000, 200, 500, 850, 1U}; return row; }
static inline UmiOpenTmsLiquidityBucket LiquidityRow(void)
{ UmiOpenTmsLiquidityBucket row = {"day", 1000U, 100, 150, 20}; return row; }
static inline UmiOpenTmsPosition PositionRow(void)
{ UmiOpenTmsPosition row = {"account", "bond", "GBP", 2, 90, 100, 5, 1U}; return row; }
static inline UmiOpenTmsRiskLimit RiskRow(void)
{ UmiOpenTmsRiskLimit row = {"limit", "desk", 100, 80, 1U, 1}; return row; }
static inline UmiOpenTmsSettlementRecord SettlementRow(void)
{ UmiOpenTmsSettlementRecord row = {"payment", "trade", "", "GBP", -100, 1000U, UMI_OPEN_TMS_SETTLEMENT_PENDING, 1U}; return row; }
static inline UmiOpenTmsReconciliationItem ReconciliationRow(void)
{ UmiOpenTmsReconciliationItem row = {"match", "source", "target", 100, 102, 0.5, 0, 0}; return row; }
#endif
