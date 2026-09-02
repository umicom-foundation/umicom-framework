/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/delivery_receipt.h
 *
 * PURPOSE:
 *   Retain per-target context delivery evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_DELIVERY_RECEIPT_H
#define UMICOM_CONTEXT_CHANNEL_DELIVERY_RECEIPT_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_DELIVERY_RECEIPT_MAX_ITEMS 256U
/**
 * Represent the context delivery receipt data shared with callers of this public contract.
 */
typedef struct UmiContextDeliveryReceipt {
    uint32_t structure_size;
    char receipt_id[UMI_CONTEXT_TEXT_CAPACITY];
    char context_id[UMI_CONTEXT_TEXT_CAPACITY];
    char subscription_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char panel_id[UMI_CONTEXT_TEXT_CAPACITY];
    UmiContextDeliveryState state;
    UmiStatus status;
    uint64_t delivered_at_ms;
    uint64_t revision;
} UmiContextDeliveryReceipt;
/**
 * Represent the context delivery receipt store data shared with callers of this public
 * contract.
 */
typedef struct UmiContextDeliveryReceiptStore {
    UmiContextDeliveryReceipt items[UMI_CONTEXT_DELIVERY_RECEIPT_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextDeliveryReceiptStore;
/**
 * Initialise context delivery receipt from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_delivery_receipt_init(UmiContextDeliveryReceipt *record);
/**
 * Check that context delivery receipt satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_context_delivery_receipt_validate(const UmiContextDeliveryReceipt *record);
/**
 * Initialise context delivery receipt store from caller-provided values so later
 * operations receive a known state.
 */
void umi_context_delivery_receipt_store_init(UmiContextDeliveryReceiptStore *store);
/**
 * Provide the context delivery receipt store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_delivery_receipt_store_put(UmiContextDeliveryReceiptStore *store,const UmiContextDeliveryReceipt *record);
/**
 * Remove context delivery receipt store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_delivery_receipt_store_remove(UmiContextDeliveryReceiptStore *store,const char *identity);
/**
 * Find context delivery receipt store while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiContextDeliveryReceipt *umi_context_delivery_receipt_store_find(UmiContextDeliveryReceiptStore *store,const char *identity);
/**
 * Provide the context delivery receipt store find const operation used by this module and
 * its client applications.
 */
const UmiContextDeliveryReceipt *umi_context_delivery_receipt_store_find_const(const UmiContextDeliveryReceiptStore *store,const char *identity);
/**
 * Return the number of records represented by context delivery receipt store without
 * changing their state.
 */
size_t umi_context_delivery_receipt_store_count(const UmiContextDeliveryReceiptStore *store);
/**
 * Provide the context delivery receipt store snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_context_delivery_receipt_store_snapshot(const UmiContextDeliveryReceiptStore *store,UmiContextDeliveryReceipt *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
