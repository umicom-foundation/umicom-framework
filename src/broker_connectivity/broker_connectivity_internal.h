/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/broker_connectivity_internal.h
 *
 * PURPOSE:
 *   Share provider/account/order/execution/risk/audit policy evaluation while
 *   canonical trading and brokerage engines remain authoritative.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_INTERNAL_H
#define UMICOM_BROKER_CONNECTIVITY_INTERNAL_H

#include "umicom/broker_connectivity/types.h"

typedef enum UmiBrokerConnectivityPolicyKind {
    UMI_BROKER_POLICY_READINESS = 1,
    UMI_BROKER_POLICY_CONNECTION,
    UMI_BROKER_POLICY_AUTH,
    UMI_BROKER_POLICY_AGE,
    UMI_BROKER_POLICY_LATENCY,
    UMI_BROKER_POLICY_RECONNECT,
    UMI_BROKER_POLICY_COVERAGE,
    UMI_BROKER_POLICY_APPROVAL,
    UMI_BROKER_POLICY_HEALTH,
    UMI_BROKER_POLICY_FRESHNESS,
    UMI_BROKER_POLICY_VALUE,
    UMI_BROKER_POLICY_UTILISATION,
    UMI_BROKER_POLICY_PNL,
    UMI_BROKER_POLICY_RECONCILIATION,
    UMI_BROKER_POLICY_ORDER_GATE,
    UMI_BROKER_POLICY_CANCEL_GATE,
    UMI_BROKER_POLICY_REPLACE_GATE,
    UMI_BROKER_POLICY_INTEGRITY,
    UMI_BROKER_POLICY_REJECTION,
    UMI_BROKER_POLICY_PROGRESS,
    UMI_BROKER_POLICY_DISTANCE,
    UMI_BROKER_POLICY_NOTIONAL,
    UMI_BROKER_POLICY_LIMIT,
    UMI_BROKER_POLICY_LOSS_LIMIT,
    UMI_BROKER_POLICY_BUYING_POWER,
    UMI_BROKER_POLICY_PRICE_BAND,
    UMI_BROKER_POLICY_KILL_SWITCH,
    UMI_BROKER_POLICY_SEQUENCE,
    UMI_BROKER_POLICY_GAP
} UmiBrokerConnectivityPolicyKind;

typedef struct UmiBrokerConnectivityPolicy {
    const char *id;
    const char *label;
    UmiBrokerConnectivityPolicyKind kind;
    double threshold;
    double warning;
    double weightOne;
    double weightTwo;
    double weightThree;
    double weightFour;
} UmiBrokerConnectivityPolicy;

UmiStatus umi_broker_connectivity_evaluate_policy(
    const UmiBrokerConnectivityPolicy *policy,
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot);

#endif
