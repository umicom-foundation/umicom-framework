/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_workstation/trading_workstation_internal.h
 *
 * PURPOSE:
 *   Share professional workstation calculations while canonical trading
 *   workspace, risk, execution and market-data services remain authoritative.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_WORKSTATION_INTERNAL_H
#define UMICOM_TRADING_WORKSTATION_INTERNAL_H

#include "umicom/trading_workstation/types.h"

typedef enum UmiTradingProfessionalPolicyKind {
    UMI_TRADING_POLICY_MIDPRICE = 1,
    UMI_TRADING_POLICY_SPREAD = 2,
    UMI_TRADING_POLICY_SPREAD_BPS = 3,
    UMI_TRADING_POLICY_CHANGE = 4,
    UMI_TRADING_POLICY_CHANGE_PERCENT = 5,
    UMI_TRADING_POLICY_RANGE = 6,
    UMI_TRADING_POLICY_RANGE_POSITION = 7,
    UMI_TRADING_POLICY_STALENESS = 8,
    UMI_TRADING_POLICY_HEALTH = 9,
    UMI_TRADING_POLICY_READINESS = 10,
    UMI_TRADING_POLICY_COUNT = 11,
    UMI_TRADING_POLICY_RATIO = 12,
    UMI_TRADING_POLICY_CAPACITY = 13,
    UMI_TRADING_POLICY_SCORE = 14,
    UMI_TRADING_POLICY_MOMENTUM = 15,
    UMI_TRADING_POLICY_VOLUME = 16,
    UMI_TRADING_POLICY_SPREAD_QUALITY = 17,
    UMI_TRADING_POLICY_CONTEXT = 18,
    UMI_TRADING_POLICY_IMBALANCE = 19,
    UMI_TRADING_POLICY_MICROPRICE = 20,
    UMI_TRADING_POLICY_LIQUIDITY = 21,
    UMI_TRADING_POLICY_PRESSURE = 22,
    UMI_TRADING_POLICY_SPREAD_TICKS = 23,
    UMI_TRADING_POLICY_VOLATILITY = 24,
    UMI_TRADING_POLICY_RATE = 25,
    UMI_TRADING_POLICY_ACTIVITY = 26,
    UMI_TRADING_POLICY_ORDER_RISK = 27,
    UMI_TRADING_POLICY_NOTIONAL = 28,
    UMI_TRADING_POLICY_DISTANCE = 29,
    UMI_TRADING_POLICY_UTILISATION = 30,
    UMI_TRADING_POLICY_PROGRESS = 31,
    UMI_TRADING_POLICY_POSITION_VALUE = 32,
    UMI_TRADING_POLICY_UNREALISED_PNL = 33,
    UMI_TRADING_POLICY_REALISED_PNL = 34,
    UMI_TRADING_POLICY_EXPOSURE = 35,
    UMI_TRADING_POLICY_ALERT = 36,
    UMI_TRADING_POLICY_EVENT = 37
} UmiTradingProfessionalPolicyKind;

typedef struct UmiTradingProfessionalPolicy {
    const char *id;
    const char *label;
    UmiTradingProfessionalPolicyKind kind;
    double threshold;
    double warning;
    uint32_t requiredFlags;
    double weightOne;
    double weightTwo;
    double weightThree;
    double weightFour;
} UmiTradingProfessionalPolicy;

UmiStatus umi_trading_professional_evaluate_policy(
    const UmiTradingProfessionalPolicy *policy,
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot);

#endif
