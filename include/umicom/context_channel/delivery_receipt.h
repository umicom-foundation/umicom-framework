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
typedef struct UmiContextDeliveryReceiptStore {
    UmiContextDeliveryReceipt items[UMI_CONTEXT_DELIVERY_RECEIPT_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextDeliveryReceiptStore;
void umi_context_delivery_receipt_init(UmiContextDeliveryReceipt *record);
UmiStatus umi_context_delivery_receipt_validate(const UmiContextDeliveryReceipt *record);
void umi_context_delivery_receipt_store_init(UmiContextDeliveryReceiptStore *store);
UmiStatus umi_context_delivery_receipt_store_put(UmiContextDeliveryReceiptStore *store,const UmiContextDeliveryReceipt *record);
UmiStatus umi_context_delivery_receipt_store_remove(UmiContextDeliveryReceiptStore *store,const char *identity);
UmiContextDeliveryReceipt *umi_context_delivery_receipt_store_find(UmiContextDeliveryReceiptStore *store,const char *identity);
const UmiContextDeliveryReceipt *umi_context_delivery_receipt_store_find_const(const UmiContextDeliveryReceiptStore *store,const char *identity);
size_t umi_context_delivery_receipt_store_count(const UmiContextDeliveryReceiptStore *store);
UmiStatus umi_context_delivery_receipt_store_snapshot(const UmiContextDeliveryReceiptStore *store,UmiContextDeliveryReceipt *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
