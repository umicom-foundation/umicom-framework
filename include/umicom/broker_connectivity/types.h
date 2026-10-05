/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/types.h
 *
 * PURPOSE:
 *   Define provider-neutral broker connectivity, account, order, execution,
 *   position, pre-trade risk, audit and adapter-boundary evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_TYPES_H
#define UMICOM_BROKER_CONNECTIVITY_TYPES_H

#include <stdint.h>
#include "umicom/base/value_archive.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BROKER_CONNECTIVITY_ID_CAPACITY 96U
#define UMI_BROKER_CONNECTIVITY_LABEL_CAPACITY 128U

typedef struct UmiBrokerConnectivityInput {
    const char *label;

    double value;
    double secondaryValue;
    double referenceValue;
    double limitValue;
    double warningValue;

    double netLiquidation;
    double cash;
    double buyingPower;
    double marginUsed;
    double marginLimit;
    double dailyPnl;
    double unrealisedPnl;
    double realisedPnl;

    double orderQuantity;
    double orderPrice;
    double orderNotional;
    double workingOrderNotional;
    double positionQuantity;
    double maxOrderQuantity;
    double maxOrderNotional;
    double maxPositionQuantity;
    double maxDailyLoss;
    double grossExposure;
    double netExposure;
    double concentrationPercent;
    double leveragePercent;
    double priceDeviationPercent;

    double orderedQuantity;
    double filledQuantity;
    double averageFillPrice;
    double referencePrice;
    double commission;
    double slippage;

    uint64_t ageMilliseconds;
    uint64_t latencyMilliseconds;
    uint64_t reconnectAttempts;

    uint64_t mappedCount;
    uint64_t totalCount;
    uint64_t matchedCount;
    uint64_t mismatchCount;
    uint64_t duplicateCount;
    uint64_t lateCount;
    uint64_t correctionCount;
    uint64_t rejectionCount;
    uint64_t auditCount;
    uint64_t persistedAuditCount;
    uint64_t gapCount;

    int connected;
    int authenticated;
    int providerReady;
    int accountReady;
    int orderReady;
    int executionReady;
    int positionReady;
    int riskReady;
    int auditReady;
    int paperCapable;
    int liveCapable;
    int liveApproved;
    int killSwitchEngaged;
    int restrictedInstrument;
    int trusted;

    uint64_t revision;
} UmiBrokerConnectivityInput;

typedef struct UmiBrokerConnectivitySnapshot {
    char id[UMI_BROKER_CONNECTIVITY_ID_CAPACITY];
    char label[UMI_BROKER_CONNECTIVITY_LABEL_CAPACITY];
    double value;
    double secondaryValue;
    double score;
    double ratio;
    int ready;
    int attention;
    int blocked;
    int approvalRequired;
    uint64_t revision;
} UmiBrokerConnectivitySnapshot;

typedef UmiStatus (*UmiBrokerConnectivityEvaluator)(
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot);

void umi_broker_connectivity_input_init(UmiBrokerConnectivityInput *input);
void umi_broker_connectivity_snapshot_init(UmiBrokerConnectivitySnapshot *snapshot);
UmiStatus umi_broker_connectivity_snapshot_validate(
    const UmiBrokerConnectivitySnapshot *snapshot);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_broker_connectivity_snapshot_archive_encode(const UmiBrokerConnectivitySnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_broker_connectivity_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiBrokerConnectivitySnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
