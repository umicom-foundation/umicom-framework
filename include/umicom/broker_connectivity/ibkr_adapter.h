/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/ibkr_adapter.h
 *
 * PURPOSE:
 *   Define the Interactive Brokers boundary mapping without vendor SDK types
 *   crossing the stable Framework C ABI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_IBKR_ADAPTER_H
#define UMICOM_BROKER_CONNECTIVITY_IBKR_ADAPTER_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIbkrAdapterConfig {
    char host[128];
    uint16_t port;
    int clientId;
    char account[64];
    int paperOnly;
    int readOnly;
} UmiIbkrAdapterConfig;

typedef struct UmiIbkrOrderMessage {
    char action[8];
    char orderType[16];
    char timeInForce[8];
    double quantity;
    double limitPrice;
    double stopPrice;
    int transmit;
} UmiIbkrOrderMessage;

void umi_ibkr_adapter_config_init(UmiIbkrAdapterConfig *config);
UmiStatus umi_ibkr_adapter_config_validate(const UmiIbkrAdapterConfig *config);
UmiStatus umi_ibkr_adapter_map_order(
    const UmiOrderRequest *request,
    const UmiIbkrAdapterConfig *config,
    UmiIbkrOrderMessage *outMessage);
UmiStatus umi_ibkr_adapter_map_status(
    const char *providerStatus,
    UmiOrderStatus *outStatus);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ibkr_adapter_config_archive_encode(const UmiIbkrAdapterConfig *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ibkr_adapter_config_archive_decode(const void *bytes, size_t byte_count,
    UmiIbkrAdapterConfig *value);

#ifdef __cplusplus
}
#endif
#endif
